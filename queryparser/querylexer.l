
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

%option noyywrap nounput noinput yylineno case-insensitive

%top{

  struct yystoken
  {
    void *v;
    unsigned int l;
    unsigned int c;
  };
  typedef struct yystoken yystoken;

  #define YYSTYPE yystoken

  #include <stdlib.h>
  #include <stdio.h>
  #include <readline/readline.h>
  #include <readline/history.h>

  static int querylexer_input(char *buf, size_t size);
  #define YY_INPUT(buf,result,max_size) result = querylexer_input(buf, max_size);

  #include "queryparser.h"

  #include <grapes/util.h>

  unsigned int yycharno = 0;

  #define common_setup do { yylval.l = yylineno; yylval.c = yycharno; yycharno += yyleng; } while (0)

}

%%

#.*                 /* ignore comments */
--.*                /* ignore comments */

create              { common_setup; return CREATE; }
drop                { common_setup; return DROP; }
show                { common_setup; return SHOW; }
use                 { common_setup; return USE; }
database|schema     { common_setup; return SCHEMA; }
databases|schemata  { common_setup; return SCHEMATA; }
table               { common_setup; return TABLE; }
tables              { common_setup; return TABLES; }
int                 { common_setup; return INT; }
varchar             { common_setup; return VARCHAR; }
if\ not\ exists     { common_setup; return IF_NOT_EXISTS; }
if\ exists          { common_setup; return IF_EXISTS; }

`[^`]*`             { common_setup;
                      yytext[yyleng - 1] = 0;
                      yylval.v = strdup(yytext + 1);
                      return BT_IDENTIFIER; }
[1-9][0-9]*         { common_setup;
                      yylval.v = strdup(yytext);
                      return NUMBER; }
[a-zA-Z0-9$_]+      { common_setup;
                      yylval.v = strdup(yytext);
                      return IDENTIFIER; }

\n                  { yycharno = 1; }
[ \r\t]*            { yycharno += yyleng; }

.                   { common_setup; return yytext[0]; }

%%

#undef common_setup

void
querylexer_restart (void)
{
  yylineno = 1;
  yycharno = 1;
  YY_FLUSH_BUFFER;
}

static int
querylexer_input (char *buf, size_t size)
{
  char *line;
  if (feof(yyin))
    return YY_NULL;
  line = readline("> ");
  if (!line)
    return YY_NULL;
  if (strlen(line) > size - 2)
  {
    perror("input line too long");
    return YY_NULL;
  }
  strcpy(buf, line);
  free(line);
  add_history(buf);
  strcat(buf, "\n");

  return strlen(buf);
}

static void
__attribute__((destructor))
querylexer_fini (void)
{
  clear_history();
}
