-include ../../config.mk

# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)
OSVER=$(shell uname -r)

WARNINGS = -Wall
DEBUG = -g -ggdb3

CFLAGS = $(WARNINGS) $(DEBUG)

COMP=$(CC)
ifeq ($(UNAME),Darwin)
	CFLAGS += -arch ppc -arch i386
  LDOPT=-dynamiclib
  LINKER=$(CC) $(LDOPT)
  SUFFIX=dylib
  BIN_RPATH=
  SONAME=-current_version $(VERSION) -compatibility_version $(VERSION)
else
  LDOPT=-shared
  LINKER=$(CC) -shared
  BIN_RPATH=
  SUFFIX=so
  SONAME=-Wl,-soname,libmquark-$(VERSION).$(SUFFIX)
endif
RANLIB = ranlib

ifndef VERBOSE
  COMP=@echo CC $@;$(CC)
  LINKER=@echo LINK $@;$(CC) $(LDOPT)
endif

VERSION=0.1

NAME=libmquark
OUTPUT_LIB=$(NAME).$(VERSION).$(SUFFIX)
OUTPUT_A=$(NAME).a

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

ifdef DEV
  OPTIMIZATIONS=
else
  INLINE_OPTS=
  OPTIMIZATIONS=-O2 -funroll-loops -finline-functions $(INLINE_OPTS)
endif

ifeq ($(CPU), powerpc)
  OPTIMIZATIONS+=-falign-loops=16
endif

CPPFLAGS += -I../libcchash -I../libmpa
CFLAGS += -fPIC $(CPPFLAGS)
DEPS=quark.h

SOURCES=quark.c
OBJS=$(SOURCES:.c=.o)
LIBS=-L../libcchash -L../libmpa -lcchash -lptr_array -lm
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

library: $(OUTPUT_LIB) $(OUTPUT_A)

$(OUTPUT_LIB): $(DEPS) $(OBJS) 
	$(LINKER) -o $(OUTPUT_LIB) $(OBJS) $(SONAME) $(LIBS)

$(OUTPUT_A): $(DEPS) $(OBJS)
	$(AR) cru $(OUTPUT_A) $(OBJS)
	$(RANLIB) $(OUTPUT_A)

.PHONY: library

clean:
	rm -f *.o *.lo *.la *.so *.dylib *.a tester

test: tester
	./tester

tester: tester.o $(OUTPUT_A)
	$(CC) -o tester $(OBJS) tester.o $(LIBS) $(OUTPUT_A)

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
