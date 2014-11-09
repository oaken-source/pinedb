
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

  #include "assertions.h"
  #include "datastore/datastore.h"

  #include <stdio.h>

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

%}

%token CREATE DROP SHOW SCHEMA SCHEMATA IF_NOT_EXISTS IF_EXISTS db_name SEMICOLON INVALID

%error-verbose

%%

/* a database script is a list of statements */
SCRIPT:
    /* empty */
  | SCRIPT STATEMENT
;

/* dispatch statements */
STATEMENT:
    CREATE_SCHEMA_STATEMENT
  | DROP_SCHEMA_STATEMENT
  | SHOW_DATABASES_STATEMENT
;

/* create schema statement */
CREATE_SCHEMA_STATEMENT:
    CREATE SCHEMA db_name SEMICOLON
      {
        queryparser_line = $3.l;
        queryparser_char = $3.c;

        schema *s = datastore_get_schema_by_name($3.s);
        parser_assert_err(QUERY_ERR_SCHEMA_CREATE_EEXISTS, !s, $3.s);

        s = malloc(sizeof(*s));
        assert_inner(s, "malloc");
        int res = schema_init(s, $3.s);
        assert_inner(!res, "schema_init");

        res = datastore_add_schema(s);
        assert_inner(!res, "datastore_add_schema");

        printf("successfully created schema `%s`\n", $3.s);

        free($3.s);
      }
  | CREATE SCHEMA IF_NOT_EXISTS db_name SEMICOLON
      {
        queryparser_line = $4.l;
        queryparser_char = $4.c;

        schema *s = datastore_get_schema_by_name($4.s);
        if (s)
          {
            printf("skipping creation of schema `%s`: already exists\n", $4.s);
          }
        else
          {
            s = malloc(sizeof(*s));
            assert_inner(s, "malloc");
            int res = schema_init(s, $4.s);
            assert_inner(!res, "schema_init");

            res = datastore_add_schema(s);
            assert_inner(!res, "datastore_add_schema");

            printf("successfully created schema `%s`\n", $4.s);
          }

        free($4.s);
      }
;

/* drop schema statement */
DROP_SCHEMA_STATEMENT:
    DROP SCHEMA db_name SEMICOLON
      {
        queryparser_line = $3.l;
        queryparser_char = $3.c;

        schema *s = datastore_get_schema_by_name($3.s);
        parser_assert_err(QUERY_ERR_SCHEMA_DROP_NOEXIST, s, $3.s);

        datastore_remove_schema(s);

        printf("successfully dropped schema `%s`\n", $3.s);

        free($3.s);
      }
  | DROP SCHEMA IF_EXISTS db_name SEMICOLON
      {
        queryparser_line = $4.l;
        queryparser_char = $4.c;

        schema *s = datastore_get_schema_by_name($4.s);
        if (!s)
          {
            printf("skipping dropping of schema `%s`: does not exist\n", $4.s);
          }
        else
          {
            datastore_remove_schema(s);

            printf("successfully dropped schema `%s`\n", $4.s);
          }

        free($4.s);
      }
;

/* show databases statement */
SHOW_DATABASES_STATEMENT:
    SHOW SCHEMATA SEMICOLON
      {
        unsigned int nschemata;
        schema **schemata = datastore_get_schemata(&nschemata);

        printf(" databases\n");
        printf(" ---------\n");

        unsigned int i;
        for (i = 0; i < nschemata; ++i)
          printf(" %s\n", schemata[i]->name);
      }
;

%%

extern void yy_scan_string(const char *data);
extern void yylex_destroy(void);

int
queryparser_parse_from_file (const char *filename, const char *data)
{
  queryparser_file = filename;
  queryparser_line = 0;
  queryparser_char = 0;

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
  queryparser_line = 0;
  queryparser_char = 0;

  printf("> ");
  fflush(stdout);

  int res = yyparse();
  assert_inner(!res, "%s: yyparse", queryparser_file);
  yylex_destroy();

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
