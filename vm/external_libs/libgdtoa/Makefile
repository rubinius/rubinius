# Based on original makefile included with the source.
# The source for libgdtoa imported from FreeBSD 7.0
# in /usr/src/contrib/gdtoa.

-include ../../config.mk

# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

ifeq ($(AR),)
  AR=ar
endif

ifeq ($(RANLIB),)
  RANLIB=ranlib
endif

UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)
OSVER=$(shell uname -r)

WARNINGS = -Wall
DEBUG = -g -ggdb3

CFLAGS = $(WARNINGS) $(DEBUG) -fno-strict-aliasing
NAME=libgdtoa
VERSION = 1
COMP=$(CC)

ifndef VERBOSE
  COMP=@echo CC $@;$(CC)
  LINKER=@echo LINK $@;$(CC) $(LDOPT)
endif

OUTPUT_A = $(NAME).a

ifeq ($(UNAME),Darwin)
  SINGLE_MODULE=-Wl,-single_module
  ifeq ($(OSVER),9.1.0)
    export MACOSX_DEPLOYMENT_TARGET=10.5
  else
    export MACOSX_DEPLOYMENT_TARGET=10.4
  endif
else
  SINGLE_MODULE=
endif

ifeq ($(UNAME),SunOS)
  CFLAGS+=-D__C99FEATURES__
endif

ifeq ($(UNAME),OpenBSD)
  CFLAGS+=-DNO_FENV_H
endif

ifdef DEV
  OPTIMIZATIONS=
else
  INLINE_OPTS=
  OPTIMIZATIONS=-O2 -funroll-loops -finline-functions $(INLINE_OPTS)
endif

ifeq ($(CPU), powerpc)
  OPTIMIZATIONS+=-falign-loops=16
endif

CFLAGS += -fPIC $(CPPFLAGS)

DEPS = dmisc.c dtoa.c g_Qfmt.c g__fmt.c g_ddfmt.c g_dfmt.c g_ffmt.c\
       g_xLfmt.c g_xfmt.c gdtoa.c gethex.c gmisc.c hd_init.c hexnan.c\
       misc.c smisc.c strtoIQ.c strtoId.c strtoIdd.c strtoIf.c strtoIg.c\
       strtoIx.c strtoIxL.c strtod.c strtodI.c strtodg.c strtof.c strtopQ.c\
       strtopd.c strtopdd.c strtopf.c strtopx.c strtopxL.c strtorQ.c\
       strtord.c strtordd.c strtorf.c strtorx.c strtorxL.c sum.c ulp.c
OBJS = dmisc.o dtoa.o g_Qfmt.o g__fmt.o g_ddfmt.o g_dfmt.o g_ffmt.o\
       g_xLfmt.o g_xfmt.o gdtoa.o gethex.o gmisc.o hd_init.o hexnan.o\
       misc.o smisc.o strtoIQ.o strtoId.o strtoIdd.o strtoIf.o strtoIg.o\
       strtoIx.o strtoIxL.o strtod.o strtodI.o strtodg.o strtof.o strtopQ.o\
       strtopd.o strtopdd.o strtopf.o strtopx.o strtopxL.o strtorQ.o\
       strtord.o strtordd.o strtorf.o strtorx.o strtorxL.o sum.o ulp.o

all: library

arith.h: arithchk.c
	$(CC) $(CFLAGS) arithchk.c || $(CC) -DNO_LONG_LONG $(CFLAGS) arithchk.c
	./a.out >arith.h
	rm -f a.out arithchk.o

gd_qnan.h: arith.h qnan.c
	$(CC) $(CFLAGS) qnan.c
	./a.out >gd_qnan.h
	rm -f a.out qnan.o

$(OBJS): arith.h gd_qnan.h

%.o: %.c
	$(COMP) $(CFLAGS) $(OPTIMIZATIONS) -c $< -o $@

%.o: %.S
	$(COMP) $(CFLAGS) $(OPTIMIZATIONS) -c $< -o $@

.%.d:  %.c  $(DEPS)
	@echo DEP $<
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

library: $(OUTPUT_A)

$(OUTPUT_A): $(DEPS) $(OBJS)
	$(AR) cru $(OUTPUT_A) $(OBJS)
	$(RANLIB) $(OUTPUT_A)

.PHONY: library

test: library
	-cd test; $(MAKE)

.PHONY: test

clean:
	rm -f *.o *.lo *.la *.so *.a
	-cd test; $(MAKE) clean

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
