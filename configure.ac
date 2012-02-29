AC_INIT([heis], [1.0], [mortenmj@stud.ntnu.no])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Check for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

AC_CONFIG_FILES([
    Makefile
    libheis/Makefile
    src/Makefile
])

AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--disable-debug],[compile without debug code]),
    enable_debug=$enableval, enable_debug=yes )

AC_ARG_ENABLE(backtrace,
                AC_HELP_STRING([--enable-backtrace],[enable printing out the backtrace in case of crash]),
                    enable_backtrace=$enableval, enable_backtrace=no )

if test x$enable_debug = xyes; then
  AC_DEFINE(ENABLE_DEBUG, [], [Enable debug code])
fi

if test x$enable_backtrace = xyes; then
  AC_DEFINE(ENABLE_BACKTRACE, [], [Enable backtrace output on crashes])
fi

COMEDI_MIN_VERSION=0.8.1

PKG_CHECK_MODULES(LIBHEIS, 
                  comedilib >= $COMEDI_MIN_VERSION)

AC_OUTPUT
