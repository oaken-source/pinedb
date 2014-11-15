
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

#include "parser.h"
#include "pinedb/globals.h"

#include <grapes/feedback.h>

#include <time.h>

/* interface to global parser state (currently processed file and line) */
extern const char *queryparser_file;
extern unsigned int queryparser_line;
extern unsigned int queryparser_char;

extern struct timespec queryparser_time;

/* This is a convenience assertion macro for parser specific warnings. It
 * checks the given condition, and prints a suitable warning message to
 * stderr, including the current parser state. It honors the global -Werror
 * and --quiet settings.
 *
 * params:
 *   FLAG - the warning associated with the condition
 *   COND - the condition to check
 *   ... - a variable length list of parameters for the format string
 */
#define parser_assert_wrn(FLAG, COND, ...) \
  do \
    { \
      if (!(COND) && (global_wflags & (FLAG))) \
        { \
          if (global_werror) \
            { \
              feedback_error(EXIT_SUCCESS, "%s:%u:%u: error: " FLAG ## _STR "[-Werror]", \
                  queryparser_file, queryparser_line, querparser_char, ## __VA_ARGS__); \
              return -1; \
            } \
          else if (!global_quiet) \
            feedback_warning("%s:%u:%u: warning: " FLAG ## _STR, queryparser_file, \
                queryparser_line, queryparser_char, ## __VA_ARGS__); \
        } \
    } \
  while (0)

/* This is a convenience assertion macro for parser specific errors. It checks
 * the given condition, and prints the corresponding error message to stderr
 * if need be, including the current parser state. It causes the current
 * function to return -1.
 *
 * params:
 *   FLAG - the error assiociated with the condition
 *   COND - the condition to check
 *   ... - a variable length list of parameters for the format string
 */
#define parser_assert_err_weak(FLAG, COND, ...) \
  do \
    { \
      if (!(COND)) \
        { \
          feedback_error(EXIT_SUCCESS, "%s:%i:%i: error: " FLAG ## _STR, \
              queryparser_file, queryparser_line, queryparser_char, ## __VA_ARGS__); \
        } \
    } \
  while(0)

#define parser_assert_err(FLAG, COND, ...) \
  do \
    { \
      if (!(COND)) \
        { \
          feedback_error(EXIT_SUCCESS, "%s:%i:%i: error: " FLAG ## _STR, \
              queryparser_file, queryparser_line, queryparser_char, ## __VA_ARGS__); \
          return -1; \
        } \
    } \
  while(0)
