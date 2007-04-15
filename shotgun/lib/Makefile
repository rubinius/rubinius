-include ../config.mk

CC=gcc
COMP=$(LIBTOOL) --mode=compile $(CC)
LINKER=$(LIBTOOL) --mode=link $(CC)
UNAME=$(shell uname)
CPU=$(shell uname -p)

ifeq ($(UNAME),Darwin)
  SINGLE_MODULE=-Wl,-single_module
else
  SINGLE_MODULE=
endif

CPPFLAGS=-I../../include -I .. -iquote . -I../external_libs/libevent -I../external_libs/cinvoke/lib -I../external_libs/onig -I../external_libs/libtommath -I../external_libs/libzip/lib `pkg-config glib-2.0 --cflags`
WARNINGS=-Wall -Winline
LIBS=`pkg-config glib-2.0 --libs` -lz ../external_libs/libtommath/libtommath.a ../external_libs/onig/.libs/libonig.a ../external_libs/libzip/lib/.libs/libzip.a ../external_libs/cinvoke/lib/libcinvoke.a $(SINGLE_MODULE) ../external_libs/libevent/.libs/libevent.a -lm

ifdef DEV
  OPTIMIZATIONS=
  WARNINGS=-Wall
else
  INLINE_OPTS=-finline-limit=2000 --param max-inline-insns-single=3500 --param large-function-growth=2000 --param inline-unit-growth=1000
  OPTIMIZATIONS=-O2 -ffast-math -funroll-loops -finline-functions -fno-omit-frame-pointer $(INLINE_OPTS)
  WARNINGS=-Wall
endif
ifeq ($(CPU), powerpc)
  OPTIMIZATIONS+=-falign-loops=16
endif

DEBUG=-g
CFLAGS=$(WARNINGS) $(OPTIMIZATIONS) $(DEBUG) $(CPPFLAGS)
DEPS=auto.h system_primitives.gen instruction_names.gen

SOURCES=$(sort $(wildcard *.c) $(wildcard subtend/*.c) grammar.c)
OBJS=$(SOURCES:.c=.o)
LOBJS=$(SOURCES:.c=.lo)
RUBY=ruby -I../../lib

ifeq ($(UNAME),SunOS)
  CFLAGS+=-D__C99FEATURES__
endif

show_info:
	@echo "Warnings: $(WARNINGS)"
	@echo "Optimizations: $(OPTIMIZATIONS)"
	@echo "Debugging: $(DEBUG)"

%.lo: %.c
	@echo "CC $@"
	@$(COMP) $(CFLAGS) -c $< -o $@

.%.d:  %.c  $(DEPS)
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.lo $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

librubinius.la: show_info $(DEPS) $(LOBJS) 
	MACOSX_DEPLOYMENT_TARGET=10.4 $(LINKER) -release $(VERSION) -o librubinius.la $(LOBJS) -rpath $(LIBPATH) -ldl $(LIBS)

cpu_instructions.o: instructions.gen instruction_names.gen
cpu_primitives.o: system_primitives.gen
object_memory.o: object_memory.h

instruction_names.gen instructions.gen: instructions.rb
	$(RUBY) instructions.rb > instructions.gen

system_primitives.gen: primitives.rb
	$(RUBY) primitives.rb > system_primitives.gen

auto.h auto.c: genheader.rb
	$(RUBY) genheader.rb > auto.c
	echo "CC auto.o"
	$(COMP) $(CFLAGS) -c auto.c

grammar.lo:
	@echo "CC grammar.o"
	@$(COMP) -Wall -O $(DEBUG) $(CPPFLAGS) -c grammar.c 

grammar.c: grammar.y
	bison -o grammar.c grammar.y

clean:
	rm -f *.o *.lo *.la subtend/*.o subtend/*.lo *.gen *.a auto.c auto.h .*.d .*.d.* grammar.c
	rm -rf .libs subtend/.libs

.PHONY: test

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
