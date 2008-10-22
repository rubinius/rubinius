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
    const static size_t fields = 5;
    const static object_type type = IOType;

  private:
    Fixnum* descriptor_; // slot
    Object* ibuffer_;    // slot
    Fixnum* mode_;       // slot
    Object* eof_;        // slot
    Fixnum* lineno_;     // slot


  public:
    /* accessors */

    attr_accessor(descriptor, Fixnum);
    attr_accessor(ibuffer, Object);
    attr_accessor(mode, Fixnum);
    attr_accessor(eof, Object);
    attr_accessor(lineno, Fixnum);

    /* interface */

    static void init(STATE);
    static IO* create(STATE, int fd);

    native_int to_fd();
    void set_mode(STATE);
    void unsafe_set_descriptor(native_int fd);
    void force_read_only(STATE);
    void force_write_only(STATE);

    // Ruby.primitive :io_allocate
    static IO* allocate(STATE, Object* self);

    // Ruby.primitive :io_ensure_open
    Object* ensure_open(STATE);

    // Ruby.primitive :io_connect_pipe
    static Object* connect_pipe(STATE, IO* lhs, IO* rhs);

    // Ruby.primitive :io_seek
    Integer* seek(STATE, Integer* amount, Fixnum* whence);

    // Ruby.primitive :io_write
    Object* write(STATE, String* buf);

    // Ruby.primitive :io_open
    static Fixnum* open(STATE, String* path, Fixnum* mode, Fixnum* perm);

    // Ruby.primitive :io_reopen
    Object* reopen(STATE, IO* other);

    // Ruby.primitive :io_close
    Object* close(STATE);

    /**
     *  Shutdown a full-duplex descriptor's read and/or write stream.
     *
     *  Careful with this, it applies to full-duplex only.
     *  It also shuts the stream *in all processes*, not
     *  just the current one.
     */
    // Ruby.primitive :io_shutdown
    Object* shutdown(STATE, Fixnum* how);

    // Ruby.primitive :io_blocking_read
    Object* blocking_read(STATE, Fixnum* count);

    // Ruby.primitive :io_query
    Object* query(STATE, Symbol* op);

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
    Integer* total_;      // slot
    Integer* used_;       // slot
    Integer* start_;      // slot
    Object* eof_;         // slot

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
