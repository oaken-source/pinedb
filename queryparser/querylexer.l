
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
    char *s;
    unsigned int l;
    unsigned int c;
  };
  typedef struct yystoken yystoken;

  #define YYSTYPE yystoken
  #include "queryparser.h"

  #include <grapes/util.h>

  unsigned int yycharno = 0;
  unsigned int yylex_from_stdin = 0;

}

%%

#.*                 /* ignore comments */
--.*                /* ignore comments */

create              { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return CREATE; }
drop                { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return DROP; }
show                { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return SHOW; }
use                 { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return USE; }
database|schema     { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return SCHEMA; }
databases|schemata  { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return SCHEMATA; }
if\ not\ exists     { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return IF_NOT_EXISTS; }
if\ exists          { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return IF_EXISTS; }

`.*`                { yytext[yyleng - 1] = 0;
                      yylval.s = strdup(yytext + 1); yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return db_name; }
[a-zA-Z0-9$_]*      { yylval.s = strdup(yytext); yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return db_name; }

;                   { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return SEMICOLON; }

\n                  { yycharno = 0;
                      if (yylex_from_stdin) { printf("> "); fflush(stdout); } yycharno = 0; }
[ \r\t]*            { yycharno += yyleng; }

.                   { yylval.l = yylineno; yylval.c = yycharno;
                      yycharno += yyleng;
                      return INVALID; }

%%

void
querylexer_restart (void)
{
  YY_FLUSH_BUFFER;
}
