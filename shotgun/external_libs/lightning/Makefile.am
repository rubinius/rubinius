# Automake requirements:
AUTOMAKE_OPTIONS = 1.8 gnu
ACLOCAL_AMFLAGS = -I build-aux

SUBDIRS = . doc lightning opcode tests
EXTRA_DIST = build-aux/texi2dvi build-aux/help2man

include_HEADERS = lightning.h

aclocaldir = $(datadir)/aclocal
dist_aclocal_DATA = build-aux/lightning.m4
bin_SCRIPTS = lightningize
