# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)

WARNINGS = -Wall
DEBUG = -g -ggdb3

# basic optimizations should be overridable
CFLAGS ?= -O2

CFLAGS += $(WARNINGS) $(DEBUG) 

CPPFLAGS += `pkg-config glib-2.0 --cflags`

ifdef VERBOSE
  COMP=$(LIBTOOL) --mode=compile $(CC)
  LINKER=$(LIBTOOL) --mode=link $(CC)
else
  COMP=@echo CC $@;$(LIBTOOL) --mode=compile $(CC) > /dev/null
  LINKER=@echo LINK $@;$(LIBTOOL) --mode=link $(CC) > /dev/null
endif
