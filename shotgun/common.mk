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

CPPFLAGS += `pkg-config glib-2.0 --cflags`

COMP=$(CC)
ifeq ($(UNAME),Darwin)
  LDOPT=-dynamiclib -undefined dynamic_lookup
  LINKER=$(CC) $(LDOPT)
  RPATH=-install_name 
  SONAME=-current_version 
  SUFFIX=dylib
else
  LDOPT=-shared
  LINKER=$(CC) -shared
  RPATH=-Wl,-rpath -Wl,
  SONAME=-Wl,-soname,
  SUFFIX=so
endif

ifndef VERBOSE
  COMP=@echo CC $@;$(CC)
  LINKER=@echo LINK $@;$(CC) $(LDOPT)
endif

RBXLIB=librubinius-$(VERSION).$(SUFFIX)
