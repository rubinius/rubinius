#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/primitive_indexes.h"

int ffi_errno() { return errno; }

void ffi_set_errno(int n) {
  errno = n;
}

long ffi_timezone() {
  return timezone;
}

char* ffi_tzname(int dst) {
  if(dst) {
    return tzname[1];
  } else {
    return tzname[0];
  }
}

int ffi_daylight() {
  return daylight;
}

OBJECT ffi_new_pointer(STATE, void *ptr) {
  void **code_start;
  OBJECT obj;
  
  NEW_STRUCT(obj, code_start, BASIC_CLASS(ffi_ptr), void*);
  *code_start = (void*)ptr;
  
  return obj;
}

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

OBJECT ffi_read_string_length(STATE, void *ptr, int len) {
  OBJECT str = string_new2(state, NULL, len);
  
  memcpy(string_byte_address(state, str), ptr, len);
  
  return str;
}

int ffi_write_string_length(STATE, void *ptr, void* str, int len) {
  void *src = string_byte_address(state, str);
  
  memcpy(ptr, src, len);
  
  return len;
}

void *ffi_read_pointer(void **ptr) {
  return *ptr;
}

char *ffi_sprintf_f(double value, int size, char *fmt) {
  char *str = ALLOC_N(char, size);
  snprintf(str, size, fmt, value);
  return str;
}

char *ffi_sprintf_d(int value, int size, char *fmt) {
  char *str = ALLOC_N(char, size);  
  snprintf(str, size, fmt, value);
  return str;
}

OBJECT ffi_pack_sockaddr_un(STATE, char *path) {
  struct sockaddr_un *sa;
  
  sa = ALLOC(struct sockaddr_un);
  memset(sa, 0, sizeof(struct sockaddr_un));
  
  strncpy(sa->sun_path, path, sizeof(sa->sun_path) - 1);

  return string_new2(state, (char*) sa, sizeof(struct sockaddr_un) );
}

OBJECT ffi_pack_sockaddr_in(STATE, char *name, char *port, int type, int flags) {
  OBJECT ret;
  struct addrinfo hints;
  struct addrinfo *res = NULL;
  int error;

  if (type == 0 && flags == 0) {
    type = SOCK_DGRAM;
  }

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = type;
  hints.ai_flags = flags;

  if(strlen(name) == 0) {
    name = NULL;
  }

  error = getaddrinfo(name, port, &hints, &res);
  if(error) {
    return tuple_new2(state, 2, Qfalse, string_new(state, gai_strerror(error)));
  }

  ret = tuple_new2(state, 2, Qtrue,
                   string_new2(state, (char *) res->ai_addr, res->ai_addrlen));

  freeaddrinfo(res);

  return ret;
}

OBJECT ffi_decode_sockaddr(STATE, struct sockaddr *addr, int len, int reverse_lookup) {
  OBJECT host;
  OBJECT address;

  int error = 0;

  char hbuf[NI_MAXHOST];
  char pbuf[NI_MAXSERV];

  error = getnameinfo(addr, len, hbuf, sizeof(hbuf), pbuf, sizeof(pbuf),
                      NI_NUMERICHOST | NI_NUMERICSERV);
  if(error) {
    return string_new(state, gai_strerror(error));
  }

  address = string_new(state, hbuf);

  if(reverse_lookup) {
    error = getnameinfo(addr, len, hbuf, sizeof(hbuf), NULL, 0, 0);

    if(error) {
      return string_new(state, gai_strerror(error));
    }
  }

  host = string_new(state, hbuf);

  return tuple_new2(state, 3, host, address, I2N(atoi(pbuf)));
}

OBJECT ffi_getpeername(STATE, int s) {
  int error = 0;

  struct sockaddr_storage addr;
  socklen_t len = sizeof addr;

  error = getpeername(s, (struct sockaddr*)&addr, &len);

  if(error) {
    return tuple_new2(state, 2, Qfalse, string_new(state, gai_strerror(error)));
  }

  return tuple_new2(state, 2, Qtrue, string_new2(state, (char *)&addr, len));
}

OBJECT ffi_getsockname(STATE, int s) {
  int error = 0;

  struct sockaddr_storage addr;
  socklen_t len = sizeof addr;

  error = getsockname(s, (struct sockaddr*)&addr, &len);

  if(error) {
    return tuple_new2(state, 2, Qfalse, string_new(state, gai_strerror(error)));
  }

  return tuple_new2(state, 2, Qtrue, string_new2(state, (char *)&addr, len));
}

int ffi_bind(int s, char *host, char *port, int type) {
  struct addrinfo hints;
  struct addrinfo *res = NULL;
  int error;
  int ret;
  
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = 0;

  if(strlen(host) == 0) host = NULL;
  
  error = getaddrinfo(NULL, "40011", &hints, &res);
  if(error) {
    printf("bind_local_socket ERROR: %s\n", gai_strerror(error));
    return -1;
  }
  ret = bind(s, res->ai_addr, res->ai_addrlen);
  return ret;
}

OBJECT ffi_getnameinfo(STATE, struct sockaddr *sockaddr, int sockaddr_len,
                       int flags) {
  char node[NI_MAXHOST], service[NI_MAXSERV];
  OBJECT value, address, port;
  int err;

  err = getnameinfo(sockaddr, sockaddr_len, node, NI_MAXHOST,
                    service, NI_MAXSERV, flags);

  if(err != 0) {
    return tuple_new2(state, 2, Qfalse,
        string_new(state, gai_strerror(err)));
  }

  address = string_new2(state, node, strlen(node));
  port = I2N(atoi(service));

  value = array_new(state, 0);
  array_append(state, value, I2N(sockaddr->sa_family));
  array_append(state, value, port);
  array_append(state, value, address);

  return tuple_new2(state, 2, Qtrue, value);
}

void *ffi_add_ptr(char *ptr, int offset) { 
  return (void*)(ptr + offset); 
}

/* FIXME: Kill these after the next rebuild of the stable RBAs */
int ffi_seek_set() { return SEEK_SET; }
int ffi_seek_cur() { return SEEK_CUR; }
int ffi_seek_end() { return SEEK_END; }
