#ifndef RBX_BUILTIN_IO_HPP
#define RBX_BUILTIN_IO_HPP

#include "object_utils.hpp"

#include "builtin/byte_array.hpp"
#include "builtin/encoding.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

namespace rubinius {
  class ByteArray;
  class String;
  class Encoding;

  class IO : public Object {

  public:
    const static object_type type = IOType;

  public:
    /* interface */

    static void bootstrap(STATE);
    static void initialize(STATE, IO* obj) {
      obj->descriptor_ = nil<Fixnum>();
      obj->path_ = nil<String>();
      obj->ibuffer_ = nil<Object>();
      obj->mode_ = nil<Fixnum>();
      obj->eof_ = cFalse;
      obj->lineno_ = Fixnum::from(0);
      obj->sync_ = nil<Object>();
      obj->external_ = nil<Encoding>();
      obj->internal_ = nil<Encoding>();
      obj->autoclose_ = nil<Object>();
    }

    static IO* create(STATE, int fd);

    native_int descriptor(STATE, CallFrame* frame);
    void ensure_open(STATE);

  /* Class primitives */

    // Rubinius.primitive :io_fnmatch
    static Object* fnmatch(STATE, String* pattern, String* path, Fixnum* flags);

  /* Instance primitives */

    // Rubinius.primitive :io_socket_read
    Object* socket_read(STATE, Fixnum* bytes, Fixnum* flags, Fixnum* type);

    // Rubinius.primitive :io_send_io
    Object* send_io(STATE, IO* io);

    // Rubinius.primitive :io_recv_fd
    Object* recv_fd(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      void auto_mark(Object* obj, ObjectMark& mark) { }
      void set_field(STATE, Object* target, size_t index, Object* val) { }
      Object* get_field(STATE, Object* target, size_t index) { return cNil; }
      void populate_slot_locations() { }
    };

  };

  class FDSet : public Object {
  public:
    const static object_type type = FDSetType;

  private:
    uint8_t descriptor_set[sizeof(fd_set)];

  public:

    static void bootstrap(STATE);

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

    static void bootstrap(STATE);

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
