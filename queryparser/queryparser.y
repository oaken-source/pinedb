
/******************************************************************************
 *                        pinedb in-memory database                           *
 *                                                                            *
 *    Copyright (C) 2014  Andreas Grapentin                                   *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/

%{

  #include <config.h>

  #include "query.h"
  #include "assertions.h"
  #include "parser_tokens.h"

  #include "datastore/datastore.h"

  #include <stdio.h>
  #include <time.h>
  #include <stdint.h>

  // required forward declarations
  int yylex(void);
  static int yyerror(char const *error);

  // parser state
  const char *queryparser_file;
  unsigned int queryparser_line;
  unsigned int queryparser_char;

  struct timespec queryparser_time;

  #define queryparser_set_current_token(S) \
      do { \
        queryparser_line = (S).l; \
        queryparser_char = (S).c; \
      } while (0)

  static int string_to_int(int *res, const char *str) may_fail;

  #define statement_set_arg(S, N, TYPE, A) \
      do { \
        (S).args[(N)].TYPE = (A); \
      } while (0)

%}

%union {
  struct yystoken token;

  int boolean;
  int integer;
  char *string;

  struct tok_statement statement;
  struct tok_column column;
  struct tok_datatype datatype;
  struct tok_tbl_name tbl_name;

  tok_column_vector columns;
}

%token <token> CREATE DROP SHOW USE SCHEMA SCHEMATA TABLE TABLES INT VARCHAR
%token <token> IF_NOT_EXISTS IF_EXISTS UNDEFINED FROM IN
%token <token> IDENTIFIER BT_IDENTIFIER NUMBER

%type <boolean> nt_if_exists nt_if_not_exists
%type <integer> nt_length nt_optional_length
%type <string> nt_name nt_from_or_in_schema

%type <statement> nt_statement
%type <column> nt_table_create_definition
%type <datatype> nt_data_type nt_column_definition
%type <tbl_name> nt_tbl_name

%type <columns> nt_table_create_definitions


%destructor {
  free($$.v);
} IDENTIFIER BT_IDENTIFIER NUMBER

%destructor {
  free($$);
} nt_name

%destructor {
  size_t i;
  for (i = 0; i < $$.nitems; ++i)
    free($$.items[i].name);
  vector_clear(&$$);
} nt_table_create_definitions

%destructor {
  free($$.name);
} nt_table_create_definition

%destructor {
  free($$);
} nt_from_or_in_schema

%destructor {
  free($$.schema);
  free($$.table);
} nt_tbl_name

%destructor {
  if (($$.func == &query_create_schema)
      || ($$.func == &query_drop_schema)
      || ($$.func == &query_show_tables)
      || ($$.func == &query_use))
    {
      free($$.args[0].string);
    }
  else if ($$.func == &query_create_table)
    {
      free($$.args[0].string);
      free($$.args[1].string);
      size_t i;
      for (i = 0; i < $$.args[4].size; ++i)
        free(((struct tok_column*)$$.args[3].pointer)[i].name);
      free($$.args[3].pointer);
    }
  else if ($$.func == &query_drop_table)
    {
      free($$.args[0].string);
      free($$.args[1].string);
    }
} nt_statement

%error-verbose

%%

/* dispatch statements */
nt_input:
    /* empty */
  | nt_input nt_statement ';'
      {
        clock_gettime(CLOCK_MONOTONIC, &queryparser_time);
        query_result *r = $2.func($2.args);
        assert_inner(r, "%s", $2.func_name);
        int res = query_result_print(r);
        query_result_destroy(r);
        assert_inner(!res, "query_result_print");
      }
;

nt_statement:
    CREATE SCHEMA nt_if_not_exists nt_name /* nt_schema_create_definitions */
      {
        queryparser_set_current_token($1);
        statement_init($$, query_create_schema);
        statement_set_arg($$, 0, string, $4);
        statement_set_arg($$, 1, boolean, !$3);
      }
  | CREATE TABLE nt_if_not_exists nt_tbl_name '(' nt_table_create_definitions ')' /* nt_table_options */
      {
        queryparser_set_current_token($1);
        statement_init($$, query_create_table);
        statement_set_arg($$, 0, string, $4.table);
        statement_set_arg($$, 1, string, $4.schema);
        statement_set_arg($$, 2, boolean, !$3);
        statement_set_arg($$, 3, pointer, $6.items);
        statement_set_arg($$, 4, size, $6.nitems);
      }
  | DROP SCHEMA nt_if_exists nt_name
      {
        queryparser_set_current_token($1);
        statement_init($$, query_drop_schema);
        statement_set_arg($$, 0, string, $4);
        statement_set_arg($$, 1, boolean, !$3);
      }
  | DROP TABLE nt_if_exists nt_tbl_name
      {
        queryparser_set_current_token($1);
        statement_init($$, query_drop_table);
        statement_set_arg($$, 0, string, $4.table);
        statement_set_arg($$, 1, string, $4.schema);
        statement_set_arg($$, 2, boolean, !$3);
      }
  | SHOW SCHEMATA
      {
        queryparser_set_current_token($1);
        statement_init($$, query_show_schemata);
      }
  | SHOW TABLES nt_from_or_in_schema
      {
        queryparser_set_current_token($1);
        statement_init($$, query_show_tables);
        statement_set_arg($$, 0, string, $3);
      }
  | USE nt_name
      {
        queryparser_set_current_token($1);
        statement_init($$, query_use);
        statement_set_arg($$, 0, string, $2);
      }
;

/* create table statement specific rules */
nt_table_create_definitions:
    nt_table_create_definition
      {
        vector_init(&$$);
        int res = vector_push(&$$, $1);
        if (res)
          {
            assert_weak(!res, "vector_push");
            vector_clear(&$$);
            parser_assert_err(QUERY_ERR_FAILED, !res);
          }
      }
  | nt_table_create_definitions ',' nt_table_create_definition
      {
        $$ = $1;
        int res = vector_push(&$$, $3);
        if (res)
          {
            assert_weak(0, "vector_push");
            vector_clear(&$$);
            parser_assert_err(QUERY_ERR_FAILED, 0);
          }
      }
;

nt_table_create_definition:
    nt_name nt_column_definition                { $$.name = $1; $$.type = $2; }
;

nt_column_definition:
    nt_data_type                                { $$ = $1; }
;

nt_data_type:
    INT nt_optional_length
      {
        $$.width = $2;
        if (!$$.width)
          $$.width = 11;
        $$.type = DATATYPE_INT;
      }
  | VARCHAR nt_length
      {
        $$.width = $2;
        $$.type = DATATYPE_VARCHAR;
      }
;

/* composite value rules */
nt_from_or_in_schema:
    /* empty */                                 { $$ = NULL; }
  | FROM nt_name                                { $$ = $2; }
  | IN nt_name                                  { $$ = $2; }
;

nt_tbl_name:
    nt_name                                     { $$.table = $1; $$.schema = NULL; }
  | nt_name '.' nt_name                         { $$.table = $3; $$.schema = $1; }
;

/* simple value rules */
nt_name:
    IDENTIFIER                                  { $$ = $1.v; }
  | BT_IDENTIFIER                               { $$ = $1.v; }
;

nt_optional_length:
    /* empty */                                 { $$ = 0; }
  | nt_length                                   { $$ = $1; }
;

nt_length:
    '(' NUMBER ')'
      {
        queryparser_set_current_token($2);
        int res = string_to_int(&$$, $2.v);
        free($2.v);
        assert_inner(!res, "string_to_int");
      }
;

/* simple boolean rules */
nt_if_exists:
    /* empty */                                 { $$ = 0; }
  | IF_EXISTS                                   { $$ = 1; }
;

nt_if_not_exists:
    /* empty */                                 { $$ = 0; }
  | IF_NOT_EXISTS                               { $$ = 1; }
;

%%

#undef queryparser_set_current_token

static int
string_to_int (int *res, const char *str)
{
  int errnum = errno;
  errno = 0;
  *res = strtol(str, NULL, 10);
  parser_assert_err(QUERY_ERR_CONVERSION, errno != ERANGE && errno != EINVAL, str);
  errno = errnum;

  return 0;
}

extern void yy_scan_string(const char *data);
extern void yylex_destroy(void);
extern void querylexer_restart(void);

int
queryparser_parse_from_file (const char *filename, const char *data)
{
  queryparser_file = filename;
  querylexer_restart();

  yy_scan_string(data);
  int res = yyparse();
  assert_inner(!res, "%s: yyparse", queryparser_file);
  yylex_destroy();

  return 0;
}

int
queryparser_parse_from_stdin (void)
{
  queryparser_file = "<stdin>";

  int res;
  do
    {
      querylexer_restart();
      res = yyparse();
      assert_weak(!res, "%s: yyparse", queryparser_file);
    }
  while (res != 0);

  yylex_destroy();

  printf("\n");

  return 0;
}

extern unsigned int yylineno;
extern unsigned int yycharno;
extern unsigned int yyleng;

static int
yyerror (const char *error)
{
  queryparser_line = yylineno;
  queryparser_char = yycharno - yyleng;

  parser_assert_err(QUERY_ERR_PARSE, 0, error);

  return 0;
}
