DISTCLEANFILES = asm.h core.h funcs.h fp.h

EXTRA_DIST = i386/Makefile.frag \
        i386/asm-32.h i386/asm-64.h i386/asm-i386.h \
        i386/core-32.h i386/core-64.h i386/core-i386.h \
        i386/fp-32.h i386/fp-64.h \
        i386/funcs.h \
        sparc/asm.h sparc/core.h sparc/funcs.h sparc/fp.h \
        ppc/asm.h ppc/core.h ppc/funcs.h ppc/fp.h

@lightning_frag@

LIGHTNING_COMMON_FILES = funcs-common.h core-common.h fp-common.h asm-common.h

if LIGHTNING_MAIN

lightningdir = $(includedir)/lightning
dist_pkgdata_DATA = Makefile.am
dist_lightning_HEADERS = $(LIGHTNING_COMMON_FILES)
nodist_lightning_HEADERS = asm.h core.h funcs.h fp.h $(LIGHTNING_TARGET_FILES)

else
all-am:
	@set frob $(LIGHTNING_TARGET_FILES); shift; \
	for i; \
	do \
	  echo $(LN_S) -f $(srcdir)/$$i `basename $$i`; \
	  $(LN_S) -f $(srcdir)/$$i `basename $$i`; \
	done

clean-local:
	@set frob $(LIGHTNING_TARGET_FILES); shift; \
	for i; \
	do \
	  echo rm -f `basename $$i`; \
	  rm -f `basename $$i`; \
	done

dist-hook:
	cp -p $(srcdir)/lightning.h $(distdir)
endif
