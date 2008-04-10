TESTS=$(wildcard test_*.hpp)
IMP=$(wildcard ../*.cpp) $(wildcard ../*.hpp)

EX_INC=-I../external_libs/libtommath -I../external_libs/onig -I../external_libs/libffi/include -I../external_libs/libltdl
EX_LIBS=../external_libs/libtommath/libtommath.a ../external_libs/onig/.libs/libonig.a ../external_libs/libffi/.libs/libffi.a ../external_libs/libltdl/.libs/libltdl.a

external_libs:
	cd ../; ln -sf ../shotgun/external_libs external_libs

runner.cpp: external_libs $(TESTS) $(IMP)
	cxxtest/cxxtestgen.pl --error-printer --have-eh --abort-on-fail -o runner.cpp $(TESTS)

runner: runner.cpp
	g++ -Wall -Icxxtest -I../ $(EX_INC) -ggdb -o runner runner.cpp ../*.cpp $(EX_LIBS)

coverage/runner: runner.cpp
	@mkdir -p coverage
	g++ -fprofile-arcs -ftest-coverage -Wall -Icxxtest -I../ $(EX_INC) -ggdb -o coverage/runner runner.cpp ../*.cpp $(EX_LIBS)

.PHONY: coverage/runner

clean:
	rm -rf *.gcda *.gcno *.gcov *.dSYM runner coverage

test: runner
	@./runner

coverage: coverage/runner
	@./coverage/runner
	@lcov/bin/lcov --directory . --capture --output-file coverage/app.info
	@cd coverage; ../lcov/bin/genhtml app.info
	@rm -f *.gcno *.gcda

show-coverage: coverage
	@open coverage/index.html
