
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

#include <grapes/util.h>

enum datatype
{
  DATATYPE_INT,
  DATATYPE_VARCHAR
};
typedef enum datatype datatype;

struct column
{
  char *name;

  datatype type;
  int width;
};
typedef struct column column;


/* allocate and initialize a column
 *
 * params:
 *   name - the name of the column
 *   type - the datatype
 *   width - the width of the field
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and strdup
 *
 * returns:
 *   a pointer to a column on success, NULL on failure
 */
column *column_create (const char *name, datatype type, int width) __may_fail;

/* destroy a column via the pointer returned by column_create
 *
 * params:
 *   c - a pointer to a column
 *
 * errors:
 *   the behaviour is undefined if an invalid pointer is passed
 */
void column_destroy (column *c);
