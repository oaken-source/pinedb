
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


#include "query.h"
#include "query_result.h"
#include "assertions.h"

#include "datastore/datastore.h"

#include <stdio.h>
#include <time.h>


schema *current_schema = NULL;

#define query_return_result(RES) \
  do { \
    if (RES) \
      { \
        int res = query_result_print(RES); \
        assert_inner(!res, "query_result_print"); \
      } \
    struct timespec end; \
    clock_gettime(CLOCK_MONOTONIC, &end); \
    double elapsed = (end.tv_sec + 1.0e-9 * end.tv_nsec) - \
        (queryparser_time.tv_sec + 1.0e-9 * queryparser_time.tv_nsec); \
    if (RES) \
      { \
        unsigned int rows = (((query_result*)RES)->nitems - 1) / ((query_result*)RES)->width; \
        printf("ok: returned %u rows [%.8lfs]\n", rows, elapsed); \
      } \
    else \
      printf("ok: [%.8lfs]\n", elapsed); \
    if (RES) \
      { \
        query_result_fini(RES); \
        free(RES); \
      } \
    return 0; \
  } while (0)

int
query_create_schema (const char *name, int strict)
{
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_CREATE_EEXISTS, !(strict && s), name);
  if (s)
    query_return_result(NULL);

  s = malloc(sizeof(*s));
  assert_inner(s, "malloc");
  int res = schema_init(s, name);
  assert_inner(!res, "schema_init");

  res = datastore_add_schema(s);
  assert_inner(!res, "datastore_add_schema");

  query_return_result(NULL);
}

int
query_drop_schema (const char *name, int strict)
{
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_DROP_NOEXIST, !(strict && !s), name);
  if (!s)
    query_return_result(NULL);

  datastore_remove_schema(s);

  if (current_schema == s)
    current_schema = NULL;

  query_return_result(NULL);
}

int
query_show_schemata (void)
{
  unsigned int nschemata;
  schema **schemata = datastore_get_schemata(&nschemata);

  query_result *r = malloc(sizeof(*r));
  assert_inner(r, "malloc");
  query_result_init(r);

  query_result_set_width(r, 1);
  query_result_push_checked(r, "database");

  unsigned int i;
  for (i = 0; i < nschemata; ++i)
    query_result_push_checked(r, schemata[i]->name);

  query_return_result(r);
}

int
query_use (const char *name)
{
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_USE_NOEXIST, s, name);

  current_schema = s;

  query_return_result(NULL);
}
