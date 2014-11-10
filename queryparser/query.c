
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

#include <stdio.h>

int
query_create_schema (const char *name, int strict)
{
  schema *s = datastore_get_schema_by_name(name);
  if (strict)
    parser_assert_err(QUERY_ERR_SCHEMA_CREATE_EEXISTS, !s, name);
  else if (s)
    {
      printf("ok (skipped): [time]");
      return 0;
    }

  s = malloc(sizeof(*s));
  assert_inner(s, "malloc");
  int res = schema_init(s, name);
  assert_inner(!res, "schema_init");

  res = datastore_add_schema(s);
  assert_inner(!res, "datastore_add_schema");

  printf("ok: [time]\n");

  return 0;
}

int
query_drop_schema (const char *name, int strict)
{
  schema *s = datastore_get_schema_by_name(name);
  if (strict)
    parser_assert_err(QUERY_ERR_SCHEMA_DROP_NOEXIST, s, name);
  else if (!s)
    {
      printf("ok (skipped): [time]");
      return 0;
    }

  datastore_remove_schema(s);

  printf("ok: [time]\n");

  return 0;
}

int
query_show_schemata (void)
{
  unsigned int nschemata;
  schema **schemata = datastore_get_schemata(&nschemata);

  printf(" databases\n");
  printf(" ---------\n");

  unsigned int i;
  for (i = 0; i < nschemata; ++i)
    printf(" %s\n", schemata[i]->name);

  return 0;
}
