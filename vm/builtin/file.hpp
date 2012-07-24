#ifndef RBX_BUILTIN_FILE_HPP
#define RBX_BUILTIN_FILE_HPP

#include "builtin/object.hpp"

namespace rubinius {

  class File: public Object {
  public:
    const static object_type type = FileType;

    // Rubinius.primitive :file_lchmod19
    static Object* lchmod19(STATE, Fixnum* mode, Arguments& args);

    // Rubinius.primitive :file_is_lchmod_available
    static Object* is_lchmod_available(STATE);

  };
};

#endif
