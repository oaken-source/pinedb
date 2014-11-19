
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


#pragma once

#include <config.h>

#include "query.h"

#include "datastore/datastore.h"

#include <grapes/vector.h>

struct yystoken
{
  char *v;
  unsigned int l;
  unsigned int c;
};
typedef struct yystoken yystoken;

struct tok_datatype
{
  datatype type;
  int width;
};

struct tok_column
{
  char *name;
  struct tok_datatype type;
};

struct tok_statement
{
  query_result*(*func)(query_arg*);
  const char *func_name;
  query_arg args[5];
};

#define statement_init(S, TYPE) \
    do { \
      (S).func = &(TYPE); \
      (S).func_name = #TYPE; \
    } while (0)

vector_declare(tok_column_vector, struct tok_column);

struct tok_tbl_name
{
  char *table;
  char *schema;
};
