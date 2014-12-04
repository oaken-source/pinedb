
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

#include "datastore.h"

#include <grapes/vector.h>

#include <string.h>

vector_declare(vec_schemata, schema*);

vec_schemata schemata = { 0 };

schema**
datastore_get_schemata (unsigned int *nschemata)
{
  *nschemata = schemata.nitems;
  return schemata.items;
}

schema*
datastore_get_schema_by_name (const char *name)
{
  vector_map(&schemata, ITEM,
    if (!strcmp(ITEM->name, name))
      return ITEM;
  );

  return NULL;
}

int
datastore_add_schema (schema *s)
{
  __returns_int;

  __checked_call(0 == vector_push(&schemata, s));

  return 0;
}

void
datastore_remove_schema (schema *s)
{
  unsigned int i;
  for (i = 0; i < schemata.nitems; ++i)
    if (schemata.items[i] == s)
      break;
  for (++i; i < schemata.nitems; ++i)
    schemata.items[i - 1] = schemata.items[i];

  --(schemata.nitems);
  schema_destroy(s);
}

static void
__attribute__((destructor))
datastore_fini (void)
{
  vector_map(&schemata, ITEM,
    schema_destroy(ITEM);
  );
  vector_clear(&schemata);
}
