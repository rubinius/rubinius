CC=gcc
UNAME=$(shell uname)
CPPFLAGS=-I../../include -iquote . -I../external_libs/onig -I../external_libs/libtommath -I../external_libs/libzip/lib `pkg-config glib-2.0 --cflags`
CFLAGS=-Wall -g -fno-omit-frame-pointer $(CPPFLAGS)
DEPS=auto.h system_primitives.gen instruction_names.gen
LIBS=`pkg-config glib-2.0 --libs` ../external_libs/libtommath/libtommath.a

SOURCES=$(wildcard *.c) grammar.c
OBJS=$(SOURCES:.c=.o)
RUBY=ruby -I../../lib

ifeq ($(UNAME),SunOS)
  CFLAGS+=-D__C99FEATURES__
else
  ifeq ($(UNAME),Darwin)
    OBJS+=missing/backtrace.o
    CFLAGS+=-D__APPLE__=1
  endif
endif

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.%.d:  %.c  $(DEPS)
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

librubinius.a: $(DEPS) $(OBJS) 
	rm -f librubinius.a
	ar rc librubinius.a $(OBJS)
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

grammar.c: grammar.y
	bison -o grammar.c grammar.y

clean:
	rm -f *.o *.gen *.a auto.c auto.h .*.d .*.d.* grammar.c

.PHONY: test

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
