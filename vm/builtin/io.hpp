#ifndef RBX_BUILTIN_IO_HPP
#define RBX_BUILTIN_IO_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class String;

  class IO : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = IOType;

  private:
    FIXNUM descriptor_; // slot
    OBJECT buffer_;     // slot
    OBJECT mode_;       // slot

  public:
    /* accessors */

    attr_accessor(descriptor, Fixnum);
    attr_accessor(buffer, Object);
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

  class IOBuffer : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = IOBufferType;

  private:
    OBJECT storage_; // slot
    INTEGER total_;  // slot
    INTEGER used_;   // slot

  public:
    /* accessors */

    attr_accessor(storage, Object);
    attr_accessor(total, Integer);
    attr_accessor(used, Integer);

    /* interface */

    static IOBuffer* create(STATE, size_t bytes);
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
