AC_INIT([rofi], [1.6.1-dev], [https://github.com/davatorium/rofi/],[],[https://reddit.com/r/qtools/])

AC_CONFIG_SRCDIR([source/rofi.c])
AC_CONFIG_HEADER([config.h])

dnl ---------------------------------------------------------------------
dnl Lex & Bison language parser.
dnl ---------------------------------------------------------------------
AC_PROG_LEX
AC_PROG_YACC


AC_DEFUN([AX_PROG_BISON], [
  AC_REQUIRE([AC_PROG_YACC])
  AC_REQUIRE([AC_PROG_EGREP])

  AC_CACHE_CHECK([if bison is the parser generator],[ax_cv_prog_bison],[
    AS_IF([$YACC --version 2>/dev/null | $EGREP -q '^bison '],
      [ax_cv_prog_bison=yes], [ax_cv_prog_bison=no])
  ])
  AS_IF([test "$ax_cv_prog_bison" = "yes"], [
      dnl replace the yacc-compatible compiler with the real bison, as
      dnl otherwise autoconf limits us to the POSIX yacc.
      dnl We also change the generated filename to the old one, so that
      dnl automake's ylwrap can deal with it.
      YACC="${YACC% -y} -o y.tab.c"
    ] m4_ifnblank([$1], [[$1]]),
    m4_ifnblank([$2], [[$2]])
  )
])

AX_PROG_BISON([],[AC_MSG_ERROR("Failed to find bison")])

m4_include([m4/ax_prog_flex_version.m4])
m4_include([m4/ax_compare_version.m4])
AC_SUBST([FLEX], [${LEX}])
AX_PROG_FLEX_VERSION([2.5.39],,AC_MSG_ERROR("Require flex version 2.5.39 or higher"))
#

dnl ---------------------------------------------------------------------
dnl Setup automake to be silent and in foreign mode.
dnl We want xz distribution
dnl ---------------------------------------------------------------------
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects dist-xz])
AM_SILENT_RULES([yes])

dnl ---------------------------------------------------------------------
dnl  Check for compiler
dnl ---------------------------------------------------------------------
AC_PROG_CC([clang gcc cc])

dnl ---------------------------------------------------------------------
dnl  C99 standard
dnl ---------------------------------------------------------------------
AC_PROG_CC_C99

dnl ---------------------------------------------------------------------
dnl C to Object rules.
dnl ---------------------------------------------------------------------
AM_PROG_CC_C_O

dnl ---------------------------------------------------------------------
dnl System extensions
dnl ---------------------------------------------------------------------
AC_USE_SYSTEM_EXTENSIONS

dnl ---------------------------------------------------------------------
dnl Static libraries programs
dnl ---------------------------------------------------------------------
AC_PROG_RANLIB
AM_PROG_AR

dnl ---------------------------------------------------------------------
dnl Base CFLAGS
dnl ---------------------------------------------------------------------
AM_CFLAGS="-Wall -Wextra -Wparentheses -Winline -pedantic  -Wunreachable-code"

dnl ---------------------------------------------------------------------
dnl Enable source code coverage reporting for GCC
dnl ---------------------------------------------------------------------

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING ([--enable-gcov],[Enable source code coverage testing using gcov])],
              [AM_CFLAGS="${AM_CFLAGS} -coverage"])
AS_IF([test "x${enable_gcov}" = "xyes" ], [AC_DEFINE([ENABLE_GCOV], [1], [Enable gcov profiling])])

AC_ARG_ENABLE([asan], [AS_HELP_STRING ([--enable-asan],[Enable asan support])])
AS_IF([test "x${enable_asan}" = "xyes" ], [
       AC_DEFINE([ENABLE_ASAN], [1], [Enable libasan])
       AM_CFLAGS="${AM_CFLAGS} -fsanitize=address -fno-omit-frame-pointer -g3"]

       ])


dnl --------------------------------------------------------------------
dnl DRun dialog
dnl --------------------------------------------------------------------
AC_ARG_ENABLE([drun], AS_HELP_STRING([--disable-drun],[Disable desktop file run dialog]))
AS_IF([test "x${enable_drun}" != "xno"], [AC_DEFINE([ENABLE_DRUN], [1], [Enable desktop file run dialog])])

dnl ---------------------------------------------------------------------
dnl Disable window mode
dnl ---------------------------------------------------------------------
AC_ARG_ENABLE([windowmode], AS_HELP_STRING([--disable-windowmode],[Disable window mode]))
AS_IF([ test "x$enable_windowmode" != "xno"], [AC_DEFINE([WINDOW_MODE],[1],[Enable the window mode])])


dnl ---------------------------------------------------------------------
dnl Check for C functions.
dnl ---------------------------------------------------------------------
AC_CHECK_FUNC([getline],,  AC_MSG_ERROR("Could not find getline in c library"))
AC_CHECK_FUNC([open],,     AC_MSG_ERROR("Could not find open in c library"))
AC_CHECK_FUNC([sysconf],,  AC_MSG_ERROR("Could not find sysconf"))
AC_CHECK_FUNC([strtok_r],, AC_MSG_ERROR("Could not find strtok_r"))
AC_CHECK_FUNC([flock],,    AC_MSG_ERROR("Could not find flock"))
AC_CHECK_FUNC([ftruncate],,AC_MSG_ERROR("Could not find ftruncate"))
AC_CHECK_FUNC([fcntl],,    AC_MSG_ERROR("Could not find fcntl"))
AC_CHECK_FUNC([setlocale],,AC_MSG_ERROR("Could not find setlocale"))
AC_CHECK_FUNC([atexit],,   AC_MSG_ERROR("Could not find atexit in c library"))
AC_CHECK_FUNC([glob],,     AC_MSG_ERROR("Could not find glob in c library"))
AC_CHECK_FUNC([readdir],,  AC_MSG_ERROR("Could not find readdir in c library"))

AC_CHECK_HEADER([math.h],, AC_MSG_ERROR("Could not find math.h header file"))
AC_SEARCH_LIBS([floor],[m],,      AC_MSG_ERROR("Could not find floor in math library"))
AC_SEARCH_LIBS([ceil], [m],,      AC_MSG_ERROR("Could not find ceil in math library"))

AC_CHECK_HEADER([sysexits.h],, AC_MSG_ERROR("Could not find the sysexists.h header file"))
AC_CHECK_HEADER([setjmp.h],,   AC_MSG_ERROR("Could not find the setjmp.h header file"))

dnl ---------------------------------------------------------------------
dnl Check dependencies
dnl ---------------------------------------------------------------------
PKG_PROG_PKG_CONFIG


dnl ---------------------------------------------------------------------
dnl PKG_CONFIG based dependencies
dnl ---------------------------------------------------------------------
glib_min_major="2"
glib_min_minor="40"
glib_min_version="${glib_min_major}.${glib_min_minor}"
NK_INIT([bindings xdg-theme])
PKG_CHECK_MODULES([glib],     [glib-2.0 >= ${glib_min_version} gio-unix-2.0 gmodule-2.0])
AC_DEFINE_UNQUOTED([GLIB_VERSION_MIN_REQUIRED], [(G_ENCODE_VERSION(${glib_min_major},${glib_min_minor}))], [The lower GLib version supported])
AC_DEFINE_UNQUOTED([GLIB_VERSION_MAX_ALLOWED], [(G_ENCODE_VERSION(${glib_min_major},${glib_min_minor}))], [The highest GLib version supported])
GW_CHECK_XCB([xcb-aux xcb-xkb xkbcommon xkbcommon-x11 xcb-ewmh xcb-icccm xcb-cursor xcb-randr xcb-xinerama])
PKG_CHECK_MODULES([pango],    [pango pangocairo])
PKG_CHECK_MODULES([cairo],	  [cairo cairo-xcb])
PKG_CHECK_MODULES([libsn],    [libstartup-notification-1.0 ])
PKG_CHECK_MODULES([gdkpixbuf],    [gdk-pixbuf-2.0])

dnl ---------------------------------------------------------------------
dnl check - Unit testing.
dnl ---------------------------------------------------------------------
AC_ARG_ENABLE([check], AS_HELP_STRING([--disable-check], [Build with checks using check library (default: enabled)]))

AS_IF([test "x${enable_check}" != "xno"], [ PKG_CHECK_MODULES([check],[check >= 0.11.0], [HAVE_CHECK=1]) ])
AM_CONDITIONAL([USE_CHECK],  [test "x${enable_check}" != "xno" && test "$HAVE_CHECK" -eq 1])


dnl ---------------------------------------------------------------------
dnl Gets the resource compile tool path.
dnl ---------------------------------------------------------------------
AM_PATH_GLIB_2_0

dnl ---------------------------------------------------------------------
dnl Add extra compiler flags
dnl ---------------------------------------------------------------------
AC_SUBST([AM_CFLAGS])

AC_CONFIG_FILES([Makefile doc/rofi.doxy pkgconfig/rofi.pc])
AC_OUTPUT

echo ""
echo "-------------------------------------"
if test x$enable_drun != xno; then
echo "Desktop File drun dialog     Enabled"
else
echo "Desktop File drun dialog     Disabled"
fi
if test x$enable_windowmode != xno; then
echo "Window Switcher dialog       Enabled"
else
echo "Window Switcher dialog       Disabled"
fi
if test x$enable_asan = xyes; then
echo "Asan address sanitize        Enabled"
else
echo "Asan address sanitize        Disabled"
fi
if test x$enable_gcov = xyes; then
echo "Code Coverage                Enabled"
else
echo "Code Coverage                Disabled"
fi
if test "x${enable_check}" != "xno" && test "$HAVE_CHECK" -eq 1; then
echo "Check based tests            Enabled"
else
echo "Check based tests            Disabled"
fi
echo "-------------------------------------"
echo "Now type 'make' to build"
echo ""
