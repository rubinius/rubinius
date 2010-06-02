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
    const static object_type type = IOType;

  private:
    Fixnum* descriptor_; // slot
    Object* ibuffer_;    // slot
    Fixnum* mode_;       // slot
    Object* eof_;        // slot
    Integer* lineno_;    // slot
    Object* sync_;       // slot


  public:
    /* accessors */

    attr_accessor(descriptor, Fixnum);
    attr_accessor(ibuffer, Object);
    attr_accessor(mode, Fixnum);
    attr_accessor(eof, Object);
    attr_accessor(lineno, Integer);
    attr_accessor(sync, Object);

    /* interface */

    static void init(STATE);
    static IO* create(STATE, int fd);

    native_int to_fd();
    void set_mode(STATE);
    void unsafe_set_descriptor(native_int fd);
    void force_read_only(STATE);
    void force_write_only(STATE);
    static void finalize(STATE, IO* io);

  /* Class primitives */

    // Ruby.primitive :io_allocate
    static IO*      allocate(STATE, Object* self);

    // Ruby.primitive :io_connect_pipe
    static Object*  connect_pipe(STATE, IO* lhs, IO* rhs);

    // Ruby.primitive :io_open
    static Fixnum*  open(STATE, String* path, Fixnum* mode, Fixnum* perm);

    /**
     *  Perform select() on descriptors.
     *
     *  @todo Replace with an evented version when redoing events. --rue
     */
    // Ruby.primitive :io_select
    static Object*  select(STATE, Object* readables, Object* writables, Object* errorables, Object* timeout, CallFrame* calling_environment);

    // Ruby.primitive :io_fnmatch
    static Object* fnmatch(STATE, String* pattern, String* path, Fixnum* flags);

  /* Instance primitives */

    // Ruby.primitive :io_ensure_open
    Object* ensure_open(STATE);

    /**
     *  Directly read up to number of bytes from descriptor.
     *
     *  Returns Qnil at EOF.
     */
    // Ruby.primitive :io_sysread
    Object* sysread(STATE, Fixnum* number_of_bytes, CallFrame* calling_environment);

    // Ruby.primitive :io_read_if_available
    Object* read_if_available(STATE, Fixnum* number_of_bytes);

    // Ruby.primitive :io_socket_read
    Object* socket_read(STATE, Fixnum* bytes, Fixnum* flags, Fixnum* type, CallFrame* calling_environment);

    // Ruby.primitive :io_seek
    Integer* seek(STATE, Integer* amount, Fixnum* whence);

    // Ruby.primitive :io_write
    Object* write(STATE, String* buf, CallFrame* calling_environment);

    Object* unlocked_write(STATE, String* buf, CallFrame* calling_environment);

    // Ruby.primitive :io_reopen
    Object* reopen(STATE, IO* other);

    // Ruby.primitive :io_reopen_path
    Object* reopen_path(STATE, String* other, Fixnum * mode);

    // Ruby.primitive :io_close
    Object* close(STATE);

    // Ruby.primitive :io_accept
    Object* accept(STATE, CallFrame* calling_environment);

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

    void set_nonblock(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

#define IOBUFFER_SIZE 1024U

  class IOBuffer : public Object {
  public:
    const static size_t fields = 7;
    const static object_type type = IOBufferType;

  private:
    ByteArray* storage_; // slot
    Channel* channel_;   // slot
    Integer* total_;      // slot
    Integer* used_;       // slot
    Integer* start_;      // slot
    Object* eof_;         // slot
    Object* write_synced_;// slot

  public:
    /* accessors */

    attr_accessor(storage, ByteArray);
    attr_accessor(channel, Channel);
    attr_accessor(total, Integer);
    attr_accessor(used, Integer);
    attr_accessor(start, Integer);
    attr_accessor(eof, Object);
    attr_accessor(write_synced, Object);

    /* interface */

    static IOBuffer* create(STATE, size_t bytes = IOBUFFER_SIZE);
    // Ruby.primitive :iobuffer_allocate
    static IOBuffer* allocate(STATE);

    // Ruby.primitive :iobuffer_unshift
    Object* unshift(STATE, String* str, Fixnum* start_pos);

    // Ruby.primitive :iobuffer_fill
    Object* fill(STATE, IO* io, CallFrame* calling_environment);

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

}

#endif
