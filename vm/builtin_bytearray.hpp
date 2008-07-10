#ifndef RBX_BUILTIN_BYTEARRAY_HPP
#define RBX_BUILTIN_BYTEARRAY_HPP

namespace rubinius {
  class ByteArray : public Object {
    public:
    const static size_t fields = 0;
    const static object_type type = ByteArrayType;

    static ByteArray* create(STATE, size_t bytes);

    class Info : public TypeInfo {
    public:
      Info(object_type type): TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
};

#endif
