thread.o: thread.c
	gcc  -m64 -c thread.c

mutex.o: mutex.c
	gcc -c  -m64 mutex.c

cond.o: cond.c
	gcc -c  -m64 cond.c

rwlock.o: rwlock.c
	gcc -c  -m64 rwlock.c

barrier.o: barrier.c
	gcc -c  -m64 barrier.c

spinlock.o: spinlock.c
	gcc -c  -m64 spinlock.c

misc.o: misc.c
	gcc -c  -m64 misc.c

libpthread.a: thread.o mutex.o cond.o rwlock.o barrier.o spinlock.o misc.o
	ar rcs libpthread.a  misc.o thread.o mutex.o cond.o rwlock.o barrier.o spinlock.o

lib: libpthread.a	

tests: lib
	cp libpthread.a pthread.h tests
	
clean:
	rm -rf *.o *.a


