SHARED = lib.$(DYNEXT)

all: $(SHARED) runtests

clean:
	rm -f lib.o $(SHARED) runtests

lib.o: lib.c
	gcc -g -c `sh ../tools/dyncflags.sh` lib.c -o lib.o -Wall -Werror

$(SHARED): lib.o
	sh ../tools/buildshared.sh $(SHARED) lib.o

runtests: runtests.c $(SHARED) ../lib/libcinvoke.a
	gcc -g -DCINVOKE_BUILD $(BUILDARCH) -o runtests runtests.c -Wall -Werror -L../lib -I../lib `sh ../tools/libdl.sh` -lcinvoke -lm
