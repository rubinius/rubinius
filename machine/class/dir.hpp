#ifndef RBX_BUILTIN_DIR_HPP
#define RBX_BUILTIN_DIR_HPP

#include "object_utils.hpp"
#include "spinlock.hpp"

#include "class/encoding.hpp"
#include "class/object.hpp"
#include "class/string.hpp"

#include <dirent.h>

namespace rubinius {
  class String;
  class Encoding;

  class Dir : public Object {
    static rubinius::locks::spinlock_mutex readdir_lock_;

  public:
    const static object_type type = DirType;

  private:
    attr_field(os, DIR*);

  public:
    attr_accessor(path, String);
    attr_accessor(encoding, Encoding);

    static void bootstrap(STATE);
    static void initialize(STATE, Dir* obj) {
      obj->os(NULL);
      obj->path(nil<String>());
      obj->encoding(nil<Encoding>());
    }

    static Dir* create(STATE);

    static void finalize(STATE, Dir* dir);

    // Rubinius.primitive :dir_allocate
    static Dir* allocate(STATE, Object* self);

    // Rubinius.primitive :dir_open
    Object* open(STATE, String *path, Object* enc);

    // Rubinius.primitive :dir_fileno
    Object* fileno(STATE);

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
