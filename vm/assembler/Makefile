INCLUDES=-I../.. -I.. -I. -Iudis86-1.7

default: test

libudis86.a:
	(cd udis86-1.7; ./configure && make && cp libudis86/.libs/libudis86.a ../)

example/codegen: example/codegen.o libudis86.a
	g++ -ggdb3 -Wall $(INCLUDES) -o example/codegen libudis86.a example/codegen.o

example/codegen.o: codegen.cpp assembler_x86.hpp
	g++ -ggdb3 $(INCLUDES) -o example/codegen.o -c example/codegen.cpp

example/vm.o: example/vm.cpp assembler_x86.hpp
	g++ -ggdb3 -O2 $(INCLUDES) -o example/vm.o -c example/vm.cpp

example/vm: example/vm.o example/vm-main.cpp
	g++ -ggdb3 -O2 $(INCLUDES) -o example/vm-main.o -c example/vm-main.cpp
	g++ -ggdb3 -O2 -Wall $(INCLUDES) -o example/vm libudis86.a example/vm.o example/vm-main.o

test/test32: test/test32.o assembler_x86.o
	g++ -ggdb3 -Wall $(INCLUDES) -o test/test32 -ldl test/test32.o assembler_x86.o libudis86.a

test/test32.o: test/test32.cpp assembler_x86.hpp libudis86.a
	g++ -ggdb3 $(INCLUDES) -o test/test32.o -c test/test32.cpp

test/test64: test/test64.o
	g++ -ggdb3 -Wall $(INCLUDES) -o test/test64 -ldl test/test64.o libudis86.a

test/test64.o: test/test64.cpp assembler_x8664.hpp libudis86.a
	g++ -ggdb3 $(INCLUDES) -o test/test64.o -c test/test64.cpp

test/test_operations: test/test_operations.o assembler_x86.o
	g++ -ggdb3 -Wall $(INCLUDES) -o test/test_operations -ldl test/test_operations.o assembler_x86.o libudis86.a

test/test_operations.o: test/test_operations.cpp operations.hpp assembler_x86.hpp libudis86.a
	g++ -ggdb3 $(INCLUDES) -o test/test_operations.o -c test/test_operations.cpp

test: test/test32 test/test64 test/test_operations
	@echo "---- 32 bit" && ./test/test32 && echo "---- 64 bit" && ./test/test64
	@echo "---- operations" && ./test/test_operations

clean:
	rm -f *.o test/*.o test/test32 test/test64 example/*.o example/codegen example/vm

distclean: clean
	rm -f libudis86.a
	cd udis86-1.7; make distclean || true

%.o: %.cpp
	g++ -ggdb3 -Wall $(INCLUDES) -o $@ -c $<


.PHONY: test
