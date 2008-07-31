#ifndef RBX_BUILTIN_IO_HPP
#define RBX_BUILTIN_IO_HPP

namespace rubinius {
  class IO : public Object {
    public:
    const static size_t fields = 4;
    const static object_type type = IOType;

    OBJECT __ivars__; // slot
    FIXNUM descriptor; // slot
    OBJECT buffer; // slot
    OBJECT mode; // slot

    native_int to_fd() {
      return descriptor->to_nint();
    }

    static void init(STATE);
    static IO* create(STATE, int fd);
    void initialize(STATE, int fd, char* mode);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

  class IOBuffer : public Object {
  public:
    const static size_t fields = 4;
    const static object_type type = IOBufferType;

    OBJECT __ivars__; // slot
    OBJECT storage; // slot
    INTEGER total; // slot
    INTEGER used; // slot

    static IOBuffer* create(STATE, size_t bytes);
    void reset(STATE);
    String* drain(STATE);

    char* byte_address() {
      return (char*)storage->bytes;
    }

    size_t left() {
      return total->n2i() - used->n2i();
    }

    char* at_unused() {
      char* start = (char*)storage->bytes;
      start += used->n2i();
      return start;
    }

    void read_bytes(size_t bytes) {
      used = Object::i2n(used->n2i() + bytes);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
};

#endif
