
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
#include "assertions.h"

#include "datastore/datastore.h"


static schema *current_schema = NULL;


static int may_fail
query_create_schema_impl (query_result *r, char *name, int strict)
{
  // make checks
  assert_inner(r, "query_result_create");
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_CREATE_EEXISTS, !(strict && s), name);

  // handle query
  if (s)
    return 0;

  s = schema_create(name);
  assert_inner(s, "schema_create");

  int res = datastore_add_schema(s);
  if (res)
    schema_destroy(s);
  assert_inner(!res, "datastore_add_schema");

  return 0;
}

query_result*
query_create_schema (query_arg *args)
{
  // extract arguments
  char *name = args[0].string;
  int strict = args[1].boolean;

  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_create_schema_impl(r, name, strict);

  // free resources
  free(name);

  // return
  if (res)
    {
      free(r);
      return NULL;
    }
  return r;
}

static int may_fail
query_drop_schema_impl (query_result *r, char *name, int strict)
{
  // make checks
  assert_inner(r, "query_result_create");
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_DROP_NOEXIST, !(strict && !s), name);

  // handle query
  if (!s)
    return 0;

  datastore_remove_schema(s);
  if (current_schema == s)
    current_schema = NULL;

  return 0;
}

query_result*
query_drop_schema (query_arg *args)
{
  // extract arguments
  char *name = args[0].string;
  int strict = args[1].boolean;

  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_drop_schema_impl(r, name, strict);

  // free resources
  free(name);

  // return
  if (res)
    {
      free(r);
      return NULL;
    }
  return r;
}

static int may_fail
query_show_schemata_impl (query_result *r)
{
  // make checks
  assert_inner(r, "query_result_create");

  // propagate result set
  unsigned int nschemata;
  schema **schemata = datastore_get_schemata(&nschemata);

  query_result_set_width(r, 1);
  query_result_push_checked(r, "database");

  unsigned int i;
  for (i = 0; i < nschemata; ++i)
    query_result_push_checked(r, schemata[i]->name);

  return 0;
}

query_result*
query_show_schemata (unused query_arg *args)
{
  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_show_schemata_impl(r);

  // return
  if (res)
    {
      free(r);
      return NULL;
    }
  return r;
}

static int may_fail
query_use_impl (query_result *r, char *name)
{
  // make checks
  assert_inner(r, "query_result_create");
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_USE_NOEXIST, s, name);

  // handle query
  current_schema = s;

  return 0;
}

query_result*
query_use (query_arg *args)
{
  // extract arguments
  char *name = args[0].string;

  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_use_impl(r, name);

  // free resources
  free(name);

  // return
  if (res)
    {
      free(r);
      return NULL;
    }
  return r;
}

query_result*
query_create_table (unused query_arg *args)
{
  // extract arguments
  //char *name = args[0].string;
  //int strict = args[1].boolean;
  //column_def *cols = args[2].pointer;
  //int ncols = args[3].integer;

  // create result instance
  query_result *r = query_result_create();
  assert_inner_ptr(r, "query_result_create");

  return r;
}

query_result*
query_drop_table (unused query_arg *args)
{
  // create result instance
  query_result *r = query_result_create();
  assert_inner_ptr(r, "query_result_create");

  return r;
}

query_result*
query_show_tables (unused query_arg *args)
{
  // create result instance
  query_result *r = query_result_create();
  assert_inner_ptr(r, "query_result_create");

  return r;
}
