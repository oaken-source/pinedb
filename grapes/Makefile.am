
## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libgrapes.la

libgrapes_la_SOURCES = feedback.c feedback.h file.c file.h util.h
AM_CFLAGS += -I${top_srcdir}

if PROFILING

libgrapes_la_LIBADD = -lgraprof
AM_CFLAGS += -finstrument-functions -g

endif ## PROFILING
