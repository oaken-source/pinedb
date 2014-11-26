
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

#include "column.h"

#include <grapes/util.h>

struct table
{
  char *name;

  column **columns;
  unsigned int ncolumns;
};
typedef struct table table;


/* allocate and initialize a table instance
 *
 * params:
 *   name - the name of the table
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and strdup
 *
 * returns:
 *   a pointer to a table on success, NULL on failure
 */
table *table_create (const char *name) __may_fail;

/* destroy a table created by table_create
 *
 * params:
 *   t - a pointer to a table
 *
 * erros:
 *   the behaviour is undefined if the given pointer is invalid
 */
void table_destroy (table *t);

/* get the column identified by the give name
 *
 * params:
 *   t - a pointer to a table
 *   name - the name of the queried column
 *
 * returns:
 *   a pointer to a column, if found, NULL otherwise
 */
column *table_get_column_by_name (table *t, const char *name);

/* add a column to a table
 *
 * params:
 *   t - a pointer to a table
 *   c - a pointer to a column
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int table_add_column (table *t, column *c) __may_fail;
