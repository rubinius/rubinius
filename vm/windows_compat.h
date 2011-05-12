#ifndef RBX_WINDOWS_COMPAT_H
#define RBX_WINDOWS_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RBX_WINDOWS

// crypt
char *crypt(const char *, const char *);

// dlopen
#include <winsock2.h>

int map_errno(DWORD winerr);

inline void* dlopen(const char* name, int mode) {
  return LoadLibrary(name);
}

inline int dlclose(void* handle) {
  return !FreeLibrary((HMODULE)handle);
}

inline const char* dlerror(void) {
  return strerror(map_errno(GetLastError()));
}

inline void* dlsym(void* handle, const char* symbol) {
  return (void*)GetProcAddress((HMODULE)handle, symbol);
}

#define RTLD_LAZY -1
#define RTLD_NOW  -1
#define RTLD_GLOBAL -1

// environment
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);

// Adapted from BSD source code.
#define UNAME_NAMELEN    256

struct  utsname {
  char sysname[UNAME_NAMELEN];  /* Name of OS */
  char nodename[UNAME_NAMELEN]; /* Name of this network node */
  char release[UNAME_NAMELEN];  /* Release level */
  char version[UNAME_NAMELEN];  /* Version level */
  char machine[UNAME_NAMELEN];  /* Hardware type */
};

int uname(struct utsname *name);

// socket

#include <winsock2.h>

#ifndef SHUT_RD
#ifdef SD_RECEIVE
#define SHUT_RD   SD_RECEIVE
#else
#define SHUT_RD   0
#endif
#endif

#ifndef SHUT_WR
#ifdef SD_SEND
#define SHUT_WR   SD_SEND
#else
#define SHUT_WR   1
#endif
#endif

#ifndef SHUT_RDWR
#ifdef SD_BOTH
#define SHUT_RDWR SD_BOTH
#else
#define SHUT_RDWR 2
#endif
#endif

#include <errno.h>
#include <pthread.h>

#ifndef EWOULDBLOCK
# define EWOULDBLOCK		WSAEWOULDBLOCK
#endif
#ifndef EINPROGRESS
# define EINPROGRESS		WSAEINPROGRESS
#endif
#ifndef EALREADY
# define EALREADY		WSAEALREADY
#endif
#ifndef ENOTSOCK
# define ENOTSOCK		WSAENOTSOCK
#endif
#ifndef EDESTADDRREQ
# define EDESTADDRREQ		WSAEDESTADDRREQ
#endif
#ifndef EMSGSIZE
# define EMSGSIZE		WSAEMSGSIZE
#endif
#ifndef EPROTOTYPE
# define EPROTOTYPE		WSAEPROTOTYPE
#endif
#ifndef ENOPROTOOPT
# define ENOPROTOOPT		WSAENOPROTOOPT
#endif
#ifndef EPROTONOSUPPORT
# define EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#endif
#ifndef ESOCKTNOSUPPORT
# define ESOCKTNOSUPPORT	WSAESOCKTNOSUPPORT
#endif
#ifndef EOPNOTSUPP
# define EOPNOTSUPP		WSAEOPNOTSUPP
#endif
#ifndef EPFNOSUPPORT
# define EPFNOSUPPORT		WSAEPFNOSUPPORT
#endif
#ifndef EAFNOSUPPORT
# define EAFNOSUPPORT		WSAEAFNOSUPPORT
#endif
#ifndef EADDRINUSE
# define EADDRINUSE		WSAEADDRINUSE
#endif
#ifndef EADDRNOTAVAIL
# define EADDRNOTAVAIL		WSAEADDRNOTAVAIL
#endif
#ifndef ENETDOWN
# define ENETDOWN		WSAENETDOWN
#endif
#ifndef ENETUNREACH
# define ENETUNREACH		WSAENETUNREACH
#endif
#ifndef ENETRESET
# define ENETRESET		WSAENETRESET
#endif
#ifndef ECONNABORTED
# define ECONNABORTED		WSAECONNABORTED
#endif
#ifndef ECONNRESET
# define ECONNRESET		WSAECONNRESET
#endif
#ifndef ENOBUFS
# define ENOBUFS		WSAENOBUFS
#endif
#ifndef EISCONN
# define EISCONN		WSAEISCONN
#endif
#ifndef ENOTCONN
# define ENOTCONN		WSAENOTCONN
#endif
#ifndef ESHUTDOWN
# define ESHUTDOWN		WSAESHUTDOWN
#endif
#ifndef ETOOMANYREFS
# define ETOOMANYREFS		WSAETOOMANYREFS
#endif
#ifndef ETIMEDOUT
# define ETIMEDOUT		WSAETIMEDOUT
#endif
#ifndef ECONNREFUSED
# define ECONNREFUSED		WSAECONNREFUSED
#endif
#ifndef ELOOP
# define ELOOP			WSAELOOP
#endif
#ifndef EHOSTDOWN
# define EHOSTDOWN		WSAEHOSTDOWN
#endif
#ifndef EHOSTUNREACH
# define EHOSTUNREACH		WSAEHOSTUNREACH
#endif
#ifndef EPROCLIM
# define EPROCLIM		WSAEPROCLIM
#endif
#ifndef EUSERS
# define EUSERS			WSAEUSERS
#endif
#ifndef EDQUOT
# define EDQUOT			WSAEDQUOT
#endif
#ifndef ESTALE
# define ESTALE			WSAESTALE
#endif
#ifndef EREMOTE
# define EREMOTE		WSAEREMOTE
#endif


int socketpair(int domain, int type, int protocol, int socket_vector[2]);
int pipe(int fildes[2]);

// file system
char* realpath(char* file_name, char* resolved_name);

// time
#define timezone  _timezone

time_t timegm(struct tm* tm);
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

#ifndef localtime_r
#define localtime_r(_Time, _Tm)  ({           \
  struct tm *___tmp_tm = localtime((_Time));  \
  if (___tmp_tm) {                            \
    *(_Tm) = *___tmp_tm;                      \
    ___tmp_tm = (_Tm);                        \
  }                                           \
  ___tmp_tm;                                  \
})
#endif

#ifndef gmtime_r
#define gmtime_r(_Time,_Tm)  ({               \
  struct tm *___tmp_tm = gmtime((_Time));     \
  if (___tmp_tm) {                            \
    *(_Tm) = *___tmp_tm;                      \
    ___tmp_tm = (_Tm);                        \
  }                                           \
  ___tmp_tm;                                  \
})
#endif

#define RBX_USEC_PER_SEC   1000000
#define timeradd(tvp, uvp, vvp)                       \
  do {                                                \
    (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;    \
    (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec; \
    if ((vvp)->tv_usec >= RBX_USEC_PER_SEC) {         \
      (vvp)->tv_sec++;                                \
      (vvp)->tv_usec -= RBX_USEC_PER_SEC;             \
    }                                                 \
  } while (0)
#define timersub(tvp, uvp, vvp)                       \
  do {                                                \
    (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;    \
    (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec; \
    if ((vvp)->tv_usec < 0) {                         \
      (vvp)->tv_sec--;                                \
      (vvp)->tv_usec += RBX_USEC_PER_SEC;             \
    }                                                 \
  } while (0)

// fcntl
#define F_SETFL 1
#define O_NONBLOCK 1

int fcntl(int fildes, int cmd, ...);

#endif  // RBX_WINDOWS

// Keep these ifdef short so they are clear. Each one
// defines a Windows and non-Windows alternative.
#ifdef RBX_WINDOWS
// MinGW defines setjmp in terms of _setjmp.
#define set_jump(x)       setjmp(x)
#define long_jump(x, y)   longjmp(x, y)
#else
#define set_jump(x)       _setjmp(x)
#define long_jump(x, y)   _longjmp(x, y)
#endif

#ifdef RBX_WINDOWS
typedef unsigned int int_fd_t;
#else
typedef int int_fd_t;
#endif

#ifdef __cplusplus
}
#endif

#endif  // RBX_WINDOWS_COMPAT_H
