dnl Hey Emacs, I want this in -*- autoconf -*- mode, please.

dnl Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
dnl Please see COPYING for a description your rights and responsibilities
dnl with this software.
dnl Process this file with autoconf to produce a configure script.

dnl ----------------------------- HOST SYSTEM -----------------------------------

AC_PREREQ(2.54)
AC_INIT([GNU lightning], 1.2b, bonzini@gnu.org, lightning)
AC_CONFIG_AUX_DIR(build-aux)
AC_CONFIG_MACRO_DIR(build-aux)
AC_CONFIG_SRCDIR([lightning.h])

AC_CANONICAL_TARGET
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PATH_PROG(INSTALL_INFO, install-info, :, $PATH:/sbin)
AC_EXEEXT

BACKENDS="LIGHTNING_BACKENDS"
AC_SUBST(BACKENDS)

LIGHTNING_CONFIGURE_LINKS(
  [AC_DEFINE_UNQUOTED(LIGHTNING_TARGET, [AS_TR_CPP([LIGHTNING_$cpu$suffix])],
		      [Used to pick the appropriate disassembler, for debugging])],
  [AC_MSG_ERROR([cpu $target_cpu not supported])])

AC_SUBST(cpu)
AM_CONDITIONAL(LIGHTNING_MAIN, :)

dnl ---------------------------- COMMAND LINE ---------------------------------

AC_ARG_ENABLE( disassembling,
[  --enable-disassembling  make the test programs disassemble the code
			   enabled by default if host != target],
, enable_disassembling=no)

AM_CONDITIONAL(REGRESSION_TESTING, test "$host_cpu" = "$target_cpu")
if test "$host_cpu" != "$target_cpu"; then
  AC_DEFINE(LIGHTNING_CROSS, 1,
    [Define if test programs should not run the compiled code])
  enable_disassembling=yes
fi

if test "$enable_disassembling" != no; then
  AC_DEFINE(LIGHTNING_DISASSEMBLE, 1,
    [Define if the test programs should disassemble the code they produce])
  LIBDISASS="libdisass.a"
else
  LIBDISASS=
fi
AM_CONDITIONAL(DISASS, test "$enable_disassembling" != no)
test "$enable_disassembling" != no && 
AC_SUBST(LIBDISASS)

AC_ARG_ENABLE( assertions,
[  --enable-assertions     perform internal consistency checks],
, enable_assertions=no)

if test "$enable_assertions" != no; then
  AC_DEFINE(_ASM_SAFETY, 1, [Define to enable assertions])
fi

dnl --------------------------- PRODUCE OUTPUT --------------------------------

AC_CONFIG_FILES(Makefile doc/Makefile tests/Makefile opcode/Makefile
	lightning/Makefile)
AC_CONFIG_FILES(lightningize, chmod +x lightningize)

AC_OUTPUT

# A small sanity check
echo "#include <stdio.h>" > confdefs.h		# dummy input file
CPPFLAGS="$CPPFLAGS -I. -I$srcdir -I$srcdir/lightning/$cpu"
AC_TRY_COMPILE([#include "lightning.h"], , ,
  AC_MSG_WARN(the compiler that was found could not compile GNU lightning))
