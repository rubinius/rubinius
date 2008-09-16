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

  private:
    String* message_;        // slot
    MethodContext* context_; // slot

  public:
    /* accessors */

    attr_accessor(message, String);
    attr_accessor(context, MethodContext);

    /* interface */

    static Exception* create(STATE);
    static Exception* Exception::arg_error(STATE, int expected, int given);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
