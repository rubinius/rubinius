test.o: test.c
	gcc -c -m64 -I ../include -I ../src test.c

test.exe: test.o
	gcc -static  -m64 -o test.exe test.o -L.. -lpthread

%.o: %.c
	gcc -c -m64 -I ../include -I ../src $<

%.exe: %.o
	gcc -static  -m64 -o $@ $< -L.. -lpthread


test: test.exe
	
clean:
	rm -rf *.o *.exe
