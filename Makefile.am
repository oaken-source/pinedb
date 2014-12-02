
## process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

AM_YFLAGS = -d -Wall -Werror
BUILT_SOURCES =

noinst_LTLIBRARIES = \
  grapes/libgrapes.la \
  queryparser/libqueryparser.la \
  datastore/libdatastore.la

grapes_libgrapes_la_SOURCES = \
  grapes/feedback.c grapes/feedback.h \
  grapes/file.c grapes/file.h \
  grapes/vector.c grapes/vector.h \
  grapes/util.h

queryparser_libqueryparser_la_SOURCES = \
  queryparser/queryparser.y \
  queryparser/querylexer.l \
  queryparser/parser.h \
  queryparser/parser_tokens.h \
  queryparser/assertions.h \
  queryparser/query.c queryparser/query.h \
  queryparser/query_result.c queryparser/query_result.h \
  queryparser/query_args.c queryparser/query_args.h

BUILT_SOURCES += \
  queryparser/queryparser.h queryparser/queryparser.c \
  queryparser/querylexer.c

queryparser_libqueryparser_la_LIBADD = -lreadline

datastore_libdatastore_la_SOURCES = \
  datastore/datastore.c datastore/datastore.h \
  datastore/schema.c datastore/schema.h \
  datastore/table.c datastore/table.h \
  datastore/column.c datastore/column.h

bin_PROGRAMS = \
  pinedb/pinedb

pinedb_pinedb_SOURCES = \
  pinedb/pinedb.c pinedb/pinedb.h \
  pinedb/globals.c pinedb/globals.h

pinedb_pinedb_LDADD = \
  queryparser/libqueryparser.la \
  datastore/libdatastore.la \
  grapes/libgrapes.la

if PROFILING

grapes_libgrapes_la_LIBADD = -lgraprof
grapes_libgrapes_la_CFLAGS = -finstrument-functions -g

queryparser_libqueryparser_la_LIBADD += -lgraprof
queryparser_libqueryparser_la_CFLAGS  = -finstrument-functions -g

datastore_libdatastore_la_LIBADD = -lgraprof
datastore_libdatastore_la_CFLAGS = -finstrument-functions -g

pinedb_pinedb_LDADD += -lgraprof
pinedb_pinedb_CFLAGS = -finstrument-functions -g

endif ## PROFILING

SUBDIRS = tests

