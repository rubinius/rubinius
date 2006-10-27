CC=gcc
CFLAGS=-Wall -g -ggdb3 -I../include -iquote . -iquote lib `pkg-config glib-2.0 --cflags`
DEPS=../include/rubinius.h
LIBS=`pkg-config glib-2.0 --libs`
OBJS=$(patsubst %.c,%.o,$(shell ls *.c))
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

librubinius.a:
	cd lib; $(MAKE) librubinius.a

.PHONY: librubinius.a

rubinius: librubinius.a main.o
	$(CC) $(LIBS) -o rubinius main.o lib/librubinius.a

test/test_state: test/test_state.c $(DEPS) $(OBJS)
	$(CC) -c -o test/test_state.o test/test_state.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_state test/test_state.o *.o

test/test_instructions: test/test_instructions.c $(DEPS) $(OBJS)
	$(CC) -c -o test/test_instructions.o test/test_instructions.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_instructions test/test_instructions.o *.o

test: test/test_state test/test_instructions
	./test/test_state
	./test/test_instructions

clean:
	rm -f *.o *.gen rubinius
	cd lib; $(MAKE) clean

.PHONY: test
