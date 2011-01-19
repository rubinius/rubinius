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

// In MRI, this only sets properties on sockets, which requires
// tracking all fd's that are sockets...
int fcntl(int fildes, int cmd, ...) {

  if(cmd != F_SETFL) {
    errno = EINVAL;
    return -1;
  }

  va_list va;
  va_start(va, cmd);
  int arg = va_arg(va, int);
  va_end(va);

  unsigned long ioctl_arg;
  if(arg & O_NONBLOCK) {
    ioctl_arg = 1;
  }
  else {
    ioctl_arg = 0;
  }

	int ret = ioctlsocket(fd, FIONBIO, &ioctl_arg);
	if(ret) errno = map_errno(WSAGetLastError());

  return ret;
}

#endif
