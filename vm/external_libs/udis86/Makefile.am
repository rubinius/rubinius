SUBDIRS = libudis86 . udcli . tests . docs

include_ladir = ${includedir}
include_la_HEADERS = udis86.h

.PHONY: libudis86 udcli tests

libudis86:
	$(MAKE) -C $@

udcli: libudis86
	$(MAKE) -C $@

tests: check

maintainer-clean-local:
	rm -rf autom4te.cache aclocal.m4 
