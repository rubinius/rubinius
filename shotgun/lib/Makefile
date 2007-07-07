-include ../config.mk
include ../common.mk

ifeq ($(UNAME),Darwin)
  SINGLE_MODULE=-Wl,-single_module
  export MACOSX_DEPLOYMENT_TARGET=10.4
else
  SINGLE_MODULE=
endif

CPPFLAGS=-I../../include -I .. -iquote . -I../external_libs/lightning -I../external_libs/libevent -I../external_libs/onig -I../external_libs/libtommath -I../external_libs/libzip/lib -I../external_libs/libltdl -D_GNU_SOURCE

LIBS=`pkg-config glib-2.0 --libs` -lz ../external_libs/libtommath/libtommath.a ../external_libs/onig/.libs/libonig.a ../external_libs/libzip/lib/.libs/libzip.a $(SINGLE_MODULE) ../external_libs/libevent/.libs/libevent.a ../external_libs/libltdl/.libs/libltdl.a -lm

# No disass support for 64
ifneq ($(findstring 64,$(MARCH)),64)
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

# BSD comes with strlcat and strlcpy
ifeq ($(findstring BSD,$(UNAME)),BSD)
  CPPFLAGS+=-DHAVE_STRLCAT -DHAVE_STRLCPY
endif

# OSX comes with strlcat and strlcpy
ifeq ($(UNAME),Darwin)
  CPPFLAGS+=-DHAVE_STRLCAT -DHAVE_STRLCPY
endif

# Darwin doesn't need -lcrypt
ifneq ($(UNAME),Darwin)
  LIBS+="-lcrypt"
endif

ifdef DEV
  OPTIMIZATIONS=
else
  INLINE_OPTS=-finline-limit=2000 --param max-inline-insns-single=3500 --param large-function-growth=2000 --param inline-unit-growth=1000
  OPTIMIZATIONS=-ffast-math -funroll-loops -finline-functions -fno-omit-frame-pointer $(INLINE_OPTS)
endif
ifeq ($(CPU), powerpc)
  OPTIMIZATIONS+=-falign-loops=16
endif

CFLAGS += -fPIC $(CPPFLAGS)
DEPS=auto.h system_primitives.gen instruction_names.c instruction_names.h \
     node_types.c node_types.h

SOURCES=$(sort \
        $(wildcard *.c) \
        $(wildcard subtend/*.c) \
        grammar.c \
        auto.c \
        instruction_names.c \
        node_types.c \
        )
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
	$(COMP) $(CFLAGS) $(OPTIMIZATIONS) -c $< -o $@

subtend/PortableUContext_asm.lo: subtend/PortableUContext_asm.S
	$(COMP) $(CFLAGS) $(OPTIMIZATIONS) -c $< -o $@

.%.d:  %.c  $(DEPS)
	@echo DEP $<
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.lo $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

librubinius.la: show_info $(DEPS) $(LOBJS) 
	$(LINKER) -release $(VERSION) -o librubinius.la $(LOBJS) -rpath $(LIBPATH) $(LIBS)

cpu_instructions.o: instructions.gen instruction_names.c instruction_names.h
cpu_primitives.o: system_primitives.gen
object_memory.o: object_memory.h

instruction_names.c instruction_names.h instructions.gen: instructions.rb
	$(RUBY) instructions.rb > instructions.gen

system_primitives.gen: primitives.rb
	$(RUBY) primitives.rb > system_primitives.gen

auto.h auto.c: genheader.rb ../../native/bytecode/system_hints.rb
	$(RUBY) genheader.rb > auto.c

node_types.c node_types.h: node_types.rb
	$(RUBY) node_types.rb

grammar.lo:
	$(COMP) $(CFLAGS) -c grammar.c

subtend/ffi.lo: subtend/ffi.c
	$(COMP) $(CFLAGS) $(OPTIMIZATIONS) -Wno-unused-variable -Wno-unused-value -c $< -o $@

# In a perfect world, we'd only rebuild grammar.c if grammar.y changed.
# However, SVN likes to play with the file mtime, which will just mess
# this up. So don't depend on grammar.y here.
grammar.c:
	bison -o grammar.c grammar.y

clean:
	rm -f *.o *.lo *.la subtend/*.o subtend/*.lo *.gen *.a auto.c auto.h .*.d .*.d.* instruction_names.c instruction_names.h node_types.c node_types.h
	rm -rf .libs subtend/.libs

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
