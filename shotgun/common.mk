# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)

ifdef VERBOSE
  COMP=$(LIBTOOL) --mode=compile $(CC)
  LINKER=$(LIBTOOL) --mode=link $(CC)
else
  COMP=@echo CC $@;$(LIBTOOL) --mode=compile $(CC) > /dev/null
  LINKER=@echo LINK $@;$(LIBTOOL) --mode=link $(CC) > /dev/null
endif
