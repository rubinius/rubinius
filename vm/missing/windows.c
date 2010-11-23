#ifdef RBX_WINDOWS

#include "windows_compat.h"
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

#endif
