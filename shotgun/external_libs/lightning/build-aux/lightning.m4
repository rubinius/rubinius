dnl I'd like this to be edited in -*- Autoconf -*- mode...
dnl
# serial 2 LIGHTNING_CONFIGURE_IF_NOT_FOUND
m4_define([LIGHTNING_BACKENDS], [i386:-32 i386:-64 sparc ppc])

AC_DEFUN([LIGHTNING_CONFIGURE_LINKS], [

suffix=
case "$host_cpu" in
  i?86)	   cpu=i386; suffix=-32 ;;
  x86_64)  cpu=i386; suffix=-64 ;;
  sparc*)  cpu=sparc	        ;;
  powerpc) cpu=ppc	        ;;
  *)			        ;;
esac
if test -n "$cpu" && test -d "$srcdir/lightning/$cpu"; then
  $1
  lightning_frag=`cd $srcdir && pwd`/lightning/$cpu/Makefile.frag
  test -f $lightning_frag || lightning_frag=/dev/null

  asm_src=lightning/$cpu/asm.h
  test -f $srcdir/lightning/$cpu/asm$suffix.h && asm_src=lightning/$cpu/asm$suffix.h
  AC_CONFIG_LINKS(lightning/asm.h:$asm_src, [], [asm_src=$asm_src])

  fp_src=lightning/$cpu/fp.h
  test -f $srcdir/lightning/$cpu/fp$suffix.h && fp_src=lightning/$cpu/fp$suffix.h
  AC_CONFIG_LINKS(lightning/fp.h:$fp_src, [], [fp_src=$fp_src])

  core_src=lightning/$cpu/core.h
  test -f $srcdir/lightning/$cpu/core$suffix.h && core_src=lightning/$cpu/core$suffix.h
  AC_CONFIG_LINKS(lightning/core.h:$core_src, [], [core_src=$core_src])

  funcs_src=lightning/$cpu/funcs.h
  test -f $srcdir/lightning/$cpu/funcs$suffix.h && funcs_src=lightning/$cpu/funcs$suffix.h
  AC_CONFIG_LINKS(lightning/funcs.h:$funcs_src, [], [funcs_src=$funcs_src])
else
  $2
  lightning_frag=/dev/null
fi
AC_SUBST_FILE(lightning_frag)

])

AC_DEFUN([LIGHTNING_CONFIGURE_IF_NOT_FOUND], [
AC_REQUIRE([AC_PROG_LN_S])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_CHECK_HEADER(lightning.h)
AM_CONDITIONAL(LIGHTNING_MAIN, (exit 1))
AM_CONDITIONAL(HAVE_INSTALLED_LIGHTNING, test "$ac_cv_header_lightning_h" = yes)

lightning=
if test "$ac_cv_header_lightning_h" = yes; then
  lightning=yes
else
  LIGHTNING_CONFIGURE_LINKS(lightning=yes, lightning=no)
fi

AS_IF([test "$lightning" = yes], [
  AC_DEFINE(HAVE_LIGHTNING, 1, [Define if GNU lightning can be used])
  $1
], [$2])
unset lightning

])dnl
