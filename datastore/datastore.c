
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

#include <string.h>

struct datastore
{
  schema **schemata;
  unsigned int nschemata;
};

struct datastore datastore = { NULL, 0 };

schema**
datastore_get_schemata (unsigned int *nschemata)
{
  *nschemata = datastore.nschemata;
  return datastore.schemata;
}

schema*
datastore_get_schema_by_name (const char *name)
{
  unsigned int i;
  for (i = 0; i < datastore.nschemata; ++i)
    if (!strcmp(datastore.schemata[i]->name, name))
      return datastore.schemata[i];

  return NULL;
}

int
datastore_add_schema (schema *s)
{
  ++(datastore.nschemata);
  datastore.schemata = realloc(datastore.schemata, sizeof(*(datastore.schemata)) * datastore.nschemata);
  assert_inner(datastore.schemata, "realloc");
  datastore.schemata[datastore.nschemata - 1] = s;

  return 0;
}

void
datastore_remove_schema (schema *s)
{
  unsigned int i;
  for (i = 0; i < datastore.nschemata; ++i)
    if (datastore.schemata[i] == s)
      break;
  for (++i; i < datastore.nschemata; ++i)
    datastore.schemata[i - 1] = datastore.schemata[i];

  --(datastore.nschemata);
  schema_destroy(s);
}

static void
__attribute__((destructor))
datastore_fini (void)
{
  unsigned int i;
  for (i = 0; i < datastore.nschemata; ++i)
    schema_destroy(datastore.schemata[i]);

  free(datastore.schemata);
}
