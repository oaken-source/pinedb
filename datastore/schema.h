
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

#include "table.h"

#include <grapes/util.h>

struct schema
{
  char *name;

  table **tables;
  unsigned int ntables;
};
typedef struct schema schema;


/* allocate and initialize a schema instance
 *
 * params:
 *   name - the name of the schema
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and strdup
 *
 * returns:
 *   a pointer to a schema on success, NULL otherwise
 */
schema *schema_create(const char *name) __may_fail;

/* destroy a schema that was created using schema_create
 *
 * params:
 *   s - a pointer to a schema
 *
 * errors:
 *   the behaviour is undefined if the pointer is invalid
 */
void schema_destroy(schema *s);

/* return the list of tables associated with the given schema
 *
 * params:
 *   s - a pointer to a schema
 *   ntables - the location where the number of returned tables is stored
 *
 * returns:
 *   a pointer to the first element of an array of pointers to tables
 */
table **schema_get_tables(schema *s, unsigned int *ntables);

/* return a table identified by the given name, if any
 *
 * params:
 *   s - a pointer to a schema
 *   name - the name of the searched table
 *
 * returns:
 *   a pointer to a table, if found, NULL otherwise
 */
table *schema_get_table_by_name(schema *s, const char *name);

/* add a table to the given schema
 *
 * params:
 *   s - a pointer to a schema
 *   t - a pointer to a table
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int schema_add_table (schema *s, table *t) __may_fail;

/* remove a table from the given schema, if found, and destroy the table
 * instance
 *
 * params:
 *   s - a pointer to a schema
 *   t - a pointer to a table
 *
 * errors:
 *   the behaviour is undefined if the given pointer is invalid
 */
void schema_remove_table (schema *s, table *t);
