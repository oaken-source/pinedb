
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

/* parse queries from mapped file
 *
 * params:
 *   filename - the name of the mapped file
 *   data - the contents of the mapped file
 *
 * errors:
 *   may fail for various reasons, including but not limited to
 *     - syntax errors
 *     - invalid configuration
 *     - memory scarcity
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int queryparser_parse_from_file(const char *filename, const char *data);

/* parse queries from stdin
 *
 * errors:
 *   may fail for various reasons, including but not limited to
 *     - syntax errors
 *     - invalid configuration
 *     - memory scarcity
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int queryparser_parse_from_stdin(void);

/* enumeration of all queryparser warning types and their string representation */
enum queryparser_warning_types
{
  QUERY_WRN_NONE
};

/* aggregated warning types */
#define QUERY_WRN_ALL       ( 0 )

/* warning types enabled by default */
#define QUERY_WRN_DEFAULT   ( 0 )

/* enumeration of all queryparser error types and their string representation */
enum error_types
{
  QUERY_ERR_PARSE,
  #define QUERY_ERR_PARSE_STR "%s"
  QUERY_ERR_SCHEMA_CREATE_EEXISTS,
  #define QUERY_ERR_SCHEMA_CREATE_EEXISTS_STR "unable to create schema `%s`: already exists"
  QUERY_ERR_SCHEMA_DROP_NOEXIST,
  #define QUERY_ERR_SCHEMA_DROP_NOEXIST_STR "unable to drop schema `%s`: does not exist"
  QUERY_ERR_SCHEMA_USE_NOEXIST,
  #define QUERY_ERR_SCHEMA_USE_NOEXIST_STR "unable to use schema `%s`: does not exist"
  QUERY_ERR_CONVERSION
  #define QUERY_ERR_CONVERSION_STR "`%s'"
};
