
include shotgun/config.mk

all:
	cd shotgun; $(MAKE) rubinius

install:
	cd shotgun; $(MAKE) install
	mkdir -p $(RBAPATH)
	mkdir -p $(CODEPATH)
	cp runtime/*.rba $(RBAPATH)/
	cp -r lib/bin $(CODEPATH)/

clean:
	cd shotgun; $(MAKE) clean

.PHONY: all install clean
