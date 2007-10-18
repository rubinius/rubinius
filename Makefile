# NOTE! When updating this file, also update INSTALL, if necessary.

include shotgun/config.mk

all: vm

vm:
	cd shotgun; $(MAKE) rubinius

install:
	cd shotgun; $(MAKE) install
	mkdir -p $(RBAPATH)
	mkdir -p $(CODEPATH)
	cp runtime/*.rba runtime/*.rbc $(RBAPATH)/
	mkdir -p $(CODEPATH)/bin
	cp -r lib/* $(CODEPATH)/
	for rb in $(CODEPATH)/*.rb ; do $(BINPATH)/rbx compile $$rb; done	
	for rb in $(CODEPATH)/**/*.rb ; do $(BINPATH)/rbx compile $$rb; done	

clean:
	cd shotgun; $(MAKE) clean

.PHONY: all install clean
