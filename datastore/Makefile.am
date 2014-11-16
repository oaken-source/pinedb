
## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libdatastore.la

libdatastore_la_SOURCES = datastore.c datastore.h schema.c schema.h table.c table.h column.c column.h
AM_CFLAGS += -I${top_srcdir}

if PROFILING

libdatastore_la_LIBADD = -lgraprof
AM_CFLAGS += -finstrument-functions -g

endif ## PROFILING
