
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

#include "assertions.h"

#include <stdio.h>
#include <string.h>

query_result*
query_result_create (void)
{
  __returns_ptr;

  query_result *r;

  __checked_call(NULL != (r = malloc(sizeof(*r))));

  r->width = 1;

  vector_init(&(r->items));

  return r;
}

void
query_result_destroy (query_result *r)
{
  if (!r)
    return;

  vector_clear(&(r->items));
  free(r);
}

void
query_result_set_width (query_result *r, unsigned int width)
{
  r->width = width;
}

int
query_result_push (query_result *r, const char *item)
{
  __returns_int;

  __checked_call(0 == vector_push(&(r->items), item));

  return 0;
}

#define draw_line(W, C) \
    do { \
      putchar(' '); putchar(C); \
      size_t i; \
      for (i = 0; i < (W) - 2; ++i) \
        putchar('-'); \
      putchar(C); \
    } while (0)

int
query_result_print (const query_result *r)
{
  __returns_int;

  if (r->items.nitems)
    {
      size_t *w;
      __checked_call(NULL != (w = calloc(r->width, sizeof(*w))));

      vector_map(&(r->items), ITEM,
        size_t l = strlen(ITEM);
        if (l > w[i % r->width])
          w[i % r->width] = l;
      );

      size_t total = 1;
      size_t i;
      for (i = 0; i < r->width; ++i)
        total += w[i] + 3;

      draw_line(total, '+');
      printf("\n |");

      for (i = 0; i < r->width; ++i)
        {
          int s = w[i % r->width];
          printf(" %*.*s |", s, s, r->items.items[i]);
        }

      printf("\n");
      draw_line(total, '|');

      for (i = r->width; i < r->items.nitems; ++i)
        {
          if (!(i % r->width))
            printf("\n |");
          int s = w[i % r->width];
          printf(" %*.*s |", s, s, r->items.items[i]);
        }

      printf("\n");
      draw_line(total, '+');
      printf("\n");

      free(w);
    }

  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed = (end.tv_sec + 1.0e-9 * end.tv_nsec) -
      (queryparser_time.tv_sec + 1.0e-9 * queryparser_time.tv_nsec);

  printf("ok: ");
  if (r->items.nitems)
    printf("returned %zu rows ", (r->items.nitems - 1) / r->width);
  printf("[%.8lfs]\n", elapsed);

  return 0;
}
