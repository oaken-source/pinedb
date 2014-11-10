
## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libqueryparser.la

BUILT_SOURCES = queryparser.h
AM_YFLAGS = -d -Wall

libqueryparser_la_SOURCES = queryparser.y querylexer.l parser.h assertions.h query.c query.h
AM_CFLAGS += -I${top_srcdir}

if PROFILING

libqueryparser_la_LIBADD = -lgraprof
AM_CFLAGS += -finstrument-functions -g

endif ## PROFILING
