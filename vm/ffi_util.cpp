#include "config.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef RBX_WINDOWS
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#include "windows_compat.h"
#include "ffi_util.hpp"

extern "C" {

extern char** environ;

int ffi_errno() { return errno; }

char** ffi_environ() { return environ; }

void ffi_set_errno(int n) {
  errno = n;
}

#ifndef major
#define major(x) x
#endif

#ifndef minor
#define minor(x) 0
#endif

long ffi_major(dev_t n) {
  return major(n);
}

long ffi_minor(dev_t n) {
  return minor(n);
}

}
