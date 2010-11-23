#ifndef RBX_WINDOWS_COMPAT_H
#define RBX_WINDOWS_COMPAT_H

#ifdef RBX_WINDOWS

// socket
int socketpair(int domain, int type, int protocol, int socket_vector[2]);
int pipe(int fildes[2]);

// file system
char* realpath(const char* file_name, char* resolved_name);

#endif

#endif
