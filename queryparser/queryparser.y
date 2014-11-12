
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

  #define YYSTYPE yystoken

  struct yystoken
  {
    char *s;
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

%token CREATE DROP SHOW USE SCHEMA SCHEMATA IF_NOT_EXISTS IF_EXISTS db_name SEMICOLON INVALID

%destructor { free($$.s); } db_name

%error-verbose

%%

/* a database script is a list of statements */
STATEMENTS:
    /* empty */
  | STATEMENTS STATEMENT
;

/* dispatch statements */
STATEMENT:
    CREATE CREATE_STATEMENT
  | DROP DROP_STATEMENT
  | SHOW SHOW_STATEMENT
  | USE USE_STATEMENT
;

/* create schema statement */
CREATE_STATEMENT:
    SCHEMA db_name SEMICOLON
      {
        queryparser_entry($2);
        int res = query_create_schema($2.s, 1);
        free($2.s);
        assert_inner(!res, "query_create_schema");
      }
  | SCHEMA IF_NOT_EXISTS db_name SEMICOLON
      {
        queryparser_entry($3);
        int res = query_create_schema($3.s, 0);
        free($3.s);
        assert_inner(!res, "query_create_schema");
      }
;

/* drop schema statement */
DROP_STATEMENT:
    SCHEMA db_name SEMICOLON
      {
        queryparser_entry($2);
        int res = query_drop_schema($2.s, 1);
        free($2.s);
        assert_inner(!res, "query_drop_schema");
      }
  | SCHEMA IF_EXISTS db_name SEMICOLON
      {
        queryparser_entry($3);
        int res = query_drop_schema($3.s, 0);
        free($3.s);
        assert_inner(!res, "query_drop_schema");
      }
;

/* show databases statement */
SHOW_STATEMENT:
    SCHEMATA SEMICOLON
      {
        queryparser_entry($1);
        int res = query_show_schemata();
        assert_inner(!res, "query_show_schemata");
      }
;

/* use statement */
USE_STATEMENT:
    db_name SEMICOLON
      {
        queryparser_entry($1);
        int res = query_use($1.s);
        free($1.s);
        assert_inner(!res, "query_use");
      }
;

%%

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
      printf("> ");
      fflush(stdout);
      querylexer_restart();
      res = yyparse();
      err |= (res != 0);
      assert_weak(!res, "%s: yyparse", queryparser_file);
    }
  while (res != 0);

  yylex_destroy();

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
