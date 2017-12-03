#define _WIN32_WINNT 0x400

#include <inttypes.h>
#include "test.h"

int
main()
{
  printf("Sizes of winpthreads structs\n");
  printf("-------------------------------\n");
  printf("%30s %4" PRIdPTR "\n", "pthread_t", sizeof(pthread_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_attr_t_", sizeof(pthread_attr_t));
  /*  printf("%30s %4" PRIdPTR "\n", "sem_t_", sizeof(struct sem_t_)); */
  printf("%30s %4" PRIdPTR "\n", "pthread_mutex_t_", sizeof(mutex_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_mutexattr_t_", sizeof(pthread_mutexattr_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_spinlock_t_", sizeof(spin_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_barrier_t_", sizeof(barrier_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_barrierattr_t_", sizeof(pthread_barrierattr_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_key_t_", sizeof(pthread_key_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_cond_t_", sizeof(cond_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_condattr_t_", sizeof(pthread_condattr_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_rwlock_t_", sizeof(rwlock_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_rwlockattr_t_", sizeof(pthread_rwlockattr_t));
  printf("%30s %4" PRIdPTR "\n", "pthread_once_t_", sizeof(pthread_once_t));
  /* printf("%30s %4" PRIdPTR "\n", "ptw32_cleanup_t", sizeof(struct ptw32_cleanup_t)); */
  /* printf("%30s %4" PRIdPTR "\n", "ptw32_mcs_node_t_", sizeof(struct ptw32_mcs_node_t_)); */
  printf("%30s %4" PRIdPTR "\n", "sched_param", sizeof(struct sched_param));
  printf("-------------------------------\n");

  return 0;
}
