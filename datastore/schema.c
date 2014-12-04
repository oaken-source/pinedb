
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
  __returns_ptr;

  schema *s;

  __checked_call(NULL != (s = malloc(sizeof(*s))));
  __checked_call(NULL != (s->name = strdup(name)),
    free(s);
  );

  vector_init(&(s->tables));

  return s;
}

void
schema_destroy (schema *s)
{
  if (!s)
    return;

  vector_map(&(s->tables), ITEM,
    table_destroy(ITEM);
  );
  vector_clear(&(s->tables));

  free(s->name);
  free(s);
}

table**
schema_get_tables (schema *s, unsigned int *ntables)
{
  *ntables = s->tables.nitems;
  return s->tables.items;
}

table*
schema_get_table_by_name (schema *s, const char *name)
{
  vector_map(&(s->tables), ITEM,
    if (!strcmp(ITEM->name, name))
      return ITEM;
  );

  return NULL;
}

int
schema_add_table (schema *s, table *t)
{
  __returns_int;

  __checked_call(0 == vector_push(&(s->tables), t));

  return 0;
}

void
schema_remove_table (schema *s, table *t)
{
  unsigned int i;
  for (i = 0; i < s->tables.nitems; ++i)
    if (s->tables.items[i] == t)
      break;
  for (++i; i < s->tables.nitems; ++i)
    s->tables.items[i - 1] = s->tables.items[i];

  --(s->tables.nitems);
  table_destroy(t);
}
