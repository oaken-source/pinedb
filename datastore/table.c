
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


#include "table.h"

#include <stdlib.h>
#include <string.h>

table*
table_create (const char *name)
{
  __returns_ptr;

  table *t;

  __checked_call(NULL != (t = malloc(sizeof(*t))));
  __checked_call(NULL != (t->name = strdup(name)),
    free(t);
  );

  vector_init(&(t->columns));

  return t;
}

void
table_destroy (table *t)
{
  if (!t)
    return;

  vector_map(&(t->columns), ITEM,
    column_destroy(ITEM);
  );
  vector_clear(&(t->columns));

  free(t->name);
  free(t);
}

column*
table_get_column_by_name (table *t, const char *name)
{
  vector_map(&(t->columns), ITEM,
    if (!strcmp(ITEM->name, name))
      return ITEM;
  );

  return NULL;
}

int
table_add_column (table *t, column *c)
{
  __returns_int;

  __checked_call(0 == vector_push(&(t->columns), c));

  return 0;
}
