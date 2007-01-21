CC=gcc
UNAME=$(shell uname)
CFLAGS=-Wall -g -ggdb3 -I../include -iquote . -iquote lib `pkg-config glib-2.0 --cflags`
DEPS=../include/rubinius.h
LIBS=`pkg-config glib-2.0 --libs` -lz external_libs/libtommath/libtommath.a external_libs/onig/.libs/libonig.a external_libs/libzip/lib/.libs/libzip.a
LDFLAGS=

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
RBLIB=lib/librubinius.a

all: rubinius

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

external_libs/libtommath/libtommath.a:
	cd external_libs/libtommath; $(MAKE)

external_libs/onig/.libs/libonig.a:
	cd external_libs/onig; ./configure; $(MAKE)

external_libs/libzip/lib/.libs/libzip.a:
	cd external_libs/libzip; ./configure; $(MAKE)

librubinius.a: external_libs/libtommath/libtommath.a external_libs/onig/.libs/libonig.a external_libs/libzip/lib/.libs/libzip.a
	cd lib; $(MAKE) librubinius.a

.PHONY: librubinius.a

rubinius: librubinius.a main.o
	$(CC) -o rubinius main.o lib/librubinius.a $(LDFLAGS) $(LIBS)

test/test_state: test/test_state.c librubinius.a
	$(CC) -c -o test/test_state.o test/test_state.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_state test/test_state.o $(RBLIB)

test/test_gc: test/test_gc.c librubinius.a
	$(CC) -c -o test/test_gc.o test/test_gc.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_gc test/test_gc.o $(RBLIB)

test/test_instructions: test/test_instructions.c librubinius.a 
	$(CC) -c -o test/test_instructions.o test/test_instructions.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_instructions test/test_instructions.o $(RBLIB)

test: test/test_state test/test_instructions
	./test/test_state
	./test/test_instructions

clean:
	rm -f *.o *.gen rubinius
	cd lib; $(MAKE) clean
	cd external_libs/libtommath; $(MAKE) clean
	cd external_libs/onig; $(MAKE) clean

.PHONY: test
