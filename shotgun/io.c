#include "rubinius.h"

OBJECT io_new(STATE, int fd) {
  OBJECT io;
  io = io_allocate(state);
  io_set_descriptor(io, I2N(fd));
  return io;
}