#
#
AC_PREREQ(2.59)
AC_INIT([udis86], [1.7], [vivek@sig9.com])
AC_CONFIG_AUX_DIR(build)
AM_MAINTAINER_MODE

# Determine the build host; we notify automake of Windows builds, so it can
# pass proper parameters for building DLLs to the linker.
AC_CANONICAL_HOST
case "$host_os" in
	mingw32*)
		TARGET_OS=windows
		AC_LIBTOOL_WIN32_DLL
		;;
esac

# Initialize the automake subsystem.
# In case we have a Windows build, we pass a TARGET_WINDOWS conditional to
# automake.
AM_INIT_AUTOMAKE
AM_CONDITIONAL(TARGET_WINDOWS, test "$TARGET_OS" = windows)

AC_PROG_CC
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

# If this is a gnu compiler, pass -Wall
if test "$ac_cv_c_compiler_gnu" = "yes"; then
	CFLAGS="$CFLAGS -Wall"
fi

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
    Makefile
    libudis86/Makefile
    udcli/Makefile
    tests/Makefile
    docs/Makefile
])

AC_OUTPUT
