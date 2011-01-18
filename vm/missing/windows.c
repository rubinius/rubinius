#ifdef RBX_WINDOWS

#include "windows_compat.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>

int uname(struct utsname *name) {
  rubinius::abort();
  return -1
}

int socketpair(int domain, int type, int protocol, int socket_vector[2]) {
  rubinius::abort();
  return 0;
}

int pipe(int fildes[2]) {
  return _pipe(fildes, 512, _O_BINARY);
}

char* realpath(const char* file_name, char* resolved_name) {
  return _fullpath(file_name, resolved_name, MAX_PATH);
}

int setenv(const char *name, const char *value, int overwrite) {
  if(getenv(name) && !overwrite) return 0;

  int size = 2 + strlen(name) + strlen(value);
  char* str = malloc(size);
  if(!str) return -1;

  snprintf(str, size, "%s=%s", name, value);
  return putenv(str);
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
  return 0;
}

#define RBX_USEC_PER_SEC   1000000

void timeradd(struct timeval *a, struct timeval *b, struct timeval *res) {
  res->tv_sec = a->tv_sec + b->tv_sec;
  res->tv_usec = a->tv_usec + b->tv_usec;

  if(res->tv_usec >= RBX_USEC_PER_SEC) {
    res->tv_sec++;
    res->tv_usec -= RBX_USEC_PER_SEC;
  }
}

void timersub(struct timeval *a, struct timeval *b, struct timeval *res) {
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;

  if(res->tv_usec < 0) {
    res->tv_sec--;
    res->tv_usec += RBX_USEC_PER_SEC;
  }
}

#endif
