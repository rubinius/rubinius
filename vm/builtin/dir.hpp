#ifndef RBX_BUILTIN_DIR_HPP
#define RBX_BUILTIN_DIR_HPP

#include "builtin/object.hpp"
#include <dirent.h>

namespace rubinius {
  class String;

  class Dir : public Object {
  public:
    const static object_type type = DirType;

  private:
    DIR *os_;
    String *path_; // slot

  public:

    attr_accessor(path, String);

    /* interface */

    static void init(STATE);

    static Dir* create(STATE);

    static void finalize(STATE, Dir* dir);

    // Rubinius.primitive :dir_allocate
    static Dir* allocate(STATE, Object* self);

    // Rubinius.primitive :dir_open
    Object* open(STATE, String *path);

    // Rubinius.primitive :dir_close
    Object* close(STATE);

    // Rubinius.primitive+ :dir_closed_p
    Object* closed_p(STATE);

    // Rubinius.primitive :dir_read
    Object* read(STATE);

    // Rubinius.primitive :dir_control
    Object* control(STATE, Fixnum* kind, Integer* pos);
    void guard(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
