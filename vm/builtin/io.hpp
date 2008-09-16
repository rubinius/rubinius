#ifndef RBX_BUILTIN_IO_HPP
#define RBX_BUILTIN_IO_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class ByteArray;
  class Channel;
  class String;
  class ByteArray;

  class IO : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = IOType;

  private:
    FIXNUM descriptor_; // slot
    OBJECT ibuffer_;    // slot
    OBJECT mode_;       // slot

  public:
    /* accessors */

    attr_accessor(descriptor, Fixnum);
    attr_accessor(ibuffer, Object);
    attr_accessor(mode, Object);

    /* interface */

    native_int to_fd();

    static void init(STATE);
    static IO* create(STATE, int fd);
    void initialize(STATE, int fd, char* mode);

    // Ruby.primitive :io_seek
    INTEGER seek(STATE, INTEGER amount, FIXNUM whence);

    // Ruby.primitive :io_write
    OBJECT write(STATE, String* buf);

    // Ruby.primitive :io_open
    static FIXNUM open(STATE, String* path, FIXNUM mode, FIXNUM perm);

    // Ruby.primitive :io_close
    OBJECT close(STATE);

    // Ruby.primitive :io_blocking_read
    OBJECT blocking_read(STATE, FIXNUM count);

    // Ruby.primitive :io_query
    OBJECT query(STATE, SYMBOL op);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

#define IOBUFFER_SIZE 32384U

  class IOBuffer : public Object {
  public:
    const static size_t fields = 6;
    const static object_type type = IOBufferType;

  private:
    ByteArray* storage_; // slot
    Channel* channel_;   // slot
    INTEGER total_;      // slot
    INTEGER used_;       // slot
    INTEGER start_;      // slot
    OBJECT eof_;         // slot

  public:
    /* accessors */

    attr_accessor(storage, ByteArray);
    attr_accessor(channel, Channel);
    attr_accessor(total, Integer);
    attr_accessor(used, Integer);
    attr_accessor(start, Integer);
    attr_accessor(eof, Object);

    /* interface */

    static IOBuffer* create(STATE, size_t bytes = IOBUFFER_SIZE);
    // Ruby.primitive :iobuffer_allocate
    static IOBuffer* allocate(STATE);
    void reset(STATE);
    String* drain(STATE);
    char* byte_address();
    size_t left();
    char* at_unused();
    void read_bytes(STATE, size_t bytes);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
};

#endif
