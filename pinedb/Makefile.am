
## Process this file with automake to produce Makefile.in

bin_PROGRAMS = pinedb

pinedb_SOURCES = pinedb.c pinedb.h globals.c globals.h

pinedb_LDADD = \
  ${top_builddir}/queryparser/libqueryparser.la \
  ${top_builddir}/datastore/libdatastore.la \
  ${top_builddir}/grapes/libgrapes.la

AM_CFLAGS += -I${top_srcdir}

if PROFILING

pinedb_LDADD += -lgraprof
AM_CFLAGS += -finstrument-functions -g

endif ## PROFILING
