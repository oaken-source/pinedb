
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
  #include "datastore/datastore.h"

  #include <stdio.h>
  #include <time.h>
  #include <stdint.h>

  struct yystoken
  {
    void *v;
    unsigned int l;
    unsigned int c;
  };

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

  struct statement
  {
    query_result*(*func)(query_arg*);
    const char *func_name;
    query_arg *args;
    size_t nargs;
  };

  void
  statement_init_impl(struct statement *s, query_result*(*func)(query_arg*), const char *func_name)
  {
    s->func = func;
    s->func_name = func_name;
    s->args = NULL;
    s->nargs = 0;
  }

  #define statement_init(S, TYPE) statement_init_impl(&(S), &TYPE, # TYPE)

  #define statement_push_arg(S, TYPE, A) \
      do { \
        ++((S).nargs); \
        (S).args = realloc((S).args, sizeof(*((S).args)) * (S).nargs); \
        assert_inner((S).args, "realloc"); \
        (S).args[(S).nargs - 1].TYPE = (A); \
      } while (0)

%}

%union {
  int boolean;
  int integer;
  char *string;
  void *pointer;
  struct yystoken token;
  struct statement statement;
}

%token <token> CREATE DROP SHOW USE SCHEMA SCHEMATA TABLE TABLES INT VARCHAR IF_NOT_EXISTS IF_EXISTS UNDEFINED
%token <token> IDENTIFIER BT_IDENTIFIER NUMBER

%type <boolean> nt_if_exists nt_if_not_exists
%type <integer> nt_length nt_optional_length
%type <string> nt_name
%type <pointer> nt_table_create_definitions
%type <statement> nt_statement


%destructor { free($$.v); } IDENTIFIER BT_IDENTIFIER NUMBER

%error-verbose

%%

/* dispatch statements */
nt_input:
    /* empty */
  | nt_input nt_statement ';'
      {
        clock_gettime(CLOCK_MONOTONIC, &queryparser_time);
        query_result *r = $2.func($2.args);
        free($2.args);
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
        statement_push_arg($$, string, $4);
        statement_push_arg($$, boolean, !$3);
      }
  | CREATE TABLE nt_if_not_exists nt_name '(' nt_table_create_definitions ')' /* nt_table_options */
      {
        queryparser_set_current_token($1);
        statement_init($$, query_create_table);
        statement_push_arg($$, string, $4);
        statement_push_arg($$, boolean, !$3);
        statement_push_arg($$, pointer, $6);
      }
  | DROP SCHEMA nt_if_exists nt_name
      {
        queryparser_set_current_token($1);
        statement_init($$, query_drop_schema);
        statement_push_arg($$, string, $4);
        statement_push_arg($$, boolean, !$3);
      }
  | DROP TABLE nt_if_exists nt_name
      {
        queryparser_set_current_token($1);
        statement_init($$, query_drop_table);
        statement_push_arg($$, string, $4);
        statement_push_arg($$, boolean, !$3);
      }
  | SHOW SCHEMATA
      {
        queryparser_set_current_token($1);
        statement_init($$, query_show_schemata);
      }
  | SHOW TABLES
      {
        queryparser_set_current_token($1);
        statement_init($$, query_show_tables);
      }
  | USE nt_name
      {
        queryparser_set_current_token($1);
        statement_init($$, query_use);
        statement_push_arg($$, string, $2);
      }
;

/* create table statement specific rules */
nt_table_create_definitions:
    nt_table_create_definition { $$ = NULL; }
  | nt_table_create_definitions ',' nt_table_create_definition { $$ = NULL; }
;

nt_table_create_definition:
    nt_name nt_column_definition
;

nt_column_definition:
    nt_data_type
;

nt_data_type:
    INT nt_optional_length
  | VARCHAR nt_length
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
        int errnum = errno;
        errno = 0;
        $$ = strtol($2.v, NULL, 10);
        int query_err_conversion = (errno == ERANGE || errno == EINVAL);
        parser_assert_err_weak(QUERY_ERR_CONVERSION, !query_err_conversion, $2.v);
        errno = errnum;
        free($2.v);
        assert_inner(!query_err_conversion, "strtol");
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

  int err = 0;
  int res;
  do
    {
      querylexer_restart();
      res = yyparse();
      err |= (res != 0);
      assert_weak(!res, "%s: yyparse", queryparser_file);
    }
  while (res != 0);

  yylex_destroy();

  printf("\n");

  return -err;
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
