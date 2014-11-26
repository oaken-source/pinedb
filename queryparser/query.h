
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

#include "query_result.h"
#include "query_args.h"

#include <grapes/util.h>

/* execute a create schema query
 *
 * params:
 *   args[0] <string>  - the name of the schema
 *   args[1] <boolean> - flag indicating wether the query fail on eexist
 *
 * errors:
 *   may fail and set errno for the same reasons as query_result_create,
 *      schema_create and datastore_add_schema
 *   will fail if strict evaluates to true and a schema of the same name
 *      already exists
 *
 * returns:
 *   a pointer to a query_result on success, NULL on failure
 */
query_result* query_create_schema(query_arg *args) __may_fail;

/* execute a drop schema query
 *
 * params:
 *   args[0] <string>  - the name of the schema
 *   args[1] <boolean> - flag indicating wether the query fails on noexist
 *
 * errors:
 *   may fail and set errno for the same reasons as query_result_create
 *   will fail if strict evaluates to true and a schema of the given name
 *      does not exist
 *
 * returns:
 *   a pointer to a query_result on success, NULL on failure
 */
query_result* query_drop_schema(query_arg *args) __may_fail;

/* execute a show schemata statement
 *
 * params:
 *   none
 *
 * errors:
 *   may fail and set errno for the same reasons as query_result_create and
 *      query_result_push
 *
 * returns:
 *   a pointer to a query_result on success, NULL on failure
 */
query_result* query_show_schemata(query_arg *args) __may_fail;

/* execute a use statement
 *
 * params:
 *   args[0] <string>  - the name of the schema
 *
 * errors:
 *   may fail and set errno for the same reasons as query_result_create
 *   will fail if a schema of the given name does not exist
 *
 * returns:
 *   a pointer to a query_result on success, NULL on failure
 */
query_result* query_use (query_arg *args) __may_fail;

/* execute a create table statement
 *
 * params:
 *   args[0] <string>  - the name of the table
 *   args[1] <boolean> - flag indicating wether the query fails on eexist
 *   args[2] <pointer> - an array of column definitions
 *   args[3] <integer> - the length of the array
 *
 * errors:
 *   TODO
 *
 * returns:
 *   a pointer to a query_result on success, NULL on failuer
 */
query_result* query_create_table (query_arg *args) __may_fail;

/* FIXME: in the making */
query_result* query_drop_table (query_arg *args) __may_fail;

/* FIXME: in the making */
query_result* query_show_tables (query_arg *args) __may_fail;
