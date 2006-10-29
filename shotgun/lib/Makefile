CC=gcc
CFLAGS=-Wall -g -ggdb3 -I../../include -iquote . `pkg-config glib-2.0 --cflags`
DEPS=auto.h ../../include/rubinius.h
LIBS=`pkg-config glib-2.0 --libs`
OBJS=$(patsubst %.c,%.o,$(shell ls *.c))
RUBY=ruby -I../../lib

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

librubinius.a: $(DEPS) $(OBJS) cpu_primitives.o cpu_instructions.o
	libtool -static -o librubinius.a *.o

cpu_instructions.o: instructions.gen instruction_names.gen
cpu_primitives.o: system_primitives.gen

instruction_names.gen instructions.gen: instructions.rb
	$(RUBY) instructions.rb > instructions.gen

system_primitives.gen: primitives.rb
	$(RUBY) primitives.rb > system_primitives.gen

auto.h auto.c: genheader.rb
	$(RUBY) genheader.rb > auto.c
	$(CC) -c -o auto.o auto.c $(CFLAGS)

clean:
	rm -f *.o *.gen *.a auto.c auto.h

.PHONY: test
