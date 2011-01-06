#
# --------------------------------------------------------------------------
#
#      Pthreads-win32 - POSIX Threads Library for Win32
#      Copyright(C) 1998 John E. Bossom
#      Copyright(C) 1999,2005 Pthreads-win32 contributors
# 
#      Contact Email: rpj@callisto.canberra.edu.au
# 
#      The current list of contributors is contained
#      in the file CONTRIBUTORS included with the source
#      code distribution. The list can also be seen at the
#      following World Wide Web location:
#      http://sources.redhat.com/pthreads-win32/contributors.html
# 
#      This library is free software; you can redistribute it and/or
#      modify it under the terms of the GNU Lesser General Public
#      License as published by the Free Software Foundation; either
#      version 2 of the License, or (at your option) any later version.
# 
#      This library is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#      Lesser General Public License for more details.
# 
#      You should have received a copy of the GNU Lesser General Public
#      License along with this library in the file COPYING.LIB;
#      if not, write to the Free Software Foundation, Inc.,
#      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
#

DLL_VER	= 2
DLL_VERD= $(DLL_VER)d

DEVROOT	= C:\PTHREADS

DLLDEST	= $(DEVROOT)\DLL
LIBDEST	= $(DEVROOT)\DLL

# If Running MsysDTK
RM	= rm -f
MV	= mv -f
CP	= cp -f

# If not.
#RM	= erase
#MV	= rename
#CP	= copy

# For cross compiling use e.g.
# make CROSS=i386-mingw32msvc- clean GC-inlined


target  = $(CROSS)
prefix = /usr/local

ifeq (,$(findstring i686, $(WTHR_TARGET_ARCH)))
ifeq (,$(findstring x86_64, $(target)))
#32bit
BITNESS=-m32
DLLFLAG=-m i386
WINDRESFLAG=pe-i386
else
#64bit
BITNESS=-m64
DLLFLAG=-m i386:x86-64
WINDRESFLAG=pe-x86-64
endif
else
ifeq (,$(findstring x86_64, $(WTHR_TARGET_ARCH)))
#32bit
BITNESS=-m32
DLLFLAG=-m i386
WINDRESFLAG=pe-i386
else
BITNESS=-m64
DLLFLAG=-m i386:x86-64
WINDRESFLAG=pe-x86-64
endif
endif

AR	= $(target)-ar
DLLTOOL = $(target)-dlltool $(DLLFLAG)
CC      = $(target)-gcc $(BITNESS)
CXX     = $(target)-g++ $(BITNESS)
RANLIB  = $(target)-ranlib
RC	= $(target)-windres -F $(WINDRESFLAG)

OPT	= $(CLEANUP) -O3 -finline-functions
DOPT	= $(CLEANUP) -g -O0
XOPT	=

RCFLAGS		= --include-dir=./include
# Uncomment this if config.h defines RETAIN_WSALASTERROR
#LFLAGS		= -lwsock32

# ----------------------------------------------------------------------
# The library can be built with some alternative behaviour to
# facilitate development of applications on Win32 that will be ported
# to other POSIX systems. Nothing definable here will make the library
# non-compliant, but applications that make assumptions that POSIX
# does not garrantee may fail or misbehave under some settings.
#
# PTW32_THREAD_ID_REUSE_INCREMENT
# Purpose:
# POSIX says that applications should assume that thread IDs can be
# recycled. However, Solaris and some other systems use a [very large]
# sequence number as the thread ID, which provides virtual uniqueness.
# Pthreads-win32 provides pseudo-unique IDs when the default increment
# (1) is used, but pthread_t is not a scalar type like Solaris's.
#
# Usage:
# Set to any value in the range: 0 <= value <= 2^wordsize
#
# Examples:
# Set to 0 to emulate non recycle-unique behaviour like Linux or *BSD.
# Set to 1 for recycle-unique thread IDs (this is the default).
# Set to some other +ve value to emulate smaller word size types
# (i.e. will wrap sooner).
#
#PTW32_FLAGS	= "-DPTW32_THREAD_ID_REUSE_INCREMENT=0"
#
# ----------------------------------------------------------------------

GC_CFLAGS	= $(PTW32_FLAGS) 
GCE_CFLAGS	= $(PTW32_FLAGS) -mthreads

## Mingw32
MAKE		?= make
CFLAGS	= $(OPT) $(XOPT) -I./include -DHAVE_CONFIG_H -Wall

STATIC_PTHR_OBJS	= \
                src/barrier.o \
		src/cond.o \
		src/misc.o \
		src/mutex.o \
                src/ref.o \
		src/rwlock.o \
		src/sched.o \
                src/sem.o \
		src/semaphore.o \
		src/spinlock.o \
		src/thread.o

DLL_INLINED_OBJS	= \
                src/barrier.o \
		src/cond.o \
		src/misc.o \
		src/mutex.o \
                src/ref.o \
		src/rwlock.o \
		src/sched.o \
                src/sem.o \
		src/semaphore.o \
		src/spinlock.o \
		src/thread.o \
		src/version.o

# Agregate modules for inlinability
DLL_OBJS	= \
		src/barrier.o \
		src/cond.o \
		src/misc.o \
		src/mutex.o \
                src/ref.o \
		src/rwlock.o \
		src/sched.o \
                src/sem.o \
		src/semaphore.o \
		src/spinlock.o \
                src/thread.o \
		src/version.o

INCL	= \
		semaphore.h \
		pthread.h


BARRIER_SRCS = \
		src/barrier.c


COND_SRCS	= \
		src/cond.c


MISC_SRCS	= \
		src/misc.c

MUTEX_SRCS	= \
		src/mutex.c \
                src/ref.c

THREAD_SRCS	= \
		src/thread.c

RWLOCK_SRCS	= \
		src/rwlock.c

SCHED_SRCS	= \
		src/sched.c

SEMAPHORE_SRCS = \
		src/sem.c \
		src/semaphore.c

SPINLOCK_SRCS	= \
		src/spinlock.c


ifeq (,$(findstring i686, $(WTHR_TARGET_ARCH)))
ifeq (,$(findstring x86_64, $(target)))
#32bit
GCE_DLL	= outlib/pthreadGCE$(DLL_VER)-32.dll
GCED_DLL= outlib/pthreadGCE$(DLL_VERD)-32.dll
GCE_LIB	= outlib/libpthreadGCE$(DLL_VER)-32.a
GCED_LIB= outlib/libpthreadGCE$(DLL_VERD)-32.a
GCE_INLINED_STAMP = outlib/pthreadGCE$(DLL_VER)-32.stamp
GCED_INLINED_STAMP = outlib/pthreadGCE$(DLL_VERD)-32.stamp
GCE_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VER)-32.stamp
GCED_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VERD)-32.stamp

GC_DLL 	= outlib/pthreadGC$(DLL_VER)-32.dll
GCD_DLL	= outlib/pthreadGC$(DLL_VERD)-32.dll
GC_LIB	= outlib/libpthreadGC$(DLL_VER)-32.dll.a
GCD_LIB	= outlib/libpthreadGC$(DLL_VERD)-32.dll.a
GC_LIB_STATIC = outlib/libpthreadGC$(DLL_VER)-32.a
GC_INLINED_STAMP = outlib/pthreadGC$(DLL_VER)-32.stamp
GCD_INLINED_STAMP = outlib/pthreadGC$(DLL_VERD)-32.stamp
GC_STATIC_STAMP = outlib/libpthreadGC$(DLL_VER)-32.stamp
GCD_STATIC_STAMP = outlib/libpthreadGC$(DLL_VERD)-32.stamp
else
#64bit
GCE_DLL	= outlib/pthreadGCE$(DLL_VER)-64.dll
GCED_DLL= outlib/pthreadGCE$(DLL_VERD)-64.dll
GCE_LIB	= outlib/libpthreadGCE$(DLL_VER)-64.a
GCED_LIB= outlib/libpthreadGCE$(DLL_VERD)-64.a
GCE_INLINED_STAMP = outlib/pthreadGCE$(DLL_VER)-64.stamp
GCED_INLINED_STAMP = outlib/pthreadGCE$(DLL_VERD)-64.stamp
GCE_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VER)-64.stamp
GCED_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VERD)-64.stamp

GC_DLL 	= outlib/pthreadGC$(DLL_VER)-64.dll
GCD_DLL	= outlib/pthreadGC$(DLL_VERD)-64.dll
GC_LIB	= outlib/libpthreadGC$(DLL_VER)-64.dll.a
GCD_LIB	= outlib/libpthreadGC$(DLL_VERD)-64.dll.a
GC_LIB_STATIC = outlib/libpthreadGC$(DLL_VER)-64.a
GC_INLINED_STAMP = outlib/pthreadGC$(DLL_VER)-64.stamp
GCD_INLINED_STAMP = outlib/pthreadGC$(DLL_VERD)-64.stamp
GC_STATIC_STAMP = outlib/libpthreadGC$(DLL_VER)-64.stamp
GCD_STATIC_STAMP = outlib/libpthreadGC$(DLL_VERD)-64.stamp
endif
else
ifeq (,$(findstring x86_64, $(WTHR_TARGET_ARCH)))
#32bit
GCE_DLL	= outlib/pthreadGCE$(DLL_VER)-32.dll
GCED_DLL= outlib/pthreadGCE$(DLL_VERD)-32.dll
GCE_LIB	= outlib/libpthreadGCE$(DLL_VER)-32.a
GCED_LIB= outlib/libpthreadGCE$(DLL_VERD)-32.a
GCE_INLINED_STAMP = outlib/pthreadGCE$(DLL_VER)-32.stamp
GCED_INLINED_STAMP = outlib/pthreadGCE$(DLL_VERD)-32.stamp
GCE_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VER)-32.stamp
GCED_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VERD)-32.stamp

GC_DLL 	= outlib/pthreadGC$(DLL_VER)-32.dll
GCD_DLL	= outlib/pthreadGC$(DLL_VERD)-32.dll
GC_LIB	= outlib/libpthreadGC$(DLL_VER)-32.dll.a
GCD_LIB	= outlib/libpthreadGC$(DLL_VERD)-32.dll.a
GC_LIB_STATIC = outlib/libpthreadGC$(DLL_VER)-32.a
GC_INLINED_STAMP = outlib/pthreadGC$(DLL_VER)-32.stamp
GCD_INLINED_STAMP = outlib/pthreadGC$(DLL_VERD)-32.stamp
GC_STATIC_STAMP = outlib/libpthreadGC$(DLL_VER)-32.stamp
GCD_STATIC_STAMP = outlib/libpthreadGC$(DLL_VERD)-32.stamp
else
#64bit
GCE_DLL	= outlib/pthreadGCE$(DLL_VER)-64.dll
GCED_DLL= outlib/pthreadGCE$(DLL_VERD)-64.dll
GCE_LIB	= outlib/libpthreadGCE$(DLL_VER)-64.a
GCED_LIB= outlib/libpthreadGCE$(DLL_VERD)-64.a
GCE_INLINED_STAMP = outlib/pthreadGCE$(DLL_VER)-64.stamp
GCED_INLINED_STAMP = outlib/pthreadGCE$(DLL_VERD)-64.stamp
GCE_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VER)-64.stamp
GCED_STATIC_STAMP = outlib/libpthreadGCE$(DLL_VERD)-64.stamp

GC_DLL 	= outlib/pthreadGC$(DLL_VER)-64.dll
GCD_DLL	= outlib/pthreadGC$(DLL_VERD)-64.dll
GC_LIB	= outlib/libpthreadGC$(DLL_VER)-64.dll.a
GCD_LIB	= outlib/libpthreadGC$(DLL_VERD)-64.dll.a
GC_LIB_STATIC = outlib/libpthreadGC$(DLL_VER)-64.a
GC_INLINED_STAMP = outlib/pthreadGC$(DLL_VER)-64.stamp
GCD_INLINED_STAMP = outlib/pthreadGC$(DLL_VERD)-64.stamp
GC_STATIC_STAMP = outlib/libpthreadGC$(DLL_VER)-64.stamp
GCD_STATIC_STAMP = outlib/libpthreadGC$(DLL_VERD)-64.stamp
endif
endif

PTHREAD_DEF	= pthread.def

help:
	@ echo "Run one of the following command lines:"
	@ echo "make clean GC            (to build the GNU C dll with C cleanup code)"
	@ echo "make clean GCE           (to build the GNU C dll with C++ exception handling)"
	@ echo "make clean GC-inlined    (to build the GNU C inlined dll with C cleanup code)"
	@ echo "make clean GCE-inlined   (to build the GNU C inlined dll with C++ exception handling)"
	@ echo "make clean GC-static     (to build the GNU C inlined static lib with C cleanup code)"
	@ echo "make clean GC-debug      (to build the GNU C debug dll with C cleanup code)"
	@ echo "make clean GCE-debug     (to build the GNU C debug dll with C++ exception handling)"
	@ echo "make clean GC-inlined-debug    (to build the GNU C inlined debug dll with C cleanup code)"
	@ echo "make clean GCE-inlined-debug   (to build the GNU C inlined debug dll with C++ exception handling)"
	@ echo "make clean GC-static-debug     (to build the GNU C inlined static debug lib with C cleanup code)"

all:
	@ $(MAKE) clean GCE
	@ $(MAKE) clean GC

GC:
		$(MAKE) CLEANUP=-D__CLEANUP_C XC_FLAGS="$(GC_CFLAGS)" OBJ="$(DLL_OBJS)" $(GC_DLL)

GC-debug:
		$(MAKE) CLEANUP=-D__CLEANUP_C XC_FLAGS="$(GC_CFLAGS)" OBJ="$(DLL_OBJS)" DLL_VER=$(DLL_VERD) OPT="$(DOPT)" $(GCD_DLL)

GCE:
		$(MAKE) CC=$(CXX) CLEANUP=-D__CLEANUP_CXX XC_FLAGS="$(GCE_CFLAGS)" OBJ="$(DLL_OBJS)" $(GCE_DLL)

GCE-debug:
		$(MAKE) CC=$(CXX) CLEANUP=-D__CLEANUP_CXX XC_FLAGS="$(GCE_CFLAGS)" OBJ="$(DLL_OBJS)" DLL_VER=$(DLL_VERD) OPT="$(DOPT)" $(GCED_DLL)

GC-inlined:
		$(MAKE) XOPT="-DPTW32_BUILD_INLINED" CLEANUP=-D__CLEANUP_C XC_FLAGS="$(GC_CFLAGS)" OBJ="$(DLL_INLINED_OBJS)" $(GC_INLINED_STAMP)

GC-inlined-debug:
		$(MAKE) XOPT="-DPTW32_BUILD_INLINED" CLEANUP=-D__CLEANUP_C XC_FLAGS="$(GC_CFLAGS)" OBJ="$(DLL_INLINED_OBJS)" DLL_VER=$(DLL_VERD) OPT="$(DOPT)" $(GCD_INLINED_STAMP)

GCE-inlined:
		$(MAKE) CC=$(CXX) XOPT="-DPTW32_BUILD_INLINED" CLEANUP=-D__CLEANUP_CXX XC_FLAGS="$(GCE_CFLAGS)" OBJ="$(DLL_INLINED_OBJS)" $(GCE_INLINED_STAMP)

GCE-inlined-debug:
		$(MAKE) CC=$(CXX) XOPT="-DPTW32_BUILD_INLINED" CLEANUP=-D__CLEANUP_CXX XC_FLAGS="$(GCE_CFLAGS)" OBJ="$(DLL_INLINED_OBJS)" DLL_VER=$(DLL_VERD) OPT="$(DOPT)" $(GCED_INLINED_STAMP)

GC-static:
		$(MAKE) XOPT="-DPTW32_BUILD_INLINED -DWTHR_STATIC_LIB" CLEANUP=-D__CLEANUP_C XC_FLAGS="$(GC_CFLAGS)" OBJ="$(STATIC_PTHR_OBJS)" $(GC_STATIC_STAMP)

GC-static-debug:
		$(MAKE) XOPT="-DPTW32_BUILD_INLINED -DWTHR_STATIC_LIB" CLEANUP=-D__CLEANUP_C XC_FLAGS="$(GC_CFLAGS)" OBJ="$(STATIC_PTHR_OBJS)" DLL_VER=$(DLL_VERD) OPT="$(DOPT)" $(GCD_STATIC_STAMP)

tests:
	@ cd tests
	@ $(MAKE) auto

%.pre: %.c
	$(CC) -E -o $@ $(CFLAGS) $^

%.s: %.c
	$(CC) -c $(CFLAGS) -DPTW32_BUILD_INLINED -Wa,-ahl $^ > $@

%.o: %.rc
	$(RC) $(RCFLAGS) $(CLEANUP) -o $@ $<

.SUFFIXES: .dll .rc .c .o

.c.o:;		 $(CC) -c -o $@ $(CFLAGS) $(XC_FLAGS) $<


$(GC_DLL) $(GCD_DLL): $(DLL_OBJS)
	$(CC) $(OPT) -shared -o $(GC_DLL) $(DLL_OBJS) $(LFLAGS)
	$(DLLTOOL) -z pthread.def --export-all-symbols $(DLL_OBJS)
	$(DLLTOOL) -k --dllname $@ --output-lib $(GC_LIB) --input-def $(PTHREAD_DEF)

$(GCE_DLL): $(DLL_OBJS)
	$(CC) $(OPT) -mthreads -shared -o $(GCE_DLL) $(DLL_OBJS) $(LFLAGS)
	$(DLLTOOL) -z pthread.def --export-all-symbols $(DLL_OBJS)
	$(DLLTOOL) -k --dllname $@ --output-lib $(GCE_LIB) --input-def $(PTHREAD_DEF)

$(GC_INLINED_STAMP) $(GCD_INLINED_STAMP): $(DLL_INLINED_OBJS)
	$(CC) $(OPT) $(XOPT) -shared -o $(GC_DLL) $(DLL_INLINED_OBJS) $(LFLAGS)
	$(DLLTOOL) -z pthread.def --export-all-symbols $(DLL_INLINED_OBJS)
	$(DLLTOOL) -k --dllname $(GC_DLL) --output-lib $(GC_LIB) --def $(PTHREAD_DEF)
	echo touched > $(GC_INLINED_STAMP)

$(GCE_INLINED_STAMP) $(GCED_INLINED_STAMP): $(DLL_INLINED_OBJS)
	$(CC) $(OPT) $(XOPT) -mthreads -shared -o $(GCE_DLL) $(DLL_INLINED_OBJS)  $(LFLAGS)
	$(DLLTOOL) -z pthread.def --export-all-symbols $(DLL_INLINED_OBJS)
	$(DLLTOOL) -k --dllname $(GCE_DLL) --output-lib $(GCE_LIB) --def $(PTHREAD_DEF)
	echo touched > $(GCE_INLINED_STAMP)

$(GC_STATIC_STAMP) $(GCD_STATIC_STAMP): $(STATIC_PTHR_OBJS)
	$(RM) $(GC_LIB_STATIC)
	$(AR) -rv $(GC_LIB_STATIC) $(STATIC_PTHR_OBJS)
	$(RANLIB) $(GC_LIB_STATIC)
	echo touched > $(GC_STATIC_STAMP)

clean:
	-$(RM) *~
	-$(RM) *.i
	-$(RM) src/*.o
	-$(RM) src/*.obj
	-$(RM) src/*.exe
	-$(RM) outlib/*.a
	-$(RM) outlib/*.dll
	-$(RM) outlib/*.stamp
	-$(RM) $(PTHREAD_DEF)

realclean: clean
	-$(RM) $(GC_LIB)
	-$(RM) $(GCE_LIB)
	-$(RM) $(GC_DLL)
	-$(RM) $(GCE_DLL)
	-$(RM) $(GC_INLINED_STAMP)
	-$(RM) $(GCE_INLINED_STAMP)
	-$(RM) $(GC_STATIC_STAMP)
	-$(RM) $(GCD_LIB)
	-$(RM) $(GCED_LIB)
	-$(RM) $(GCD_DLL)
	-$(RM) $(GCED_DLL)
	-$(RM) $(GCD_INLINED_STAMP)
	-$(RM) $(GCED_INLINED_STAMP)
	-$(RM) $(GCD_STATIC_STAMP)

src\barrier.o:	src\barrier.c $(BARRIER_SRCS)
src\cond.o:	src\cond.c $(COND_SRCS)
src\misc.o:		src\misc.c $(MISC_SRCS)
src\mutex.o:	src\mutex.c $(MUTEX_SRCS)
src\thread.o:	src\thread.c $(THREAD_SRCS)
src\ref.o:   src\ref.c $(MUTEX_SRCS)
src\rwlock.o:	src\rwlock.c $(RWLOCK_SRCS)
src\sched.o:	src\sched.c $(SCHED_SRCS)
src\sem.o:   src\sem.c $(SEMAPHORE_SRCS)
src\semaphore.o:	src\semaphore.c $(SEMAPHORE_SRCS)
src\spinlock.o:		src\spinlock.c $(SPINLOCK_SRCS)
src\version.o:	src\version.rc

install:
	install -d $(prefix)/$(target)/bin
	install -d $(prefix)/$(target)/lib
	install -d $(prefix)/$(target)/include
	install $(GC_DLL) $(prefix)/$(target)/bin/pthreadGC$(DLL_VER).dll
	install $(GC_LIB) $(prefix)/$(target)/lib/libpthread.a
	install pthreads_win32_config.h pthread.h semaphore.h $(prefix)/$(target)/include
