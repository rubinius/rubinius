CC=gcc
CPPFLAGS=-I../../include -iquote . -I../external_libs/onig -I../external_libs/libtommath -I../external_libs/libedit `pkg-config glib-2.0 --cflags`
CFLAGS=-Wall -g -ggdb3 $(CPPFLAGS)
DEPS=auto.h system_primitives.gen instruction_names.gen
LIBS=`pkg-config glib-2.0 --libs` ../external_libs/libtommath/libtommath.a
SOURCES=$(wildcard *.c)
OBJS=$(SOURCES:.c=.o)
RUBY=ruby -I../../lib


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.%.d:  %.c  $(DEPS)
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

librubinius.a: $(DEPS) $(OBJS) cpu_primitives.o cpu_instructions.o
	rm -f librubinius.a
	ar rc librubinius.a *.o
	ranlib librubinius.a

cpu_instructions.o: instructions.gen instruction_names.gen
cpu_primitives.o: system_primitives.gen
object_memory.o: object_memory.h

instruction_names.gen instructions.gen: instructions.rb
	$(RUBY) instructions.rb > instructions.gen

system_primitives.gen: primitives.rb
	$(RUBY) primitives.rb > system_primitives.gen

auto.h auto.c: genheader.rb
	$(RUBY) genheader.rb > auto.c
	$(CC) -c -o auto.o auto.c $(CFLAGS)

grammer.c: grammer.y
	bison -o grammer.c grammer.y

clean:
	rm -f *.o *.gen *.a auto.c auto.h .*.d .*.d.*

.PHONY: test

-include $(SOURCES:%.c=.%.d)
