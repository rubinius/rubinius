#ifndef RBX_BUILTIN_EXCEPTION_HPP
#define RBX_BUILTIN_EXCEPTION_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Class;
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

    static Exception* make_exception(STATE, Class* exc_class, const char* message);
    static Exception* make_type_error(STATE, object_type type, OBJECT object,
                           const char* reason = NULL);

    static void argument_error(STATE, int expected, int given);
    static void argument_error(STATE, const char* reason);
    static void type_error(STATE, const char* reason);
    static void type_error(STATE, object_type type, OBJECT object,
                           const char* reason = NULL);
    static void float_domain_error(STATE, const char* reason = NULL);
    static void zero_division_error(STATE, const char* reason = NULL);
    static void io_error(STATE, const char* reason);

    static void assertion_error(STATE, const char* reason = NULL);
    static void object_bounds_exceeded_error(STATE, OBJECT obj, size_t index);

    static void errno_error(STATE, const char* reason = NULL, int ern = 0);

    /**
     * Convenience predicates for checking the class of an
     * exception instance. These are provided as a way around
     * creating a bunch more C++ builtin classes and globals
     * and because the exception hierarchy (e.g. FloatDomainError)
     * isn't always consistent.
     */
    static bool argument_error_p(STATE, Exception* exc);
    static bool type_error_p(STATE, Exception* exc);
    static bool zero_division_error_p(STATE, Exception* exc);
    static bool float_domain_error_p(STATE, Exception* exc);
    static bool assertion_error_p(STATE, Exception* exc);
    static bool object_bounds_exceeded_error_p(STATE, Exception* exc);
    static bool errno_error_p(STATE, Exception* exc);
    static bool io_error_p(STATE, Exception* exc);

    static Class* get_argument_error(STATE);
    static Class* get_type_error(STATE);
    static Class* get_zero_division_error(STATE);
    static Class* get_float_domain_error(STATE);
    static Class* get_assertion_error(STATE);
    static Class* get_object_bounds_exceeded_error(STATE);
    static Class* get_errno_error(STATE, int ern);
    static Class* get_io_error(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
