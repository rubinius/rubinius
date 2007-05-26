-include ../config.mk

# Respect ENV
ifeq ($(CC),)
   CC=gcc
endif


COMP=$(LIBTOOL) --mode=compile $(CC)
LINKER=$(LIBTOOL) --mode=link $(CC)
UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)

ifeq ($(UNAME),Darwin)
  SINGLE_MODULE=-Wl,-single_module
else
  SINGLE_MODULE=
endif

CPPFLAGS=-I../../include -I .. -iquote . -I../external_libs/lightning -I../external_libs/libevent -I../external_libs/onig -I../external_libs/libtommath -I../external_libs/libzip/lib -I../external_libs/libltdl `pkg-config glib-2.0 --cflags`

WARNINGS=-Wall -Winline

LIBS=`pkg-config glib-2.0 --libs` -lz ../external_libs/libtommath/libtommath.a ../external_libs/onig/.libs/libonig.a ../external_libs/libzip/lib/.libs/libzip.a $(SINGLE_MODULE) ../external_libs/libevent/.libs/libevent.a ../external_libs/libltdl/.libs/libltdl.a -lm

# No disass support for 64
ifneq ($(MARCH),amd64)	
	CPPFLAGS+="-DRBS_DISASS=1"
	LIBS+=../external_libs/lightning/opcode/libdisass.a
endif 

ifdef USE_CINVOKE
CPPFLAGS+= -I../external_libs/cinvoke/lib
LIBS+= ../external_libs/cinvoke/lib/libcinvoke.a
endif

# BSD do not require a separate libdl
ifneq ($(findstring BSD,$(UNAME)),BSD)
  LIBS+="-ldl"
endif

# Darwin doesn't need -lcrypt
ifneq ($(UNAME),Darwin)
  LIBS+="-lcrypt"
endif

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
CFLAGS=-fPIC $(WARNINGS) $(OPTIMIZATIONS) $(DEBUG) $(CPPFLAGS)
DEPS=auto.h system_primitives.gen instruction_names.gen

SOURCES=$(sort $(wildcard *.c) $(wildcard subtend/*.c) grammar.c)
OBJS=$(SOURCES:.c=.o) subtend/PortableUContext_asm.o
LOBJS=$(SOURCES:.c=.lo) subtend/PortableUContext_asm.lo
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
	$(COMP) $(CFLAGS) -c $< -o $@

subtend/PortableUContext_asm.lo:
	$(COMP) $(CFLAGS) -c subtend/PortableUContext_asm.S -o subtend/PortableUContext_asm.lo

.%.d:  %.c  $(DEPS)
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.lo $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

librubinius.la: show_info $(DEPS) $(LOBJS) 
	MACOSX_DEPLOYMENT_TARGET=10.4 $(LINKER) -release $(VERSION) -o librubinius.la $(LOBJS) -rpath $(LIBPATH) $(LIBS)

cpu_instructions.o: instructions.gen instruction_names.gen
cpu_primitives.o: system_primitives.gen
object_memory.o: object_memory.h

instruction_names.gen instructions.gen: instructions.rb
	$(RUBY) instructions.rb > instructions.gen

system_primitives.gen: primitives.rb
	$(RUBY) primitives.rb > system_primitives.gen

auto.h auto.c: genheader.rb ../../native/bytecode/system_hints.rb
	$(RUBY) genheader.rb > auto.c
	echo "CC auto.o"
	$(COMP) $(CFLAGS) -c auto.c

grammar.lo:
	@echo "CC grammar.o"
	@$(COMP) -Wall -O $(DEBUG) $(CPPFLAGS) -c grammar.c

subtend/ffi.lo: subtend/ffi.c
	@echo "CC subtend/ffi.o"
	$(COMP) $(CFLAGS) -Wno-unused-variable -Wno-unused-value -c $< -o $@

# only rebuild grammar.c if grammar.y changed
grammar.c: grammar.y
	bison -o grammar.c grammar.y

clean:
	rm -f *.o *.lo *.la subtend/*.o subtend/*.lo *.gen *.a auto.c auto.h .*.d .*.d.*
	rm -rf .libs subtend/.libs

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
