#ifndef RBX_BUILTIN_DIR_HPP
#define RBX_BUILTIN_DIR_HPP

namespace rubinius {
  class Dir : public Object {
    public:
    const static size_t fields = 2;
    const static object_type type = DirType;

    OBJECT __ivars__; // slot
    MemoryPointer* data; // slot

    static Dir* create(STATE);
    // Ruby.primitive :dir_open
    OBJECT open(STATE, String *path);
    // Ruby.primitive :dir_close
    OBJECT close(STATE);
    // Ruby.primitive :dir_closed_p
    OBJECT closed_p(STATE);
    // Ruby.primitive :dir_read
    OBJECT read(STATE);
    // Ruby.primitive :dir_control
    OBJECT control(STATE, FIXNUM kind, INTEGER pos);
    void guard(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
