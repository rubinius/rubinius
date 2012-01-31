## Makefile.am for Oniguruma
encdir    = $(top_srcdir)/enc
sampledir = $(top_srcdir)/sample
libname = libonig.a

#AM_CFLAGS = -DNOT_RUBY
AM_CFLAGS =
INCLUDES  = -I$(top_srcdir) -I$(includedir)

SUBDIRS = .

include_HEADERS = oniguruma.h

lib_LIBRARIES = $(libname)

libonig_a_SOURCES = regint.h regparse.h regenc.h st.h \
	regcomp.c regenc.c regerror.c regexec.c regparse.c \
	regsyntax.c regtrav.c regversion.c st.c \
	$(encdir)/ascii.c $(encdir)/big5.c $(encdir)/cp949.c \
	$(encdir)/emacs_mule.c \
	$(encdir)/euc_jp.c $(encdir)/euc_kr.c $(encdir)/euc_tw.c \
	$(encdir)/gb18030.c $(encdir)/gbk.c \
	$(encdir)/iso_8859_1.c $(encdir)/iso_8859_2.c \
	$(encdir)/iso_8859_3.c $(encdir)/iso_8859_4.c \
	$(encdir)/iso_8859_5.c $(encdir)/iso_8859_6.c \
	$(encdir)/iso_8859_7.c $(encdir)/iso_8859_8.c \
	$(encdir)/iso_8859_9.c $(encdir)/iso_8859_10.c \
	$(encdir)/iso_8859_11.c $(encdir)/iso_8859_13.c \
	$(encdir)/iso_8859_14.c $(encdir)/iso_8859_15.c \
	$(encdir)/iso_8859_16.c \
	$(encdir)/koi8_r.c $(encdir)/koi8_u.c \
	$(encdir)/shift_jis.c \
	$(encdir)/unicode.c \
	$(encdir)/us_ascii.c \
	$(encdir)/utf_16be.c $(encdir)/utf_16le.c \
	$(encdir)/utf_32be.c $(encdir)/utf_32le.c \
	$(encdir)/utf_8.c \
	$(encdir)/windows_1251.c

EXTRA_DIST = HISTORY README.ja index.html index_ja.html \
	doc/API doc/API.ja doc/RE doc/RE.ja doc/FAQ doc/FAQ.ja \
	win32/Makefile win32/config.h win32/testc.c \
	$(encdir)/koi8.c $(encdir)/mktable.c \
	$(sampledir)/encode.c $(sampledir)/listcap.c $(sampledir)/names.c \
	$(sampledir)/posix.c $(sampledir)/simple.c $(sampledir)/sql.c \
	$(sampledir)/syntax.c

bin_SCRIPTS = onig-config

onig-config: onig-config.in

dll:
	$(CXX) -shared -Wl,--output-def,libonig.def -o libonig.dll *.o \
	$(LIBS)
	strip libonig.dll

# Ruby TEST
rtest:
	$(RUBYDIR)/ruby -w -Ke $(srcdir)/test.rb

# character-types-table source generator
mktable: $(encdir)/mktable.c $(srcdir)/regenc.h
	$(CC) -I$(top_srcdir) -o mktable $(encdir)/mktable.c


# TEST
# TESTS = testc testp testcu
# 
# check_PROGRAMS = testc testp testcu
# 
# atest: testc testp testcu
# 	@echo "[Oniguruma API, ASCII/EUC-JP check]"
# 	@$(top_builddir)/testc  | grep RESULT
# 	@echo "[POSIX API, ASCII/EUC-JP check]"
# 	@$(top_builddir)/testp  | grep RESULT
# 	@echo "[Oniguruma API, UTF-16 check]"
# 	@$(top_builddir)/testcu | grep RESULT
# 
# testc_SOURCES = testc.c
# testc_LDADD = $(libname)
# 
# testp_SOURCES = testc.c
# testp_LDADD = $(libname)
# testp_CFLAGS = -DPOSIX_TEST
# 
# testcu_SOURCES = testu.c
# testcu_LDADD = $(libname)


#testc.c: $(srcdir)/test.rb $(srcdir)/testconv.rb
#	ruby -Ke $(srcdir)/testconv.rb < $(srcdir)/test.rb > $@

#testu.c: $(srcdir)/test.rb $(srcdir)/testconvu.rb
#	ruby -Ke $(srcdir)/testconvu.rb $(srcdir)/test.rb > $@

#win32/testc.c: $(srcdir)/test.rb $(srcdir)/testconv.rb
#	ruby -Ke $(srcdir)/testconv.rb -win < $(srcdir)/test.rb | nkf -cs > $@

## END OF FILE
