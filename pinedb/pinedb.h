
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

#include "globals.h"

#include "queryparser/parser.h"

#include <grapes/feedback.h>

#include <stdlib.h>
#include <string.h>
#include <argp.h>

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

const char doc[] = PACKAGE_NAME " - an in-memory database attempt";

const char args_doc[] = "file...";

static struct argp_option options[] =
{
  {"quiet", 'q', 0, 0, "suppress all warning output", 0},
  {0, 'W', "[no-]<warnings>", 0, "turn a specific category of warnings on or off", 0},
  {0, 0, 0, 0, 0, 0},
};

struct arguments
{
  const char **files;
  unsigned int nfiles;

  unsigned int werror;
  unsigned int wflags;
  unsigned int quiet;
};


#define check_single_warning_flag(FLAG, STR) \
  else if (!strcmp(arg, STR)) args->wflags |= (FLAG); \
  else if (!strcmp(arg, "no-" STR)) args->wflags &= ~(FLAG)

#define check_collection_warning_flag(FLAG, STR) \
  else if (!strcmp(arg, STR)) args->wflags |= (FLAG)


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *args = (struct arguments*)state->input;

  switch (key)
    {
    case 'q':
      args->quiet = 1;
      break;
    case 'W':
      // handle -W[no-]error
      if (!strcmp(arg, "error"))
        global_werror = 1;
      else if (!strcmp(arg, "no-error"))
        global_werror = 0;

      // handle collection flags
      check_collection_warning_flag(QUERY_WRN_ALL, "all");

      // handle single flags
      // check_single_warning_flag(QUERY_WRN_**, "**");

      // handle unrecognized flags
      else
        {
          feedback_error(EXIT_SUCCESS, "unrecognized warning flag: -W%s", arg);
          argp_usage(state);
        }

      break;
    case ARGP_KEY_ARG:
      ++(args->nfiles);
      args->files = realloc(args->files, sizeof(*(args->files)) * args->nfiles);
      assert_inner(args->files, "realloc");
      args->files[args->nfiles - 1] = arg;
      break;
    case ARGP_KEY_END:
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

#undef check_single_warning_flag
#undef check_collection_warning_flag

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

