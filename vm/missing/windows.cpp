#include "config.h"

#ifdef RBX_WINDOWS

#include "exception.hpp"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <io.h>

#include "windows_compat.h"

int uname(struct utsname *name) {
  strncpy(name->sysname, "Windows", UNAME_NAMELEN);
  return 0;
}

int socketpair(int domain, int type, int protocol, int socket_vector[2]) {
  rubinius::abort();
  return 0;
}

int pipe(int fildes[2]) {
  return _pipe(fildes, 512, _O_BINARY);
}

char* realpath(char* file_name, char* resolved_name) {
  return _fullpath(file_name, resolved_name, MAX_PATH);
}

int setenv(const char *name, const char *value, int overwrite) {
  if(getenv(name) && !overwrite) return 0;

  int size = 2 + strlen(name) + strlen(value);
  char* str = (char*)malloc(size);
  if(!str) return -1;

  snprintf(str, size, "%s=%s", name, value);
  return putenv(str);
}

int unsetenv(const char *name) {
  return setenv(name, "", 1);
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
  return 0;
}

time_t timegm (struct tm *tm) {
  char* tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();

  time_t ret = mktime(tm);

  if(tz) {
    setenv("TZ", tz, 1);
  } else {
    unsetenv("TZ");
  }
  tzset();

  return ret;
}


// In MRI, this only sets properties on sockets, which requires
// tracking all fd's that are sockets...
int fcntl(int fd, int cmd, ...) {

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

/* errno mapping, adapted from MRI. */
static struct {
  DWORD winerr;
  int err;
} error_map[] = {
  { ERROR_INVALID_FUNCTION,           EINVAL    },
  { ERROR_FILE_NOT_FOUND,             ENOENT    },
  { ERROR_PATH_NOT_FOUND,             ENOENT    },
  { ERROR_TOO_MANY_OPEN_FILES,        EMFILE    },
  { ERROR_ACCESS_DENIED,              EACCES    },
  { ERROR_INVALID_HANDLE,             EBADF     },
  { ERROR_ARENA_TRASHED,              ENOMEM    },
  { ERROR_NOT_ENOUGH_MEMORY,          ENOMEM    },
  { ERROR_INVALID_BLOCK,              ENOMEM    },
  { ERROR_BAD_ENVIRONMENT,            E2BIG     },
  { ERROR_BAD_FORMAT,                 ENOEXEC   },
  { ERROR_INVALID_ACCESS,             EINVAL    },
  { ERROR_INVALID_DATA,               EINVAL    },
  { ERROR_INVALID_DRIVE,              ENOENT    },
  { ERROR_CURRENT_DIRECTORY,          EACCES    },
  { ERROR_NOT_SAME_DEVICE,            EXDEV     },
  { ERROR_NO_MORE_FILES,              ENOENT    },
  { ERROR_WRITE_PROTECT,              EROFS     },
  { ERROR_BAD_UNIT,                   ENODEV    },
  { ERROR_NOT_READY,                  ENXIO     },
  { ERROR_BAD_COMMAND,                EACCES    },
  { ERROR_CRC,                        EACCES    },
  { ERROR_BAD_LENGTH,                 EACCES    },
  { ERROR_SEEK,                       EIO       },
  { ERROR_NOT_DOS_DISK,               EACCES    },
  { ERROR_SECTOR_NOT_FOUND,           EACCES    },
  { ERROR_OUT_OF_PAPER,               EACCES    },
  { ERROR_WRITE_FAULT,                EIO       },
  { ERROR_READ_FAULT,                 EIO       },
  { ERROR_GEN_FAILURE,                EACCES    },
  { ERROR_LOCK_VIOLATION,             EACCES    },
  { ERROR_SHARING_VIOLATION,          EACCES    },
  { ERROR_WRONG_DISK,                 EACCES    },
  { ERROR_SHARING_BUFFER_EXCEEDED,    EACCES    },
  { ERROR_BAD_NETPATH,                ENOENT    },
  { ERROR_NETWORK_ACCESS_DENIED,      EACCES    },
  { ERROR_BAD_NET_NAME,               ENOENT    },
  { ERROR_FILE_EXISTS,                EEXIST    },
  { ERROR_CANNOT_MAKE,                EACCES    },
  { ERROR_FAIL_I24,                   EACCES    },
  { ERROR_INVALID_PARAMETER,          EINVAL    },
  { ERROR_NO_PROC_SLOTS,              EAGAIN    },
  { ERROR_DRIVE_LOCKED,               EACCES    },
  { ERROR_BROKEN_PIPE,                EPIPE     },
  { ERROR_DISK_FULL,                  ENOSPC    },
  { ERROR_INVALID_TARGET_HANDLE,      EBADF     },
  { ERROR_INVALID_HANDLE,             EINVAL    },
  { ERROR_WAIT_NO_CHILDREN,           ECHILD    },
  { ERROR_CHILD_NOT_COMPLETE,         ECHILD    },
  { ERROR_DIRECT_ACCESS_HANDLE,       EBADF     },
  { ERROR_NEGATIVE_SEEK,              EINVAL    },
  { ERROR_SEEK_ON_DEVICE,             EACCES    },
  { ERROR_DIR_NOT_EMPTY,              ENOTEMPTY },
  { ERROR_DIRECTORY,                  ENOTDIR   },
  { ERROR_NOT_LOCKED,                 EACCES    },
  { ERROR_BAD_PATHNAME,               ENOENT    },
  { ERROR_MAX_THRDS_REACHED,          EAGAIN    },
  { ERROR_LOCK_FAILED,                EACCES    },
  { ERROR_ALREADY_EXISTS,             EEXIST    },
  { ERROR_INVALID_STARTING_CODESEG,   ENOEXEC   },
  { ERROR_INVALID_STACKSEG,           ENOEXEC   },
  { ERROR_INVALID_MODULETYPE,         ENOEXEC   },
  { ERROR_INVALID_EXE_SIGNATURE,      ENOEXEC   },
  { ERROR_EXE_MARKED_INVALID,         ENOEXEC   },
  { ERROR_BAD_EXE_FORMAT,             ENOEXEC   },
  { ERROR_ITERATED_DATA_EXCEEDS_64k,  ENOEXEC   },
  { ERROR_INVALID_MINALLOCSIZE,       ENOEXEC   },
  { ERROR_DYNLINK_FROM_INVALID_RING,  ENOEXEC   },
  { ERROR_IOPL_NOT_ENABLED,            ENOEXEC  },
  { ERROR_INVALID_SEGDPL,             ENOEXEC   },
  { ERROR_AUTODATASEG_EXCEEDS_64k,    ENOEXEC   },
  { ERROR_RING2SEG_MUST_BE_MOVABLE,   ENOEXEC   },
  { ERROR_RELOC_CHAIN_XEEDS_SEGLIM,   ENOEXEC   },
  { ERROR_INFLOOP_IN_RELOC_CHAIN,      ENOEXEC  },
  { ERROR_FILENAME_EXCED_RANGE,        ENOENT   },
  { ERROR_NESTING_NOT_ALLOWED,        EAGAIN    },
#ifndef ERROR_PIPE_LOCAL
#define ERROR_PIPE_LOCAL  229L
#endif
  {  ERROR_PIPE_LOCAL,                 EPIPE           },
  {  ERROR_BAD_PIPE,                   EPIPE           },
  {  ERROR_PIPE_BUSY,                  EAGAIN          },
  {  ERROR_NO_DATA,                    EPIPE           },
  {  ERROR_PIPE_NOT_CONNECTED,         EPIPE           },
  {  ERROR_OPERATION_ABORTED,          EINTR           },
  {  ERROR_NOT_ENOUGH_QUOTA,           ENOMEM          },
  {  ERROR_MOD_NOT_FOUND,              ENOENT          },
  {  WSAEINTR,                         EINTR           },
  {  WSAEBADF,                         EBADF           },
  {  WSAEACCES,                        EACCES          },
  {  WSAEFAULT,                        EFAULT          },
  {  WSAEINVAL,                        EINVAL          },
  {  WSAEMFILE,                        EMFILE          },
  {  WSAEWOULDBLOCK,                   EWOULDBLOCK     },
  {  WSAEINPROGRESS,                   EINPROGRESS     },
  {  WSAEALREADY,                      EALREADY        },
  {  WSAENOTSOCK,                      ENOTSOCK        },
  {  WSAEDESTADDRREQ,                  EDESTADDRREQ    },
  {  WSAEMSGSIZE,                      EMSGSIZE        },
  {  WSAEPROTOTYPE,                    EPROTOTYPE      },
  {  WSAENOPROTOOPT,                   ENOPROTOOPT     },
  {  WSAEPROTONOSUPPORT,               EPROTONOSUPPORT },
  {  WSAESOCKTNOSUPPORT,               ESOCKTNOSUPPORT },
  {  WSAEOPNOTSUPP,                    EOPNOTSUPP      },
  {  WSAEPFNOSUPPORT,                  EPFNOSUPPORT    },
  {  WSAEAFNOSUPPORT,                  EAFNOSUPPORT    },
  {  WSAEADDRINUSE,                    EADDRINUSE      },
  {  WSAEADDRNOTAVAIL,                 EADDRNOTAVAIL   },
  {  WSAENETDOWN,                      ENETDOWN        },
  {  WSAENETUNREACH,                   ENETUNREACH     },
  {  WSAENETRESET,                     ENETRESET       },
  {  WSAECONNABORTED,                  ECONNABORTED    },
  {  WSAECONNRESET,                    ECONNRESET      },
  {  WSAENOBUFS,                       ENOBUFS         },
  {  WSAEISCONN,                       EISCONN         },
  {  WSAENOTCONN,                      ENOTCONN        },
  {  WSAESHUTDOWN,                     ESHUTDOWN       },
  {  WSAETOOMANYREFS,                  ETOOMANYREFS    },
  {  WSAETIMEDOUT,                     ETIMEDOUT       },
  {  WSAECONNREFUSED,                  ECONNREFUSED    },
  {  WSAELOOP,                         ELOOP           },
  {  WSAENAMETOOLONG,                  ENAMETOOLONG    },
  {  WSAEHOSTDOWN,                     EHOSTDOWN       },
  {  WSAEHOSTUNREACH,                  EHOSTUNREACH    },
  {  WSAEPROCLIM,                      EPROCLIM        },
  {  WSAENOTEMPTY,                     ENOTEMPTY       },
  {  WSAEUSERS,                        EUSERS          },
  {  WSAEDQUOT,                        EDQUOT          },
  {  WSAESTALE,                        ESTALE          },
  {  WSAEREMOTE,                       EREMOTE         },
};

#define ERROR_MAP_SIZE  (int)(sizeof(error_map) / sizeof(*error_map))

// Adapted from MRI.
int map_errno(DWORD winerr) {
  if(winerr == 0) {
    return 0;
  }

  for(int i = 0; i < ERROR_MAP_SIZE; i++) {
    if (error_map[i].winerr == winerr) {
      return error_map[i].err;
    }
  }

  if(winerr >= WSABASEERR) {
    return winerr;
  }

  return EINVAL;
}

#endif
