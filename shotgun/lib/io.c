#include "shotgun.h"
#include <fcntl.h>

void io_initialize(STATE, int fd) {
  int flags;
  
  return;  
  /* FIXME: do some error checking. */
  
  flags = fcntl(fd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  
  fcntl(fd, F_SETFL, flags);
}

OBJECT io_new(STATE, int fd) {
  OBJECT io;
  io = io_allocate_with_extra(state, 1);
  io_set_descriptor(io, I2N(fd));
  io_initialize(state, fd);
  return io;
}

