
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


#include "pinedb.h"

#include "queryparser/parser.h"

#include <grapes/feedback.h>
#include <grapes/file.h>

int
main (int argc, char *argv[])
{
  // process cli arguments
  struct arguments args = { { 0 }, 0, QUERY_WRN_DEFAULT, 0 };
  argp_parse(&argp, argc, argv, 0, 0, &args);

  global_werror = args.werror;
  global_wflags = args.wflags;
  global_quiet  = args.quiet;

  // parse queries from stdin if no files
  if (!args.files.nitems)
    {
      int res = queryparser_parse_from_stdin();
      feedback_assert(!res, "query processing failed.");
    }

  // parse queries from files otherwise
  unsigned int i;
  for (i = 0; i < args.files.nitems; ++i)
    {
      size_t length;
      char *data = file_map(args.files.items[i], &length);
      feedback_assert(data, "%s", args.files.items[i]);

      int res = queryparser_parse_from_file(args.files.items[i], data);
      feedback_assert(!res, "query processing failed.");

      res = file_unmap(data, length);
      feedback_assert(!res, "%s", args.files.items[i]);
    }

  // cleanup
  vector_clear(&(args.files));

  return 0;
}
