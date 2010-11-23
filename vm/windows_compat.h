#ifndef RBX_WINDOWS_COMPAT_H
#define RBX_WINDOWS_COMPAT_H

#ifdef RBX_WINDOWS

// system

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

#endif

#endif
