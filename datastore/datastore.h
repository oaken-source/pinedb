
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

#include "schema.h"

/* get the list of schemata associated with the datastore
 *
 * params:
 *   nschemata - a pointer where the number of returned elements is stored
 *
 * returns:
 *   a pointer to the first element of an array of pointers to schemata
 */
schema **datastore_get_schemata(unsigned int *nschemata);

/* get a schema instance identified by the given name
 *
 * params:
 *   name - the name of the queried schema
 *
 * returns:
 *   a pointer to a schema, if one is found, NULL otherwise
 */
schema *datastore_get_schema_by_name(const char *name);

/* add a schema instance to the datastore
 *
 * params:
 *   s - a pointer to a schema
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int datastore_add_schema(schema *s) may_fail;

/* remove a schema from the datastore, if present, and destroy the schema
 * instance
 *
 * params:
 *   s - a pointer to a schema
 *
 * errors:
 *   the behaviour is undefined, if the passed pointer is not a valid schema
 */
void datastore_remove_schema(schema *s);
