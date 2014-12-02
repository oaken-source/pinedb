
## process this file with automake to produce Makefile.in

EXTRA_DIST =  asparagus.exp \
              lib/feedback.tcl lib/globals.tcl lib/string.tcl \
              steps/dispatcher.tcl steps/default.tcl steps/custom.tcl \
	      test.sql \
	      pinedb.tests/cli.exp \
	      pinedb.tests/schema.exp \
	      pinedb.tests/table.exp \
	      leakcheck.tests/leakcheck.exp

AUTOMAKE_OPTIONS = dejagnu

DEJAGNU  = $(top_srcdir)/tests/asparagus.exp
DEJATOOL = pinedb

export DEJAGNU

if PROFILING

DEJATOOL += leakcheck

endif ## PROFILING

