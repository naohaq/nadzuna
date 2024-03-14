
AC_INIT([nadzuna], [0.1])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])

AC_CONFIG_SRCDIR([src/tgafile.c])

AC_LANG([C])
AC_PROG_CC

AC_CHECK_HEADER([png.h])
AC_CHECK_LIB([png], [png_image_begin_read_from_file], , , [-lz])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
])

AC_OUTPUT
