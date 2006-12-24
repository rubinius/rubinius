CC=gcc
CFLAGS=-Wall -g -ggdb3 -I../include -iquote . -iquote lib `pkg-config glib-2.0 --cflags`
DEPS=../include/rubinius.h
LIBS=`pkg-config glib-2.0 --libs` -lncurses -lz external_libs/libtommath/libtommath.a external_libs/libedit/libedit.a external_libs/onig/.libs/libonig.a
OBJS=$(patsubst %.c,%.o,$(shell ls *.c))
RBLIB=lib/librubinius.a

all: rubinius

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

external_libs/libtommath/libtommath.a:
	cd external_libs/libtommath; $(MAKE)

external_libs/libedit/libedit.a:
	cd external_libs/libedit; ./configure; $(MAKE) libedit.a

external_libs/onig/.libs/libonig.a:
	cd external_libs/onig; ./configure; $(MAKE)

librubinius.a: external_libs/libtommath/libtommath.a external_libs/libedit/libedit.a external_libs/onig/.libs/libonig.a
	cd lib; $(MAKE) librubinius.a

.PHONY: librubinius.a

rubinius: librubinius.a main.o
	$(CC) -o rubinius main.o lib/librubinius.a $(LIBS)

test/test_state: test/test_state.c librubinius.a
	$(CC) -c -o test/test_state.o test/test_state.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_state test/test_state.o $(RBLIB)

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
	cd external_libs/libedit; $(MAKE) clean || exit 0
#	cd external_libs/onig; $(MAKE) clean

.PHONY: test
