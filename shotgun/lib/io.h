OBJECT io_new(STATE, int fd, const char *mode);
void io_initialize(STATE, int fd);
OBJECT io_wrap(STATE, OBJECT io, int fd, const char *mode);

//#define io_to_fd(io) fileno((FILE*)ffi_pointer(io_get_descriptor(io)))
#define io_to_fd(io) FIXNUM_TO_INT(io_get_descriptor(io))
