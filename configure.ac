##
## Configure file for ASLib.
## autoreconf will generate configure script.
##
## Create by JS <sanghe@163.com>
##
## Copyright (c) 2016 - 2017
## 
dnl --------------------------------------------------------------------------------------
dnl
dnl                 Basic informations about the configure
dnl
dnl --------------------------------------------------------------------------------------
dnl ------------------------------------------------
dnl AutoConf version and basic software information
dnl ------------------------------------------------
AC_PREREQ([2.69])
AC_INIT([ASLib], [0.0.1], [sanghe@163.com],[ASLib])
dnl ------------------------------------------------
dnl AutoMake initialize parameters
dnl ------------------------------------------------
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_SRCDIR([aslib/asl_type.h])
AC_CONFIG_HEADERS([config.h])
dnl ------------------------------------------------
dnl Get hostname and other information.
dnl ------------------------------------------------
AC_CANONICAL_BUILD()
AC_CANONICAL_HOST()
dnl ------------------------------------------------
dnl Change Compile Process to Silent Mode.
dnl ------------------------------------------------
m4_ifndef([AM_SILENT_RULES], [m4_define([AM_SILENT_RULES],[])])
AM_SILENT_RULES([yes])
dnl ------------------------------------------------
dnl Define the platform according to host.
dnl ------------------------------------------------
case "$host" in
  *-linux-*)
      USE_LINUX="yes"
      AC_DEFINE(PLAT_LINUX,,Use the Linux platform)
  ;;
  *)
      USE_LINUX="no"
      AC_DEFINE(PLAT_VXWORKS,,Use the VxWorks platform)
esac
AC_DEFINE_UNQUOTED(HOST_NAME,"$host",ASLib Run on this platform)
dnl ------------------------------------------------
dnl Check CFLAGS
dnl ------------------------------------------------
AC_ARG_WITH(cflags,
[  --with-cflags           Set CFLAGS for use in compilation.])
if test "x$with_cflags" != "x" ; then
  CFLAGS="$with_cflags" ; cflags_specified=yes ;
elif test -n "$CFLAGS" ; then
  cflags_specified=yes ;
fi
dnl ------------------------------------------------
dnl Check CC
dnl ------------------------------------------------
AC_PROG_CC
dnl ------------------------------------------------
dnl If CLFAGS doesn\'t exist set default value
dnl ------------------------------------------------
if test "x$cflags_specified" = "x" ; then
  CFLAGS="$CFLAGS -Wall"
fi
dnl ------------------------------------------------
dnl Check programs
dnl ------------------------------------------------
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_TOOL(AR, ar)
AC_CHECK_TOOL(RANLIB, ranlib, :)
dnl ------------------------------------------------
dnl Check libraries
dnl ------------------------------------------------
AM_PROG_AR
AC_PROG_RANLIB
dnl --------------------------------------------------------------------------------------
dnl
dnl                                 Customized Modules.
dnl
dnl --------------------------------------------------------------------------------------
ENABLE_MODULES="ASLib"
dnl ------------------------------------------------
dnl Basic library and platform functions
dnl ------------------------------------------------
LIB="aslib"
laslib="../aslib/libaslib.a"
AC_SUBST(LIB)
AC_SUBST(laslib)
dnl ------------------------------------------------
dnl Frameworks
dnl ------------------------------------------------
FRAMEWORK="asframework"
AC_SUBST(FRAMEWORK)
lasfw="../asframework/libasfw.a"
AC_SUBST(lasfw)
dnl ------------------------------------------------
dnl Data Structures
dnl ------------------------------------------------
DATASTRUCTURE="asdatastructure"
AC_SUBST(DATASTRUCTURE)
lasds="../asdatastructure/libasds.a"
AC_SUBST(lasds)
dnl ------------------------------------------------
dnl Algorithm
dnl ------------------------------------------------
ALGORITHM="asalgorithm"
AC_SUBST(ALGORITHM)
lasa="../asalgorithm/libasa.a"
AC_SUBST(lasa)
dnl --------------------------------------------------------------------------------------
dnl
dnl                 Packages configuration. Need to add more defines later
dnl
dnl --------------------------------------------------------------------------------------
dnl ------------------------------------------------
dnl if need to eliminate the Warning, enable the option
dnl ------------------------------------------------
AC_ARG_ENABLE(allwarning,
[  --disable-allwarning,   disable "treat warning as error" function during compile])
if test "${enable_allwarning}" = "no"; then
  CFLAGS="$CFLAGS"
else
  CFLAGS="$CFLAGS -Werror"
fi
dnl --------------------------------------------------------------------------
dnl                              Check all defines
dnl --------------------------------------------------------------------------
dnl ------------------------------------------------
dnl Define the default configure file path
dnl ------------------------------------------------
if test $prefix = NONE; then
 prefix=/usr/local
fi
CONFIGDIR=$prefix/etc
AC_DEFINE_UNQUOTED(PATH_SYSCONFDIR,"${CONFIGDIR}",Define the default configure file path)
dnl ------------------------------------------------
dnl Check libxml2 Support
dnl ------------------------------------------------
AC_ARG_ENABLE(xml2,
[  --enable-xml2=ARG  designate libxml2 base path])
if test "x${enable_xml2}" = "x" ; then
IXML="/usr/include/libxml2"
LXML="/lib/x86_64-linux-gnu"
else
IXML="${enable_xml2}/include/libxml2"
LXML="${enable_xml2}/lib"
fi
CFLAGS="$CFLAGS -I${IXML} -L${LXML}"
AC_MSG_CHECKING(whether have libxml2 support)
AC_TRY_COMPILE([#include <libxml/tree.h>
],[xmlDocPtr doc; doc=xmlNewDoc(BAD_CAST "1.0"); xmlFreeDoc(doc);],
AC_MSG_RESULT(yes),
[AC_MSG_RESULT(no)
 echo "ERROR: Current Environment not Support libxml2."
 exit 1])
AC_SUBST(IXML)
AC_SUBST(LXML)
dnl ------------------------------------------------
dnl Figure out build date
dnl ------------------------------------------------
BUILDDATE="`date +%x` `date | awk '{print $4}'`"
AC_DEFINE_UNQUOTED(BUILDDATE, "${BUILDDATE}",Build Number)

AC_CONFIG_FILES([Makefile
                 aslib/Makefile
                 asframework/Makefile
                 asdatastructure/Makefile
                 asalgorithm/Makefile])
AC_OUTPUT
echo "
ASLib configuration
-------------------
Version			: AC_PACKAGE_STRING
Host			: ${host}
Enable Modules          : ${ENABLE_MODULES}
compiler                : ${CC}
compiler flags          : ${CFLAGS}
Build Date		: ${BUILDDATE}
Target Dir		: $prefix
"
