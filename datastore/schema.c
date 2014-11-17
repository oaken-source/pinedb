
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


#include "schema.h"

#include <string.h>

schema*
schema_create (const char *name)
{
  schema *s = malloc(sizeof(*s));
  assert_inner_ptr(s, "malloc");

  s->name = strdup(name);
  if (!s->name)
    {
      free(s);
      assert_inner_ptr(0, "strdup");
    }

  s->tables = NULL;
  s->ntables = 0;

  return s;
}

void
schema_destroy (schema *s)
{
  unsigned int i;
  for (i = 0; i < s->ntables; ++i)
    table_destroy(s->tables[i]);
  free(s->tables);

  free(s->name);
  free(s);
}

table*
schema_get_table_by_name (schema *s, const char *name)
{
  unsigned int i;
  for (i = 0; i < s->ntables; ++i)
    if (!strcmp(s->tables[i]->name, name))
      return s->tables[i];

  return NULL;
}

int
schema_add_table (schema *s, table *t)
{
  ++(s->ntables);
  void *new = realloc(s->tables, sizeof(*(s->tables)) * s->ntables);
  assert_inner(new, "realloc");

  s->tables = new;
  s->tables[s->ntables - 1] = t;

  return 0;
}

void
schema_remove_table (schema *s, table *t)
{
  unsigned int i;
  for (i = 0; i < s->ntables; ++i)
    if (s->tables[i] == t)
      break;
  for (++i; i < s->ntables; ++i)
    s->tables[i - 1] = s->tables[i];

  --(s->ntables);
  table_destroy(t);
}
