#include <stdint.h>
#include <sys/resource.h>

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
#include <mach/mach.h>
#endif

int thread_cpu_usage(uint64_t* user, uint64_t* system) {
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
  mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
  thread_basic_info_data_t info;
  kern_return_t err;

  mach_port_t port = mach_thread_self();
  err = thread_info(port, THREAD_BASIC_INFO, (thread_info_t)&info, &count);
  mach_port_deallocate(mach_task_self(), port);

  if(err == KERN_SUCCESS) {
    *user   = (uint64_t) info.user_time.seconds * 1000000 + info.user_time.microseconds;
    *system = (uint64_t) info.system_time.seconds * 1000000 + info.system_time.microseconds;
    return 0;
  }
  return -1;
#elif defined(RUSAGE_THREAD)
  struct rusage buf;
  if(getrusage(RUSAGE_THREAD, &buf)) return -1;
  *user   = (uint64_t)buf.ru_utime.tv_sec * 1000000 + buf.ru_utime.tv_usec;
  *system = (uint64_t)buf.ru_stime.tv_sec * 1000000 + buf.ru_stime.tv_usec;
  return 0;
#elif defined(_WIN32)
  FILETIME unused, unused2;
  FILETIME sys, usr;
  GetThreadTimes(GetCurrentThread(), &unused, &unused2, &sys, &user);
  *user   = (((uint64_t) usr.dwHighDateTime) << 32) + usr.dwLowDateTime;
  *system = (((uint64_t) sys.dwHighDateTime) << 32) + sys.dwLowDateTime;
  return 0;
#else
  return -1;
#endif
}
