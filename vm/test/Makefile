TESTS=$(wildcard test_*.hpp)
IMP=$(wildcard ../*.cpp) $(wildcard ../*.hpp)

runner.cpp: $(TESTS) $(IMP)
	@cxxtest/cxxtestgen.pl --error-printer --have-eh --abort-on-fail -o runner.cpp $(TESTS)

runner: runner.cpp
	@g++ -Wall -Icxxtest -I../ -ggdb -o runner runner.cpp ../*.cpp

coverage/runner: runner.cpp
	@mkdir -p coverage
	@g++ -fprofile-arcs -ftest-coverage -Wall -Icxxtest -I../ -ggdb -o coverage/runner runner.cpp ../*.cpp

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
