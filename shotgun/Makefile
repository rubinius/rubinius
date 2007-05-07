include config.mk

# Respect the environment
ifeq ($(CC),)
CC=gcc
endif

COMP=$(LIBTOOL) --mode=compile $(CC)
LINKER=$(LIBTOOL) --mode=link $(CC)
INSTALL=$(LIBTOOL) --mode=install install
MKDIR=install -d

ifdef DEV
  BINSUFFIX?=-dev
else
  BINSUFFIX?=
endif

UNAME=$(shell uname)
MARCH=$(shell uname -m)

CFLAGS=-Wall -g -ggdb3 -I../include -iquote . -iquote lib `pkg-config glib-2.0 --cflags` -Iexternal_libs/cinvoke/lib

# amd64 must have fPIC shared
ifeq ($(MARCH),amd64)
	CFLAGS+=-fPIC
endif

DEPS=../include/rubinius.h

ALIBS=external_libs/libtommath/libtommath.a external_libs/onig/.libs/libonig.a external_libs/libzip/lib/.libs/libzip.a external_libs/libltdl/.libs/libltdl.a external_libs/libevent/.libs/libevent.a

LIBS=`pkg-config glib-2.0 --libs` $(ALIBS) -lz -lm
LDFLAGS=

ifdef USE_CINVOKE
LIBS+= external_libs/cinvoke/lib/libcinvoke.a
ALIBS+= external_libs/cinvoke/lib/libcinvoke.a
endif

ifeq ($(UNAME),SunOS)
  LDFLAGS+=-lrt
endif

# Support *BSD libexecinfo for backtrace etc.
ifeq ($(UNAME),FreeBSD)
  LIBS+=-lexecinfo
endif

ifeq ($(UNAME),OpenBSD)
  LIBS+=-lexecinfo
endif


ifeq ($(UNAME),Linux)
	LIBS+=-ldl
endif

OBJS=$(patsubst %.c,%.o,$(shell ls *.c))
LOBJS=$(patsubst %.c,%.lo,$(shell ls *.c))
RBLIB=lib/librubinius.a

all: rubinius

install:
	$(MKDIR) $(LIBPATH)
	$(INSTALL) -c lib/librubinius.la $(LIBPATH)
	$(MKDIR) $(BINPATH)
	$(INSTALL) -c rubinius $(BINPATH)/rbx$(BINSUFFIX)

config.h:
	./mkconfig.sh

%.lo: %.c $(DEPS)
	$(COMP) $(CFLAGS) -c $< 

external_libs/libtommath/libtommath.a:
	cd external_libs/libtommath; $(MAKE)

external_libs/onig/.libs/libonig.a:
	cd external_libs/onig; CFLAGS=-fPIC ./configure; $(MAKE)

external_libs/libzip/lib/.libs/libzip.a:
	cd external_libs/libzip; CFLAGS=-fPIC ./configure; $(MAKE)

external_libs/cinvoke/lib/libcinvoke.a:
	cd external_libs/cinvoke; CFLAGS=-fPIC ./configure; $(MAKE)

external_libs/libevent/.libs/libevent.a:
	cd external_libs/libevent; CFLAGS=-fPIC ./configure; $(MAKE) libevent.la

external_libs/libltdl/.libs/libltdl.a:
	cd external_libs/libltdl; CFLAGS=-fPIC ./configure; $(MAKE)

librubinius.la: config.h $(ALIBS)
	cd lib; $(MAKE) librubinius.la

.PHONY: librubinius.a

rubinius: librubinius.la main.lo
	$(LINKER) -o rubinius main.lo lib/librubinius.la

test/test_state: test/test_state.c librubinius.a
	$(COMP) -c -o test/test_state.o test/test_state.c $(CFLAGS)
	$(COMP) $(LIBS) -o test/test_state test/test_state.o $(RBLIB)

test/test_gc: test/test_gc.c librubinius.a
	$(COMP) -c -o test/test_gc.o test/test_gc.c $(CFLAGS)
	$(COMP) $(LIBS) -o test/test_gc test/test_gc.o $(RBLIB)

test/test_instructions: test/test_instructions.c librubinius.a 
	$(COMP) -c -o test/test_instructions.o test/test_instructions.c $(CFLAGS)
	$(COMP) $(LIBS) -o test/test_instructions test/test_instructions.o $(RBLIB)

test: test/test_state test/test_instructions
	./test/test_state
	./test/test_instructions

clean:
	rm -f *.o *.lo *.gen rubinius config.h config.mk
	rm -rf .libs
	cd lib; $(MAKE) clean
	cd external_libs/libtommath; $(MAKE) clean
	cd external_libs/onig; $(MAKE) clean
	cd external_libs/libevent; $(MAKE) clean

.PHONY: test
