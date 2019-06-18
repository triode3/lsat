#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lsat], [0.9.8.6], [number9@dimlight.org])
AC_CONFIG_SRCDIR([src])

# Checks for programs.
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_MKDIR_P

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T
AC_TYPE_UID_T

# Checks for library functions.
AC_CHECK_FUNCS([strstr strtol])

AC_CONFIG_FILES([Makefile src/Makefile])
AM_INIT_AUTOMAKE
AC_OUTPUT
