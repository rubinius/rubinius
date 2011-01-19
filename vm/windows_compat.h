#ifndef RBX_WINDOWS_COMPAT_H
#define RBX_WINDOWS_COMPAT_H

#ifdef RBX_WINDOWS

// crypt
char *crypt(const char *, const char *);

// environment
int setenv(const char *name, const char *value, int overwrite);

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
int socketpair(int domain, int type, int protocol, int socket_vector[2]);
int pipe(int fildes[2]);

// file system
char* realpath(const char* file_name, char* resolved_name);

// time
#define timezone  _timezone

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

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

#endif  // RBX_WINDOWS_COMPAT_H
