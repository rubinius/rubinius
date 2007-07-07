include config.mk
include common.mk

INSTALL=$(LIBTOOL) --mode=install install
MKDIR=install -d

ifdef DEV
  BINSUFFIX?=-dev
else
  BINSUFFIX?=
endif

CFLAGS += -I../include -iquote . -iquote lib $(CPPFLAGS)

# amd64 must have fPIC shared
ifeq ($(MARCH),amd64)
	CFLAGS+=-fPIC
endif

DEPS=../include/rubinius.h

ALIBS=external_libs/libtommath/libtommath.a external_libs/onig/.libs/libonig.a external_libs/libzip/lib/.libs/libzip.a external_libs/libltdl/.libs/libltdl.a external_libs/libevent/.libs/libevent.a

# No disass support for 64
ifneq ($(findstring 64,$(MARCH)),64)
	RBS_LIGHTNING_DISASS='--enable-disassembling'
	ALIBS+=external_libs/lightning/opcode/libdisass.a
endif	

LIBS=`pkg-config glib-2.0 --libs` $(ALIBS) -lz -lm
LDFLAGS=

ifdef USE_CINVOKE
CFLAGS += -Iexternal_libs/cinvoke/lib
LIBS+= external_libs/cinvoke/lib/libcinvoke.a
ALIBS+= external_libs/cinvoke/lib/libcinvoke.a
endif

ifeq ($(UNAME),SunOS)
  LDFLAGS+=-lrt
endif

# Support *BSD libexecinfo for backtrace etc.
ifeq ($(findstring BSD,$(UNAME)),BSD)
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

external_libs/lightning/config.h:
	cd external_libs/lightning; CFLAGS=-fPIC ./configure $(RBS_LIGHTNING_DISASS); $(MAKE)

external_libs/lightning/opcode/libdisass.a: external_libs/lightning/config.h

external_libs/libtommath/libtommath.a:
	cd external_libs/libtommath; $(MAKE)

external_libs/onig/config.h:
	cd external_libs/onig; CFLAGS=-fPIC ./configure

external_libs/onig/.libs/libonig.a:	external_libs/onig/config.h
	cd external_libs/onig; $(MAKE)

external_libs/libzip/config.h:
	cd external_libs/libzip; CFLAGS=-fPIC ./configure

external_libs/libzip/lib/.libs/libzip.a:	external_libs/libzip/config.h
	cd external_libs/libzip; $(MAKE)

# cinvoke doesn't produce a config.h
external_libs/cinvoke/lib/libcinvoke.a:
	cd external_libs/cinvoke; CFLAGS=-fPIC ./configure; $(MAKE)

external_libs/libevent/config.h:
	cd external_libs/libevent; CFLAGS=-fPIC ./configure

external_libs/libevent/.libs/libevent.a: external_libs/libevent/config.h
	cd external_libs/libevent; $(MAKE) libevent.la

external_libs/libltdl/config.h:
	cd external_libs/libltdl; CFLAGS=-fPIC ./configure

external_libs/libltdl/.libs/libltdl.a: external_libs/libltdl/config.h
	cd external_libs/libltdl; $(MAKE)

librubinius.la: config.h $(ALIBS) external_libs/lightning/config.h 
	cd lib; $(MAKE) librubinius.la; 

.PHONY: librubinius.la

# Don't try to build main until librubinius exists
main.c: librubinius.la

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

.PHONY: test

clean:
	rm -f *.o *.lo *.gen rubinius config.h config.mk
	rm -rf .libs
	cd lib; $(MAKE) clean
	cd external_libs/libtommath; $(MAKE) clean
	cd external_libs/onig; $(MAKE) clean
	cd external_libs/libevent; $(MAKE) clean

.PHONY: clean

# Clean, but also nuke configs
distclean: clean
	cd external_libs/lightning; rm -f config.h
	cd external_libs/onig; rm -f config.h
	cd external_libs/libzip; rm -f config.h
	cd external_libs/libevent; rm -f config.h
	cd external_libs/libltdl; rm -f config.h
.PHONY: distclean
