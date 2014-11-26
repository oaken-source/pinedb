
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

struct query_result
{
  unsigned int width;
  const char **items;
  unsigned int nitems;
};
typedef struct query_result query_result;

/* convenience wrapper macro for query_result_push that includes the common
 * error handling routines
 */
#define query_result_push_checked(R, I) \
    do { \
      int res = query_result_push((R), (I)); \
      assert_inner(!res, "query_result_push"); \
    } while (0)


/* create and instanciate a query_result instance
 * the returned instance should be freed by invoking query_result_destroy
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc
 *
 * returns:
 *   a pointer to a query_result on success, NULL otherwise
 */
query_result *query_result_create(void) __may_fail;

/* destroy a query_result and free all associated memory
 *
 * params:
 *   r - a pointer to a query_result
 *
 * errors:
 *   the behaviour is undefined if the passed pointer is invalid
 */
void query_result_destroy(query_result *r);

/* set the number of columns in a query_result
 *
 * params:
 *   r - a pointer to a query_result
 *   width - the number of columns
 */
void query_result_set_width(query_result *r, unsigned int width);

/* push an item to a query_result
 *
 * params:
 *   r - a pointer to a query result
 *   item - an item string
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int query_result_push(query_result *r, const char *item) __may_fail;

/* print a completed query_result nicely formatted to stdout
 *
 * params:
 *   r - a pointer to a query_result
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc
 *   the behaviour is undefined if the query_result cotains garbage
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int query_result_print(const query_result *r) __may_fail;

