
dnl process this file with autoconf to produce a configure script

AC_PREREQ([2.69])
AC_INIT([pinedb], [0.1], [andreas@grapentin.org])
AC_CONFIG_SRCDIR([pinedb/pinedb.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([AM_CFLAGS], [])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AM_PROG_LEX
AC_PROG_YACC

AX_ALLOW_DEBUG_MESSAGES
AX_ALLOW_PROFILING

LT_INIT

AC_CONFIG_FILES([
  Makefile
  tests/Makefile])

AC_OUTPUT
