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

#endif  // RBX_WINDOWS

// Keep this ifdef short so it's clear
#ifdef RBX_WINDOWS
// MinGW defines setjmp in terms of _setjmp.
#define set_jump(x)       setjmp(x)
#define long_jump(x, y)   longjmp(x, y)
#else
#define set_jump(x)       _setjmp(x)
#define long_jump(x, y)   _longjmp(x, y)
#endif  // RBX_WINDOWS

#endif  // RBX_WINDOWS_COMPAT_H
