#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#include "ffi_util.h"

int ffi_errno() { return errno; }

void ffi_set_errno(int n) {
  errno = n;
}

time_t ffi_timezone() {
  return timezone;
}

char* ffi_tzname(int dst) {
  if(dst) {
    return tzname[1];
  } else {
    return tzname[0];
  }
}

/*
int ffi_daylight() {
  return daylight;
}
*/

uintptr_t ffi_address(void *ptr) {
  return (uintptr_t)ptr;
}

int ffi_write_int(int *ptr, int val) {
  *ptr = val;
  return val;
}

int ffi_read_int(int *ptr) {
  return *ptr;
}

long ffi_write_long(long *ptr, long val) {
  *ptr = val;
  return val;
}

long ffi_read_long(long *ptr) {
  return *ptr;
}

double ffi_write_float(double *ptr, double val) {
  *ptr = val;
  return val;
}

double ffi_read_float(double *ptr) {
  return *ptr;
}

char *ffi_read_string(char *ptr) {
  return ptr;
}

void *ffi_read_pointer(void **ptr) {
  return *ptr;
}

void *ffi_add_ptr(char *ptr, int offset) { 
  return (void*)(ptr + offset); 
}

int ffi_type_size(int type) {
  switch(type) {
    case RBX_FFI_TYPE_CHAR:
    case RBX_FFI_TYPE_UCHAR:
    return sizeof(char);

    case RBX_FFI_TYPE_SHORT:
    case RBX_FFI_TYPE_USHORT:
    return sizeof(short);

    case RBX_FFI_TYPE_INT:
    case RBX_FFI_TYPE_UINT:
    return sizeof(int);

    case RBX_FFI_TYPE_LONG:
    case RBX_FFI_TYPE_ULONG:
    return sizeof(long);

    case RBX_FFI_TYPE_LONG_LONG:
    case RBX_FFI_TYPE_ULONG_LONG:
    return sizeof(long long);

    case RBX_FFI_TYPE_FLOAT:
    return sizeof(float);

    case RBX_FFI_TYPE_DOUBLE:
    return sizeof(double);

    case RBX_FFI_TYPE_PTR:
    case RBX_FFI_TYPE_STRING:
    case RBX_FFI_TYPE_STATE:
    case RBX_FFI_TYPE_STRPTR:
    case RBX_FFI_TYPE_OBJECT:
    return sizeof(void*);

    default:

    return -1;
  }
}

unsigned int ffi_cast(unsigned int val) {
  return val;
}

long ffi_major(dev_t n) {
#if defined(major)
  return major(n);
#else
  return -1;
#endif
}

long ffi_minor(dev_t n) {
#if defined(minor)
  return minor(n);
#else
  return -1;
#endif
}

/* FIXME: these are TEMPORARY until we determine how to
 * have FFI resolve symbols that may be macros. This is
 * used rather than a primitive so that it is easier to
 * replace (unlike primitives).
 */
int ffi_stat(const char *path, struct stat *buf) {
  return stat(path, buf);
}

int ffi_fstat(int filedes, struct stat *buf) {
  return fstat(filedes, buf);
}

int ffi_lstat(const char *path, struct stat *buf) {
  return lstat(path, buf);
}
