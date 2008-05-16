#ifndef RBX_BUILTIN_EXCEPTION_HPP
#define RBX_BUILTIN_EXCEPTION_HPP

namespace rubinius {
  class Exception : public BuiltinType {
  public:
    const static size_t fields = 3;
    const static object_type type = ExceptionType;

    static Exception* create(STATE);
    OBJECT __ivars__; // slot
    OBJECT message; // slot
    OBJECT context; // slot

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
