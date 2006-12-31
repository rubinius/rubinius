noinst_PROGRAMS= \
	add_from_buffer \
	add_from_file \
	add_from_filep \
	encrypt \
	fread \
	get_comment \
	open \
	name_locate \
	set_comment_all \
	set_comment_localonly \
	set_comment_removeglobal \
	set_comment_revert

EXTRA_PROGRAMS=deltest ziptest

EXTRA_DIST= \
	runtest \
	${TESTCASES} \
	testbuffer.zip \
	testchanged.zip testchangedlocal.zip \
	testcomment.zip testcommentremoved.zip \
	testfile.txt testfile.zip \
	broken.zip test.zip encrypt.zip

TESTS_ENVIRONMENT=	ZIPCMP=../src/zipcmp

TESTCASES= \
	add_from_buffer.test \
	add_from_file.test \
	add_from_filep.test \
	encrypt.test \
	fread.test \
	get_comment.test \
	name_locate.test \
	open.test \
	set_comment_all.test \
	set_comment_localonly.test \
	set_comment_removeglobal.test \
	set_comment_revert.test

check:
	@failed=0; all=0; \
	srcdir=$(srcdir); export srcdir; \
	for tst in ${TESTCASES}; do \
	  t=`basename $$tst .test`; \
	  if ${TESTS_ENVIRONMENT} $(srcdir)/runtest $$t; then \
	    all=`expr $$all + 1`; \
	    echo "PASS: $$tst"; \
	  elif test $$? -ne 77; then \
	    all=`expr $$all + 1`; \
	    failed=`expr $$failed + 1`; \
	    echo "FAIL: $$tst"; \
	  fi; \
	done; \
	if test "$$failed" -eq 0; then \
	  banner="All $$all tests passed"; \
	else \
	  banner="$$failed of $$all tests failed"; \
	fi; \
	dashes=`echo "$$banner" | sed s/./=/g`; \
	echo "$$dashes"; \
	echo "$$banner"; \
	echo "$$dashes"; \
	test "$$failed" -eq 0

AM_CPPFLAGS=-I${top_srcdir}/lib
LDADD=../lib/libzip.la
