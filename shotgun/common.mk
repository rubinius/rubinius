# Respect the environment
ifeq ($(CC),)
  CC=gcc
endif

### Platform stuff
UNAME=$(shell uname)
CPU=$(shell uname -p)
MARCH=$(shell uname -m)
OSVER=$(shell uname -r)

# amd64 is also known as x86_64
ifeq ($(MARCH),x86_64)
  MARCH=amd64
endif

WARNINGS = -Wall -Wwrite-strings
DEBUG = -g -ggdb3

CFLAGS = $(WARNINGS) $(DEBUG) 

COMP=$(CC)
ifeq ($(UNAME),Darwin)
  LDOPT=-dynamiclib -undefined dynamic_lookup
  LINKER=$(CC) $(LDOPT)
  SUFFIX=dylib
  BIN_RPATH=
  BIN_RPATHLOCAL=
  SONAME=-current_version $(VERSION) -compatibility_version $(VERSION) -install_name $(PREFIX)/lib/librubinius-$(VERSION).$(SUFFIX)
  SONAMELOCAL=-current_version $(VERSION) -compatibility_version $(VERSION) -install_name `pwd`/librubinius-local-dev.$(SUFFIX)
  ifeq ($(CPU),powerpc)
    ifeq ($(OSVER),9.1.0)
      MACOSX_DEPLOYMENT_TARGET=10.5
    else
      MACOSX_DEPLOYMENT_TARGET=10.4
    endif
  endif
else
  LDOPT=-shared
  LINKER=$(CC) -shared
  BIN_RPATH=-Wl,-rpath -Wl,/usr/local/lib
  BIN_RPATHLOCAL=-Wl,-rpath -Wl,`pwd`/lib
  SUFFIX=so
  SONAME=-Wl,-soname,librubinius-$(VERSION).$(SUFFIX)
  SONAMELOCAL=-Wl,-soname,librubinius-local-dev.$(SUFFIX)
endif

ifndef VERBOSE
  COMP=@echo CC $@;$(CC)
  LINKER=@echo LINK $@;$(CC) $(LDOPT)
endif

RBXLIB=librubinius-$(VERSION).$(SUFFIX)
RBXLIBLOCAL=librubinius-local-dev.$(SUFFIX)
