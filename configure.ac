AC_PREREQ(2.59)

AC_INIT([asf-player],[1.0],[kskster@gmail.com])

AM_INIT_AUTOMAKE([1.9])

AC_PROG_CXX

OPENCV_REQUIRED=2.2.0

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

AC_OUTPUT([Makefile src/Makefile example/Makefile])
