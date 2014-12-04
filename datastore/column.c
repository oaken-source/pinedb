
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


#include "column.h"

#include <string.h>
#include <stdlib.h>

column*
column_create (const char *name, datatype type, int width)
{
  __returns_ptr;

  column *c;

  __checked_call(NULL != (c = malloc(sizeof(*c))));
  __checked_call(NULL != (c->name = strdup(name)),
    free(c);
  );

  c->type = type;
  c->width = width;

  return c;
}

void
column_destroy (column *c)
{
  if (!c)
    return;

  free(c->name);
  free(c);
}
