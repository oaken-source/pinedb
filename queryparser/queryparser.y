
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
%token SEMICOLON LPAREN RPAREN INVALID
%token identifier, number

%destructor { free($$.v); } identifier

%error-verbose

%%

NT_STATEMENTS:
    /* empty */
  | NT_STATEMENTS NT_STATEMENT
;

NT_STATEMENT:
    CREATE NT_CREATE_STATEMENT
  | DROP NT_DROP_STATEMENT
  | SHOW NT_SHOW_STATEMENT
  | USE NT_USE_STATEMENT
;

NT_CREATE_STATEMENT:
    SCHEMA NT_IF_NOT_EXISTS identifier SEMICOLON
      {
        queryparser_entry($3);
        int res = query_create_schema($3.v, !$2.v);
        free($3.v);
        assert_inner(!res, "query_create_schema");
      }
  | TABLE NT_IF_NOT_EXISTS identifier NT_CREATE_DEFINITIONS NT_TABLE_OPTIONS SEMICOLON
      {
        queryparser_entry($3);
        int res = query_create_table($3.v, !$2.v, $4.v, $5.v);
        free($3.v);
        assert_inner(!res, "query_create_table");
      }
;

NT_CREATE_DEFINITIONS:
    NT_CREATE_DEFINITION
  | NT_CREATE_DEFINITIONS NT_CREATE_DEFINITION
;

NT_CREATE_DEFINITION:
    identifier NT_COLUMN_DEFINITION
;

NT_COLUMN_DEFINITION:
    NT_DATATYPE
;

NT_DATATYPE:
    INT NT_LENGTH
  | VARCHAR NT_LENGTH
;

NT_LENGTH:
    LPAREN  RPAREN
;

NT_TABLE_OPTIONS:
    /* empty */
  | NT_TABLE_OPTIONS NT_TABLE_OPTION
;

NT_TABLE_OPTION:
    /* empty */
;

NT_IF_NOT_EXISTS:
    /* empty */     { $$.v = (void*)0; }
  | IF_NOT_EXISTS   { $$.v = (void*)1; }
;

NT_DROP_STATEMENT:
    SCHEMA NT_IF_EXISTS identifier SEMICOLON
      {
        queryparser_entry($3);
        int res = query_drop_schema($3.v, !$2.v);
        free($3.v);
        assert_inner(!res, "query_drop_schema");
      }
  | TABLE NT_IF_EXISTS identifier SEMICOLON
      {
        queryparser_entry($3);
        int res = query_drop_table($3.v, !$2.v);
        free($3.v);
        assert_inner(!res, "query_drop_table");
      }
;

NT_IF_EXISTS:
    /* empty */     { $$.v = (void*)0; }
  | IF_EXISTS       { $$.v = (void*)1; }
;

NT_SHOW_STATEMENT:
    SCHEMATA SEMICOLON
      {
        queryparser_entry($1);
        int res = query_show_schemata();
        assert_inner(!res, "query_show_schemata");
      }
;

NT_USE_STATEMENT:
    identifier SEMICOLON
      {
        queryparser_entry($1);
        int res = query_use($1.v);
        free($1.v);
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
