#ifndef RBX_BUILTIN_EXCEPTION_HPP
#define RBX_BUILTIN_EXCEPTION_HPP

#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "type_info.hpp"

namespace rubinius {
  class MethodContext;

  class Exception : public Object {
  public:
    const static size_t fields = 2;
    const static object_type type = ExceptionType;

    static Exception* create(STATE);
    String* message; // slot
    MethodContext* context; // slot

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
