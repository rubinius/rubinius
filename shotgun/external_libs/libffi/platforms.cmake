# What follows is a piece from libffi's configure.ac.
# Here on CMake we need to recreate this stuff without 
# precious config.guess script in order to be completely 
# platform independent (not all platforms have gotten Shell, yeah).
# So CMAKE_SYSTEM_NAME & CMAKE_SYSTEM_PROCESSOR are our friends.
# All platforms cannot be done in advance, given these variables
# may not correspond to $host. 

# If your plaftorm is not supported yet, you'll get an error message. 
# Then just add corresponding bit into this file.

# TARGETDIR="unknown"
# case "$host" in
#   alpha*-*-*)
#         TARGET=ALPHA; TARGETDIR=alpha;
#         # Support 128-bit long double, changable via command-line switch.
#         HAVE_LONG_DOUBLE='defined(__LONG_DOUBLE_128__)'
#         ;;
# 
#   arm*-*-*)
#         TARGET=ARM; TARGETDIR=arm
#         ;;
# 
#   amd64-*-freebsd*)
#         TARGET=X86_64; TARGETDIR=x86
#         ;;
# 
#   cris-*-*)
#         TARGET=LIBFFI_CRIS; TARGETDIR=cris
#         ;;
# 
#   frv-*-*)
#         TARGET=FRV; TARGETDIR=frv
#         ;;
# 
#   hppa*-*-linux* | parisc*-*-linux*)
#         TARGET=PA_LINUX; TARGETDIR=pa
#         ;;
#   hppa*64-*-hpux*)
#         TARGET=PA64_HPUX; TARGETDIR=pa
#         ;;
#   hppa*-*-hpux*)
#         TARGET=PA_HPUX; TARGETDIR=pa
#         ;;
# 
#   i386-*-freebsd* | i386-*-openbsd*)
#         TARGET=X86_FREEBSD; TARGETDIR=x86
#         ;;
#   i?86-win32* | i?86-*-cygwin* | i?86-*-mingw*)
#         TARGET=X86_WIN32; TARGETDIR=x86
#         ;;
#   i?86-*-darwin*)
#         TARGET=X86_DARWIN; TARGETDIR=x86
#         ;;
#   i?86-*-solaris2.1[[0-9]]*)
#         TARGET=X86_64; TARGETDIR=x86
#         ;;
#   i?86-*-*)
#         TARGET=X86; TARGETDIR=x86
#         ;;
# 
#   ia64*-*-*)
#         TARGET=IA64; TARGETDIR=ia64
#         ;;
# 
#   m32r*-*-*)
#         TARGET=M32R; TARGETDIR=m32r
#         ;;
# 
#   m68k-*-*)
#         TARGET=M68K; TARGETDIR=m68k
#         ;;
# 
#   mips-sgi-irix5.* | mips-sgi-irix6.*)
#         TARGET=MIPS; TARGETDIR=mips
#         ;;
#   mips*-*-linux*)
#         TARGET=MIPS; TARGETDIR=mips
#         ;;
# 
#   powerpc*-*-linux* | powerpc-*-sysv*)
#         TARGET=POWERPC; TARGETDIR=powerpc
#         ;;
#   powerpc-*-beos*)
#         TARGET=POWERPC; TARGETDIR=powerpc
#         ;;
#   powerpc-*-darwin*)
#         TARGET=POWERPC_DARWIN; TARGETDIR=powerpc
#         ;;
#   powerpc-*-aix* | rs6000-*-aix*)
#         TARGET=POWERPC_AIX; TARGETDIR=powerpc
#         ;;
#   powerpc-*-freebsd*)
#         TARGET=POWERPC_FREEBSD; TARGETDIR=powerpc
#         ;;
#   powerpc*-*-rtems*)
#         TARGET=POWERPC; TARGETDIR=powerpc
#         ;;
# 
#   s390-*-* | s390x-*-*)
#         TARGET=S390; TARGETDIR=s390
#         ;;
# 
#   sh-*-* | sh[[34]]*-*-*)
#         TARGET=SH; TARGETDIR=sh
#         ;;
#   sh64-*-* | sh5*-*-*)
#         TARGET=SH64; TARGETDIR=sh64
#         ;;
# 
#   sparc*-*-*)
#         TARGET=SPARC; TARGETDIR=sparc
#         ;;
# 
#   x86_64-*-darwin*)
#         TARGET=X86_DARWIN; TARGETDIR=x86
#         ;;
#   x86_64-*-cygwin* | x86_64-*-mingw*)
#         ;;
#   x86_64-*-*)
#         TARGET=X86_64; TARGETDIR=x86
#         ;;
# esac
set(p ${CMAKE_SYSTEM_PROCESSOR})
set(s ${CMAKE_SYSTEM_NAME})

if(p MATCHES "i.86")
  set(X86 TRUE)
elseif(p MATCHES "x86_64")
  set(X86_64 TRUE)
endif(p MATCHES "i.86")

###########################################################################

# TODO: 64-bit Darwin

# X86_64 fallback
if(NOT t AND X86_64)
  set(t X86_64)
  set(td x86)
endif(NOT t AND X86_64)

# TODO: X86_FREEBSD
# TODO: X86_DARWIN
if(NOT t AND WIN32)
  set(t X86_WIN32)
  set(td x86)
endif(NOT t AND WIN32)

# X86 fallback
if(NOT t AND X86)
  set(t X86)
  set(td x86)
endif(NOT t AND X86)

# TODO: powerpc

if(NOT t)
  message(SEND_ERROR "Unknown platform for FFI: proc ${p} sysname ${s}")
  message(SEND_ERROR "Please check shotgun/external_libs/libffi/platforms.cmake")
else(NOT t)
  set(TARGET ${t})
  set(TARGETDIR ${td})
endif(NOT t)
