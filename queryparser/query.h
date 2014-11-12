
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

#include <grapes/util.h>

/* execute a create schema query
 *
 * params:
 *   name - the name of the schema
 *   strict - flag indicating wether the query fail on eexist
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc, schema_init and
 *      datastore_add_schema
 *   will fail if strict evaluates to true and a schema of the same name
 *      already exists
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int query_create_schema(const char *name, int strict) may_fail;

/* execute a drop schema query
 *
 * params:
 *   name - the name of the schema
 *   strict - flag indicating wether the query fails on noexist
 *
 * errors:
 *   will fail if strict evaluates to true and a schema of the given name
 *      does not exist
 * returns:
 *   -1 on failure, 0 on success
 */
int query_drop_schema(const char *name, int strict) may_fail;

/* execute a show schemata statement
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and
 *      query_result_push
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int query_show_schemata(void) may_fail;

/* execute a use statement
 *
 * params:
 *   name - the name of the schema
 *
 * errors:
 *   may fail if a schema of the given name does not exist
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int query_use (const char *name) may_fail;

int query_create_table (const char *name, int strict, void *columns, void *options) may_fail;

int query_drop_table (const char *name, int strict) may_fail;
