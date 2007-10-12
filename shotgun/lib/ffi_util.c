
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

#include "shotgun.h"
#include "symbol.h"
#include "object.h"
#include "tuple.h"
#include "hash.h"
#include "string.h"
#include "primitive_indexes.h"

#include <errno.h>

int ffi_errno() { return errno; }

OBJECT ffi_new_pointer(STATE, void *ptr) {
  void **code_start;
  OBJECT obj;
  
  NEW_STRUCT(obj, code_start, BASIC_CLASS(ffi_ptr), void*);
  *code_start = (void*)ptr;
  
  return obj;
}

int ffi_address(void *ptr) {
  return (int)ptr;
}

int ffi_write_int(int *ptr, int val) {
  *ptr = val;
  return val;
}

int ffi_read_int(int *ptr) {
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

char *ffi_sprintf_f(double value, int size, char *fmt) {
  char *str = calloc(size, sizeof(char));
  snprintf(str, size, fmt, value);
  return str;
}

char *ffi_sprintf_d(int value, int size, char *fmt) {
  char *str = calloc(size, sizeof(char));
  snprintf(str, size, fmt, value);
  return str;
}

OBJECT ffi_pack_sockaddr_un(STATE, char *path) {
  struct sockaddr_un *sa;
  
  sa = (struct sockaddr_un*)malloc(sizeof(struct sockaddr_un));
  memset(sa, 0, sizeof(struct sockaddr_un));
  
  strncpy(sa->sun_path, path, sizeof(sa->sun_path) - 1);
  
  return tuple_new2(state, 2, 
    ffi_new_pointer(state, (void*)sa), I2N(sizeof(struct sockaddr_un)));
}

OBJECT ffi_pack_sockaddr_in(STATE, char *name, char *port, int type, int flags) {
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
  
  error = getaddrinfo(name, port, &hints, &res);
  if(error) {
    printf("ERROR: %s\n", gai_strerror(error));
    return Qnil;
  }
  
  return tuple_new2(state, 2, ffi_new_pointer(state, 
    (void*)res->ai_addr), I2N(res->ai_addrlen));
}

OBJECT ffi_getpeername(STATE, int s, int reverse_lookup) {
  OBJECT host;
  OBJECT address;

  int error = 0;

  struct sockaddr_storage addr;
  socklen_t len = sizeof addr;

  char hbuf[1024];
  char pbuf[1024];

  error = getpeername(s, (struct sockaddr*)&addr, &len);
  if(error) {
    printf("ffi_getpeername ERROR: %s\n", gai_strerror(error));
    return Qnil;
  }

  error = getnameinfo((struct sockaddr*)&addr, len, hbuf, sizeof(hbuf), pbuf, sizeof(pbuf), NI_NUMERICHOST | NI_NUMERICSERV);
  if(error) {
    printf("ffi_getpeername ERROR: %s\n", gai_strerror(error));
    return Qnil;
  }
  address = string_new(state, hbuf);

  if(reverse_lookup) {
    error = getnameinfo((struct sockaddr*)&addr, len, hbuf, sizeof(hbuf), NULL, 0, 0);
    if(error) {
      printf("ffi_getpeername ERROR: %s\n", gai_strerror(error));
      return Qnil;
    }
  }

  host = string_new(state, hbuf);

  return tuple_new2(state, 2, host, address);
}

/*
int ffi_bind(int s, struct sockaddr *name, socklen_t len) {
  int ret;
  struct stat sb;
  ret = fstat(s, &sb);
  assert(sb.st_mode & S_IFSOCK && "trying to bind something that isn't a socket!");
  ret = bind(s, name, len);
  return ret;
}

int ffi_bind_local_socket(int s) {
  struct addrinfo hints;
  struct addrinfo *res = NULL;
  int error;
  int ret;
  
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  
  error = getaddrinfo(0, 0, &hints, &res);
  if(error) {
    printf("bind_local_socket ERROR: %s\n", gai_strerror(error));
    return Qnil;
  }
  ret = bind(s, res->ai_addr, res->ai_addrlen);
  return ret;
}
<<<<<<< HEAD:shotgun/lib/ffi_util.c
*/

void *ffi_add_ptr(char *ptr, int offset) { 
  return (void*)(ptr + offset); 
}
