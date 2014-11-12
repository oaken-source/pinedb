
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


#include "query_result.h"

#include <stdio.h>
#include <string.h>

void
query_result_init (query_result *r)
{
  r->width = 1;
  r->items = NULL;
  r->nitems = 0;
}

void
query_result_fini (query_result *r)
{
  free(r->items);
}

void
query_result_set_width (query_result *r, unsigned int width)
{
  r->width = width;
}

int
query_result_push (query_result *r, const char *item)
{
  ++(r->nitems);
  r->items = realloc(r->items, sizeof(*(r->items)) * r->nitems);
  assert_inner(r->items, "realloc");
  r->items[r->nitems - 1] = item;

  return 0;
}

int
query_result_print (const query_result *r)
{
  size_t *w = calloc(r->width, sizeof(*w));
  assert_inner(w, "calloc");

  size_t i;
  for (i = 0; i < r->nitems; ++i)
    {
      size_t l = strlen(r->items[i]);
      if (l > w[i % r->width])
        w[i % r->width] = l;
    }

  size_t total = 1;
  for (i = 0; i < r->width; ++i)
    total += w[i] + 3;

  printf(" +");
  for (i = 0; i < total - 2; ++i)
    putchar('-');
  printf("+\n |");

  for (i = 0; i < r->width; ++i)
    {
      int s = w[i % r->width];
      printf(" %*.*s |", s, s, r->items[i]);
    }

  printf("\n |");
  for (i = 0; i < total - 2; ++i)
    putchar('-');
  printf("|");

  for (i = r->width; i < r->nitems; ++i)
    {
      if (!((i - 1) % r->width))
        printf("\n |");
      int s = w[i % r->width];
      printf(" %*.*s |", s, s, r->items[i]);
    }

  printf("\n +");
  for (i = 0; i < total - 2; ++i)
    putchar('-');
  printf("+\n");

  free(w);

  return 0;
}
