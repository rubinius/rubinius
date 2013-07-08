#ifndef RBX_BUILTIN_IO_HPP
#define RBX_BUILTIN_IO_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class ByteArray;
  class String;
  class Encoding;

  class IO : public Object {
  public:
    const static object_type type = IOType;

  private:
    Fixnum* descriptor_; // slot
    String* path_;       // slot
    Object* ibuffer_;    // slot
    Fixnum* mode_;       // slot
    Object* eof_;        // slot
    Integer* lineno_;    // slot
    Object* sync_;       // slot
    Encoding* external_; // slot
    Encoding* internal_; // slot
    Object* autoclose_;  // slot

  public:
    /* accessors */

    attr_accessor(descriptor, Fixnum);
    attr_accessor(path, String);
    attr_accessor(ibuffer, Object);
    attr_accessor(mode, Fixnum);
    attr_accessor(eof, Object);
    attr_accessor(lineno, Integer);
    attr_accessor(sync, Object);
    attr_accessor(external, Encoding);
    attr_accessor(internal, Encoding);
    attr_accessor(autoclose, Object);

    /* interface */

    static void init(STATE);
    static IO* create(STATE, int fd);

    native_int to_fd();
    void set_mode(STATE);
    void force_read_only(STATE);
    void force_write_only(STATE);
    static void finalize(STATE, IO* io);

  /* Class primitives */

    // Rubinius.primitive :io_allocate
    static IO*      allocate(STATE, Object* self);

    // Rubinius.primitive :io_connect_pipe
    static Object*  connect_pipe(STATE, IO* lhs, IO* rhs);

    // Rubinius.primitive :io_open
    static Fixnum*  open(STATE, String* path, Fixnum* mode, Fixnum* perm, CallFrame* calling_environment);

    static void update_max_fd(STATE, native_int fd);

    /**
     *  Perform select() on descriptors.
     *
     *  @todo Replace with an evented version when redoing events. --rue
     */
    // Rubinius.primitive :io_select
    static Object*  select(STATE, Object* readables, Object* writables, Object* errorables, Object* timeout, CallFrame* calling_environment);

    // Rubinius.primitive :io_fnmatch
    static Object* fnmatch(STATE, String* pattern, String* path, Fixnum* flags);

  /* Instance primitives */

    // Rubinius.primitive :io_ensure_open
    Object* ensure_open(STATE);

    /**
     *  Directly read up to number of bytes from descriptor.
     *
     *  Returns cNil at EOF.
     */
    // Rubinius.primitive :io_sysread
    Object* sysread(STATE, Fixnum* number_of_bytes, CallFrame* calling_environment);

    // Rubinius.primitive :io_read_if_available
    Object* read_if_available(STATE, Fixnum* number_of_bytes);

    // Rubinius.primitive :io_socket_read
    Object* socket_read(STATE, Fixnum* bytes, Fixnum* flags, Fixnum* type, CallFrame* calling_environment);

    // Rubinius.primitive :io_seek
    Integer* seek(STATE, Integer* amount, Fixnum* whence);

    // Rubinius.primitive :io_truncate
    static Integer* truncate(STATE, String* name, Fixnum* off);

    // Rubinius.primitive :io_ftruncate
    Integer* ftruncate(STATE, Fixnum* off);

    // Rubinius.primitive :io_write
    Object* write(STATE, String* buf, CallFrame* calling_environment);

    // Rubinius.primitive :io_reopen
    Object* reopen(STATE, IO* other);

    // Rubinius.primitive :io_reopen_path
    Object* reopen_path(STATE, String* other, Fixnum * mode, CallFrame* calling_environment);

    // Rubinius.primitive :io_close
    Object* close(STATE);

    // Rubinius.primitive :io_accept
    Object* accept(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :io_send_io
    Object* send_io(STATE, IO* io);

    // Rubinius.primitive :io_recv_fd
    Object* recv_fd(STATE, CallFrame* calling_environment);

    /**
     *  Shutdown a full-duplex descriptor's read and/or write stream.
     *
     *  Careful with this, it applies to full-duplex only.
     *  It also shuts the stream *in all processes*, not
     *  just the current one.
     */
    // Rubinius.primitive :io_shutdown
    Object* shutdown(STATE, Fixnum* how);

    // Rubinius.primitive :io_query
    Object* query(STATE, Symbol* op);

    // Rubinius.primitive :io_write_nonblock
    Object* write_nonblock(STATE, String* buf);

    // Rubinius.primitive :io_advise
    Object* advise(STATE, Symbol* advice_name, Integer* offset, Integer* len);

    void set_nonblock(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

#define IOBUFFER_SIZE 32768U

  class IOBuffer : public Object {
  public:
    const static size_t fields = 7;
    const static object_type type = IOBufferType;

  private:
    ByteArray* storage_;   // slot
    Integer* total_;       // slot
    Integer* used_;        // slot
    Integer* start_;       // slot
    Object* eof_;          // slot
    Object* write_synced_; // slot

  public:
    /* accessors */

    attr_accessor(storage, ByteArray);
    attr_accessor(total, Integer);
    attr_accessor(used, Integer);
    attr_accessor(start, Integer);
    attr_accessor(eof, Object);
    attr_accessor(write_synced, Object);

    /* interface */

    static IOBuffer* create(STATE, size_t bytes = IOBUFFER_SIZE);
    // Rubinius.primitive :iobuffer_allocate
    static IOBuffer* allocate(STATE);

    // Rubinius.primitive :iobuffer_unshift
    Object* unshift(STATE, String* str, Fixnum* start_pos);

    // Rubinius.primitive :iobuffer_fill
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
