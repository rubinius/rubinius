-include ../../config.mk

# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)

WARNINGS = -Wall
DEBUG = -g -ggdb3

CFLAGS = $(WARNINGS) $(DEBUG)

COMP=$(CC)
ifeq ($(UNAME),Darwin)
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
  SONAME=-Wl,-soname,libbstrings-$(VERSION).$(SUFFIX)
endif

ifndef VERBOSE
  COMP=@echo CC $@;$(CC)
  LINKER=@echo LINK $@;$(CC) $(LDOPT)
endif

VERSION=0.32

OUTPUT_LIB=libbstring.$(VERSION).$(SUFFIX)
OUTPUT_A=libbstring.a

ifeq ($(UNAME),Darwin)
  SINGLE_MODULE=-Wl,-single_module
  export MACOSX_DEPLOYMENT_TARGET=10.4
else
  SINGLE_MODULE=
endif

ifeq ($(UNAME),SunOS)
  CFLAGS+=-D__C99FEATURES__
endif

ifdef DEV
  OPTIMIZATIONS=
else
  INLINE_OPTS=-finline-limit=2000 --param max-inline-insns-single=3500 --param large-function-growth=2000 --param inline-unit-growth=1500
  FP_OPTS=-fno-math-errno -funsafe-math-optimizations -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fcx-limited-range
  OPTIMIZATIONS=-O2 -funroll-loops -finline-functions $(INLINE_OPTS)
endif

ifeq ($(CPU), powerpc)
  OPTIMIZATIONS+=-falign-loops=16
endif

CFLAGS += -fPIC $(CPPFLAGS)
DEPS=bstraux.h bstrlib.h

SOURCES=bstraux.c bstrlib.c
OBJS=$(SOURCES:.c=.o)
LOBJS=$(SOURCES:.c=.lo)

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

.PHONY: library

clean:
	rm -f *.o *.lo *.la *.so *.dylib *.a

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=.%.d)
endif
