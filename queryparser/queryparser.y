
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

  #define YYSTYPE yystoken

  struct yystoken
  {
    void *v;
    unsigned int l;
    unsigned int c;
  };
  typedef struct yystoken yystoken;

  int yylex(void);
  static int yyerror(char const *error);

  // parser state
  const char *queryparser_file;
  unsigned int queryparser_line;
  unsigned int queryparser_char;

  struct timespec queryparser_time;

  #define queryparser_entry(S) \
      do { \
        queryparser_line = S.l; \
        queryparser_char = S.c; \
        clock_gettime(CLOCK_MONOTONIC, &queryparser_time); \
      } while (0)

%}

%token CREATE DROP SHOW USE SCHEMA SCHEMATA TABLE TABLES INT VARCHAR IF_NOT_EXISTS IF_EXISTS identifier
%token IDENTIFIER BT_IDENTIFIER NUMBER

%destructor { free($$.v); } IDENTIFIER BT_IDENTIFIER NUMBER

%error-verbose

%%

/*  */
nt_statements:
    /* empty */
  | nt_statements nt_statement
;

nt_statement:
    CREATE nt_create_statement
  | DROP nt_drop_statement
  | SHOW nt_show_statement
  | USE nt_use_statement
;

nt_create_statement:
    SCHEMA nt_if_not_exists nt_db_name nt_schema_create_definitions ';'
      {
        queryparser_entry($3);
        int res = query_create_schema($3.v, !$2.v);
        free($3.v);
        assert_inner(!res, "query_create_schema");
      }
  | TABLE nt_if_not_exists nt_tbl_name '(' nt_table_create_definitions ')' nt_table_options ';'
      {
        queryparser_entry($3);
        int res = query_create_table($3.v, !$2.v, $4.v, $5.v);
        free($3.v);
        assert_inner(!res, "query_create_table");
      }
;

nt_if_not_exists:
    /* empty */
      {
        $$.v = (void*)0;
      }
  | IF_NOT_EXISTS
      {
        $$.v = (void*)1;
      }
;

nt_db_name:
    IDENTIFIER
      {
        $$ = $1;
      }
  | BT_IDENTIFIER
      {
        $$ = $1;
      }
;

nt_schema_create_definitions:
    /* FIXME: none are supported yet */
;

nt_tbl_name:
    IDENTIFIER
      {
        $$ = $1;
      }
  | BT_IDENTIFIER
      {
        $$ = $1;
      }
;

nt_table_create_definitions:
    nt_table_create_definition
  | nt_table_create_definitions ',' nt_table_create_definition
;

nt_table_create_definition:
    nt_col_name nt_column_definition
;

nt_col_name:
    IDENTIFIER
      {
        $$ = $1;
      }
  | BT_IDENTIFIER
      {
        $$ = $1;
      }
;

nt_column_definition:
    nt_data_type
;

nt_data_type:
    INT nt_optional_length
      {
        unused long int length = 11;
        if ($2.v)
          {
            int errnum = errno;
            errno = 0;
            length = strtol($2.v, NULL, 10);
            queryparser_entry($2);
            parser_assert_err(QUERY_ERR_CONVERSION, errno != EINVAL && errno != ERANGE, $2.v);
            errno = errnum;
          }
      }
  | VARCHAR nt_length
      {
        unused long int length;
        int errnum = errno;
        errno = 0;
        length = strtol($2.v, NULL, 10);
        queryparser_entry($2);
        parser_assert_err(QUERY_ERR_CONVERSION, errno != EINVAL && errno != ERANGE, $2.v);
        errno = errnum;
      }
;

nt_optional_length:
    /* empty */
      {
        $$.v = NULL;
      }
  | nt_length
      {
        $$ = $1;
      }
;

nt_length:
    '(' NUMBER ')'
      {
        $$ = $2;
      }
;
nt_table_options:
    /* empty */
  | nt_table_options_list
;

nt_table_options_list:
    nt_table_option
  | nt_table_options ',' nt_table_option
;

nt_table_option:
    /* FIXME: none are supported yet */
;

nt_drop_statement:
    SCHEMA nt_if_exists nt_db_name ';'
      {
        queryparser_entry($3);
        int res = query_drop_schema($3.v, !$2.v);
        free($3.v);
        assert_inner(!res, "query_drop_schema");
      }
  | TABLE nt_if_exists nt_tbl_name ';'
      {
        queryparser_entry($3);
        int res = query_drop_table($3.v, !$2.v);
        free($3.v);
        assert_inner(!res, "query_drop_table");
      }
;

nt_if_exists:
    /* empty */
      {
        $$.v = (void*)0;
      }
  | IF_EXISTS
      {
        $$.v = (void*)1;
      }
;

nt_show_statement:
    SCHEMATA ';'
      {
        queryparser_entry($1);
        int res = query_show_schemata();
        assert_inner(!res, "query_show_schemata");
      }
;

nt_use_statement:
    nt_db_name ';'
      {
        queryparser_entry($1);
        int res = query_use($1.v);
        free($1.v);
        assert_inner(!res, "query_use");
      }
;

%%

#undef queryparser_entry

extern void yy_scan_string(const char *data);
extern void yylex_destroy(void);
extern unsigned int yylex_from_stdin;

int
queryparser_parse_from_file (const char *filename, const char *data)
{
  queryparser_file = filename;
  queryparser_line = 0;
  queryparser_char = 0;

  yylex_from_stdin = 0;

  yy_scan_string(data);
  int res = yyparse();
  assert_inner(!res, "%s: yyparse", queryparser_file);
  yylex_destroy();

  return 0;
}

extern void querylexer_restart(void);

int
queryparser_parse_from_stdin (void)
{
  queryparser_file = "<stdin>";
  queryparser_line = 0;
  queryparser_char = 0;

  yylex_from_stdin = 1;

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
