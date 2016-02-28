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

  public:
    /* accessors */

    attr_accessor(descriptor, Fixnum);

    /* interface */

    static void init(STATE);
    static IO* create(STATE, int fd);

    native_int to_fd();

  /* Class primitives */

    static IO*      allocate(STATE, Object* self);

    // Rubinius.primitive :io_fnmatch
    static Object* fnmatch(STATE, String* pattern, String* path, Fixnum* flags);

  /* Instance primitives */

    Object* ensure_open(STATE);

    // Rubinius.primitive :io_socket_read
    Object* socket_read(STATE, Fixnum* bytes, Fixnum* flags, Fixnum* type, CallFrame* calling_environment);

    // Rubinius.primitive :io_send_io
    Object* send_io(STATE, IO* io);

    // Rubinius.primitive :io_recv_fd
    Object* recv_fd(STATE, CallFrame* calling_environment);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
  
  class FDSet : public Object {
  public:
    const static object_type type = FDSetType;
    
  private:
    uint8_t descriptor_set[sizeof(fd_set)];
    
  public:
  
    static void init(STATE);

    static FDSet* create(STATE);
    
    // Rubinius.primitive :fdset_allocate
    static FDSet* allocate(STATE, Object* self);
    
      // Rubinius.primitive :fdset_zero
    Object* zero(STATE);
    
    // Rubinius.primitive :fdset_is_set
    Object* is_set(STATE, Fixnum* descriptor);
    
    // Rubinius.primitive :fdset_set
    Object* set(STATE, Fixnum* descriptor);
	
	// Rubinius.primitive :fdset_to_set
	Object* to_set(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      void auto_mark(Object* obj, ObjectMark& mark) { }
      void set_field(STATE, Object* target, size_t index, Object* val) { }
      Object* get_field(STATE, Object* target, size_t index) { return cNil; }
      void populate_slot_locations() { }
    };
  };
  
  class RIOStream : public Object {
  public:
    const static object_type type = RIOStreamType;

    static void init(STATE);

    // Rubinius.primitive :rio_close
    static Object* close(STATE, Object* io, Object* allow_exception);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      void auto_mark(Object* obj, ObjectMark& mark) { }
      void set_field(STATE, Object* target, size_t index, Object* val) { }
      Object* get_field(STATE, Object* target, size_t index) { return cNil; }
      void populate_slot_locations() { }
    };
  };

}

#endif
