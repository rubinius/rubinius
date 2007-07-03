# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)

LINKER=$(LIBTOOL) --mode=link $(CC)

ifdef VERBOSE
  COMP=$(LIBTOOL) --mode=compile $(CC)
else
  COMP=@echo CC $@;$(LIBTOOL) --mode=compile $(CC) > /dev/null
endif
