
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
#include "parser_tokens.h"

#include "datastore/datastore.h"


static schema *current_schema = NULL;


static int may_fail
query_create_schema_impl (query_result *r, char *name, int strict)
{
  // make checks
  assert_inner(r, "query_result_create");
  schema *s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_EEXISTS, !(strict && s), name);

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
      query_result_destroy(r);
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
  parser_assert_err(QUERY_ERR_SCHEMA_NOEXIST, !(strict && !s), name);

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
      query_result_destroy(r);
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
      query_result_destroy(r);
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
  parser_assert_err(QUERY_ERR_SCHEMA_NOEXIST, s, name);

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
      query_result_destroy(r);
      return NULL;
    }
  return r;
}

static int may_fail
query_create_table_impl (query_result *r, char *schema_name, char *name, int strict, tok_column_vector *columns)
{
  // make checks
  assert_inner(r, "query_result_create");
  parser_assert_err(QUERY_ERR_NO_SCHEMA_SELECTED, schema_name || current_schema);
  schema *s = current_schema;
  if (schema_name)
    s = datastore_get_schema_by_name(schema_name);
  parser_assert_err(QUERY_ERR_SCHEMA_NOEXIST, s, schema_name);

  table *t = schema_get_table_by_name(s, name);
  parser_assert_err(QUERY_ERR_TABLE_EEXISTS, !(strict && t), name);
  if (t)
    return 0;

  // handle query
  t = table_create(name);
  assert_inner(t, "table_create");

  size_t i;
  for (i = 0; i < columns->nitems; ++i)
    {
      column *c = table_get_column_by_name(t, columns->items[i].name);
      if (c)
        {
          table_destroy(t);
          parser_assert_err(QUERY_ERR_COLUMN_EEXISTS, 0, columns->items[i].name);
        }

      c = column_create(columns->items[i].name, columns->items[i].type.type, columns->items[i].type.width);
      if (!c)
        {
          table_destroy(t);
          assert_inner(0, "column_create");
        }

      int res = table_add_column(t, c);
      if (res)
        {
          table_destroy(t);
          column_destroy(c);
          assert_inner(0, "table_add_column");
        }
    }

  int res = schema_add_table(s, t);
  if (res)
    {
      table_destroy(t);
      assert_inner(0, "schema_add_table");
    }

  return 0;
}

query_result*
query_create_table (query_arg *args)
{
  // extract arguments
  char *name = args[0].string;
  char *schema = args[1].string;
  int strict = args[2].boolean;
  tok_column_vector *columns = args[3].pointer;

  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_create_table_impl(r, schema, name, strict, columns);

  // free resources
  free(name);
  free(schema);
  vector_clear(columns);

  // return
  if (res)
    {
      query_result_destroy(r);
      return NULL;
   }
  return r;
}

static int may_fail
query_drop_table_impl (query_result *r, char *schema_name, char *name, int strict)
{
  // make checks
  assert_inner(r, "query_result_create");
  parser_assert_err(QUERY_ERR_NO_SCHEMA_SELECTED, schema_name || current_schema);
  schema *s = current_schema;
  if (schema_name)
    s = datastore_get_schema_by_name(schema_name);
  parser_assert_err(QUERY_ERR_SCHEMA_NOEXIST, !(strict && !s), schema_name);
  if (!s)
    return 0;

  table *t = schema_get_table_by_name(s, name);
  parser_assert_err(QUERY_ERR_TABLE_NOEXIST, !(strict && !t), name);
  if (!t)
    return 0;

  // handle query
  schema_remove_table(s, t);

  return 0;
}

query_result*
query_drop_table (query_arg *args)
{
  // extract arguments
  char *name = args[0].string;
  char *schema = args[1].string;
  int strict = args[2].boolean;

  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_drop_table_impl(r, schema, name, strict);

  // free resources
  free(name);
  free(schema);

  // return
  if (res)
    {
      query_result_destroy(r);
      return NULL;
    }
  return r;
}

// FIXME: remove debug include
#include <stdio.h>

static int may_fail
query_show_tables_impl (query_result *r, char *name)
{
  // make checks
  assert_inner(r, "query_result_create");
  parser_assert_err(QUERY_ERR_NO_SCHEMA_SELECTED, current_schema || name);
  schema *s = current_schema;
  if (name)
    s = datastore_get_schema_by_name(name);
  parser_assert_err(QUERY_ERR_SCHEMA_NOEXIST, s, name);

  // propagate result set
  query_result_set_width(r, 1);
  query_result_push_checked(r, "table");

  unsigned int ntables;
  table **tables = schema_get_tables(s, &ntables);

  unsigned int i;
  for (i = 0; i < ntables; ++i)
    query_result_push_checked(r, tables[i]->name);

  return 0;
}

query_result*
query_show_tables (unused query_arg *args)
{
  // extract arguments
  char *schema = args[0].string;

  // create result instance
  query_result *r = query_result_create();

  // call impl
  int res = query_show_tables_impl(r, schema);

  // free resources
  free(schema);

  // return
  if (res)
    {
      query_result_destroy(r);
      return NULL;
    }
  return r;
}


