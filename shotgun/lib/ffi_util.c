
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h> //remove me
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

int ffi_read_int(int *ptr, int val) {
  return *ptr;
}

double ffi_write_float(double *ptr, double val) {
  *ptr = val;
  return val;
}

double ffi_read_float(double *ptr) {
  return *ptr;
}

char *ffi_sprintf_f(double value, int size, char *fmt) {
  char *str;
  str = malloc(size);
  snprintf(str, size, fmt, value);
  return str;
}

char *ffi_sprintf_d(int value, int size, char *fmt) {
  char *str = malloc(size);
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

int ffi_connect(int fd, struct sockaddr *name, int len) {
  int ret;
  
  ret = connect(fd, name, len);
  
  return ret;
}

int ffi_bind(int s, struct sockaddr *name, socklen_t len) {
  int ret;
  struct stat sb;
  ret = fstat(s, &sb);
  assert(sb.st_mode & S_IFSOCK && "trying to bind something that isn't a socket!");
  ret = bind(s, name, len);
  return ret;
}

int ffi_accept(int s, struct sockaddr *name, socklen_t *len) {
  int ret;
  
  ret = accept(s, name, len);
  
  return ret;
}
