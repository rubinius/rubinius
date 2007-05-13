TEXI2DVI=$(top_srcdir)/build-aux/texi2dvi
HELP2MAN = $(top_srcdir)/build-aux/help2man

dist_man1_MANS = lightningize.1
info_TEXINFOS = lightning.texi 
EXTRA_TEXINFOS = u-lightning.texi p-lightning.texi
MOSTLYCLEANFILES = lightning.tmp

lightning_TEXINFOS = body.texi toc.texi using.texi porting.texi version.texi
u_lightning_TEXINFOS = body.texi toc.texi using.texi version.texi
p_lightning_TEXINFOS = body.texi toc.texi porting.texi version.texi

lightningize.1: $(top_srcdir)/lightningize.in $(top_srcdir)/configure.ac
	cd $(top_builddir) && $(MAKE) lightningize
	$(HELP2MAN) -p lightning $(top_builddir)/lightningize > $(srcdir)/lightningize.1
 
