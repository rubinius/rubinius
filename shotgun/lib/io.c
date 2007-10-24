#include "shotgun.h"
#include "subtend/ffi.h"
#include <fcntl.h>
#include <unistd.h>

void io_initialize(STATE, int fd) {
  int flags;
  
  return;
  
  /* FIXME: do some error checking. */
  
  if(!isatty(fd)) {
    flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
  
    fcntl(fd, F_SETFL, flags);
  }
}

OBJECT io_wrap(STATE, OBJECT io, int fd, const char *mode) {
  io_set_descriptor(io, I2N(fd));
  // io_set_descriptor(io, ffi_new_pointer(state, (void*)fdopen(fd, mode)));
  io_initialize(state, fd);
  return io;
}

OBJECT io_new(STATE, int fd, const char *mode) {
  OBJECT io;
  io = io_allocate_with_extra(state, 1);
  io_wrap(state, io, fd, mode);
  return io;
}

OBJECT io_open(STATE, const char *path, const char *mode) {
  OBJECT io;
  FILE *fptr;
  fptr = fopen(path, mode);
  if(!fptr) { return Qnil; }
  
  io = io_allocate_with_extra(state, 1);
  io_set_descriptor(io, I2N(fileno(fptr)));
  // io_set_descriptor(io, ffi_new_pointer(state, (void*)fptr));
  io_initialize(state, fileno(fptr));
  return io;
}

