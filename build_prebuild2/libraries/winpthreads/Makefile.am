AM_CFLAGS = -Wall

lib_LIBRARIES = libpthread.a

libpthread_a_CPPFLAGS = -I$(srcdir)/include
libpthread_a_SOURCES = \
  src/barrier.h  src/cond.h  src/misc.h  src/mutex.h  src/rwlock.h  src/spinlock.h  src/thread.h  src/ref.h  src/sem.h \
  src/barrier.c  src/cond.c  src/misc.c  src/mutex.c  src/rwlock.c  src/spinlock.c  src/thread.c  src/ref.c  src/sem.c  src/sched.c

include_HEADERS = include/pthread.h include/semaphore.h

DISTCHECK_CONFIGURE_FLAGS = --host=$(host_triplet)

