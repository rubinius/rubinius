CC=gcc
CFLAGS=-Wall -g -ggdb3 -I../include -iquote . -iquote lib `pkg-config glib-2.0 --cflags`
DEPS=../include/rubinius.h
LIBS=`pkg-config glib-2.0 --libs` -lncurses libs/libtommath/libtommath.a libs/libedit/libedit.a libs/onig/.libs/libonig.a
OBJS=$(patsubst %.c,%.o,$(shell ls *.c))
RBLIB=lib/librubinius.a

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libs/libtommath/libtommath.a:
	cd libs/libtommath; $(MAKE)

libs/libedit/libedit.a:
	cd libs/libedit; ./configure; $(MAKE) libedit.a

libs/onig/.libs/libonig.a:
	cd libs/onig; ./configure; $(MAKE)

librubinius.a: libs/libtommath/libtommath.a libs/libedit/libedit.a libs/onig/.libs/libonig.a
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
	cd libs/libtommath; $(MAKE) clean
	cd libs/libedit; $(MAKE) clean || exit 0
	cd libs/onig; $(MAKE) clean

.PHONY: test
