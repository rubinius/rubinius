#ifndef RBX_CAPI_RUBY_THREAD_NATIVE_H
#define RBX_CAPI_RUBY_THREAD_NATIVE_H

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_t rb_nativethread_id_t;
typedef pthread_mutex_t rb_nativethread_lock_t;

#define rb_nativethread_self()          pthread_self()
#define rb_nativethread_lock_initialize(l)   pthread_mutex_init((l), 0)
#define rb_nativethread_lock_destroy(l)      pthread_mutex_destroy(l)
#define rb_nativethread_lock_lock(l)         pthread_mutex_lock(l)
#define rb_nativethread_lock_unlock(l)       pthread_mutex_unlock(l)

#ifdef __cplusplus
}
#endif

#endif
