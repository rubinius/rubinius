# NOTE! When updating this file, also update INSTALL, if necessary.

-include shotgun/config.mk

all: vm

vm:
	cd shotgun; $(MAKE) rubinius

install:
	rake install

clean:
	cd shotgun; $(MAKE) clean

.PHONY: all install clean
