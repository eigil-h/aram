AC_INIT([aram], [v0.1])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

PKG_CHECK_MODULES([gtkmm], [gtkmm-3.0 >= 3.8.1])
PKG_CHECK_MODULES([jack2], [jack >= 1.9.10])
PKG_CHECK_MODULES([odb], [libodb >= 2.3.0])
PKG_CHECK_MODULES([odbsqlite], [libodb-sqlite >= 2.3.0])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
