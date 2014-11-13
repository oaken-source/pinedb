
## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libqueryparser.la

BUILT_SOURCES = queryparser.h
AM_YFLAGS = -d -Wall -Werror

libqueryparser_la_SOURCES = queryparser.y querylexer.l parser.h assertions.h query.c query.h query_result.c query_result.h
AM_CFLAGS += -I${top_srcdir}

libqueryparser_la_LIBADD = -lreadline

if PROFILING

libqueryparser_la_LIBADD += -lgraprof
AM_CFLAGS += -finstrument-functions -g

endif ## PROFILING
