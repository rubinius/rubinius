#ifndef RBX_BUILTIN_DIR_HPP
#define RBX_BUILTIN_DIR_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class MemoryPointer;
  class String;

  class Dir : public Object {
  public:
    const static object_type type = DirType;

  private:
    MemoryPointer* data_; // slot

  public:
    /* accessors */

    attr_accessor(data, MemoryPointer);

    /* interface */

    static void init(STATE);
    static Dir* create(STATE);
    // Ruby.primitive :dir_open
    Object* open(STATE, String *path);
    // Ruby.primitive :dir_close
    Object* close(STATE);
    // Ruby.primitive :dir_closed_p
    Object* closed_p(STATE);
    // Ruby.primitive :dir_read
    Object* read(STATE);
    // Ruby.primitive :dir_control
    Object* control(STATE, Fixnum* kind, Integer* pos);
    void guard(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
