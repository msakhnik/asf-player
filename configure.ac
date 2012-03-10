AC_PREREQ(2.59)

AC_INIT([asf-player],[1.0],[kskster@gmail.com])

AM_INIT_AUTOMAKE([1.9])

AC_PROG_CXX

AC_PROG_CPP

OPENCV_REQUIRED=2.2.0

AC_ARG_ENABLE(unittests,
   [  --enable-unittests      build unittests],
   ENABLE_UNITTEST=$enableval)

if test "$ENABLE_UNITTEST" = "yes"; then
    AC_CHECK_HEADERS(
    [gtest/gtest.h],,
    [AC_MSG_ERROR([Missing gtest library, install the google unittest framework])])

    UNITTEST_LIBS="$(gtest-config --ldflags --libs)"
    AC_SUBST(UNITTEST_LIBS)
fi
AM_CONDITIONAL(ENABLE_UNITTEST, test "$enable_unittests" = "yes")

AC_SUBST([pkgconfigdir])
AC_MSG_NOTICE([pkgconfig directory is ${pkgconfigdir}])

PKG_CHECK_MODULES(OPENCV, [
  opencv >= $OPENCV_REQUIRED
  ], [
  AC_SUBST(OPENCV_CFLAGS)
  AC_SUBST(OPENCV_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the OpenCV development
      packages on your system.
      The minimum version required is $OPENCV_REQUIRED.
  ])
])

AC_OUTPUT([Makefile src/Makefile example/Makefile test/Makefile])
