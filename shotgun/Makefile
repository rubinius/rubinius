CC=gcc
CFLAGS=-Wall -g -ggdb3 -I../include -iquote . `pkg-config glib-2.0 --cflags`
DEPS=auto.h ../include/rubinius.h
LIBS=`pkg-config glib-2.0 --libs`
OBJS=$(patsubst %.c,%.o,$(shell ls *.c))
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rubinius: $(DEPS) $(OBJS) cpu_primitives.o cpu_instructions.o
	$(CC) $(LIBS) -o rubinius *.o

cpu_instructions.o: instructions.gen instruction_names.gen
cpu_primitives.o: system_primitives.gen

instruction_names.gen instructions.gen: instructions.rb
	ruby -I../lib instructions.rb > instructions.gen

system_primitives.gen: primitives.rb
	ruby -I../lib primitives.rb > system_primitives.gen

test/test_state: test/test_state.c $(DEPS) $(OBJS)
	$(CC) -c -o test/test_state.o test/test_state.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_state test/test_state.o *.o

test/test_instructions: test/test_instructions.c $(DEPS) $(OBJS)
	$(CC) -c -o test/test_instructions.o test/test_instructions.c $(CFLAGS)
	$(CC) $(LIBS) -o test/test_instructions test/test_instructions.o *.o

test: test/test_state test/test_instructions
	./test/test_state
	./test/test_instructions

auto.h auto.c: genheader.rb
	ruby -I../lib genheader.rb > auto.c

clean:
	rm *.o *.gen rubinius

.PHONY: test
