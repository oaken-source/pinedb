
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


#pragma once

#include <config.h>

#include "table.h"

#include <grapes/util.h>

struct schema
{
  char *name;

  table **tables;
  unsigned int ntables;
};
typedef struct schema schema;


schema *schema_create(const char *name) may_fail;

void schema_destroy(schema *s);

table **schema_get_tables(schema *s, unsigned int *ntables);

table *schema_get_table_by_name(schema *s, const char *name);

int schema_add_table (schema *s, table *t) may_fail;

void schema_remove_table (schema *s, table *t);
