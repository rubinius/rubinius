# This Makefile is a stub to invoke the actual build system to conform to
# common expectations about building software by running:
#
#  configure && make && make install
#

.PHONY: depend build install clean

all: install

depend:
	@echo TODO: implement 'make depend'

build:
	@RBX_SUPRESS_DEPRECATION=1 rake -q build

install:
	@RBX_SUPRESS_DEPRECATION=1 rake -q install

clean:
	@RBX_SUPRESS_DEPRECATION=1 rake -q clean
