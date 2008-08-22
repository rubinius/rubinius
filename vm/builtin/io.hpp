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

    FIXNUM descriptor; // slot
    OBJECT buffer; // slot
    OBJECT mode; // slot

    native_int to_fd();

    static void init(STATE);
    static IO* create(STATE, int fd);
    void initialize(STATE, int fd, char* mode);

    // Ruby.primitive :io_write
    OBJECT write(STATE, String* buf);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

  class IOBuffer : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = IOBufferType;

    OBJECT storage; // slot
    INTEGER total; // slot
    INTEGER used; // slot

    static IOBuffer* create(STATE, size_t bytes);
    void reset(STATE);
    String* drain(STATE);
    char* byte_address();
    size_t left();
    char* at_unused();
    void read_bytes(size_t bytes);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
};

#endif
