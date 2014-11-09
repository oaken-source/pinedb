
AC_DEFUN([AX_ALLOW_PROFILING], [
  AC_ARG_ENABLE(profiling,
  AS_HELP_STRING([--enable-profiling], [Build and install instrumented versions of graprof used for profiling. This is mainly useful for developers working on graprof itself. Default: no]),
  [case "${enableval}" in
    yes) profiling=true ;;
    no)  profiling=false ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-profiling]) ;;
  esac],
  [profiling=false])

  AM_CONDITIONAL(PROFILING, test x"$profiling" = x"true")
])
