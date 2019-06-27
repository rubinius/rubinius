#ifndef RBX_BUILTIN_EXCEPTION_HPP
#define RBX_BUILTIN_EXCEPTION_HPP

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/object.hpp"

namespace rubinius {
  class Class;
  class Array;
  class CompiledCode;
  class Arguments;

  class Exception : public Object {
  public:
    const static object_type type = ExceptionType;

    attr_accessor(reason_message, Object);
    attr_accessor(locations, Array);
    attr_accessor(cause, Exception);
    attr_accessor(backtrace, Object);
    attr_accessor(custom_backtrace, Object);

    static void bootstrap(STATE);
    static void initialize(STATE, Exception* obj) {
      obj->reason_message(nil<Object>());
      obj->locations(nil<Array>());
      obj->cause(nil<Exception>());
      obj->backtrace(nil<Object>());
      obj->custom_backtrace(nil<Object>());
    }

    static Exception* create(STATE);

    // Rubinius.primitive :exception_allocate
    static Exception* allocate(STATE, Object* self);

    void print_locations(STATE);

    const char* message_c_str(STATE);

    static Exception* make_exception(STATE, Class* exc_class, const char* message);
    static Exception* make_type_error(STATE, object_type type, Object* object,
                                      const char* reason = NULL);
    static Exception* make_errno_exception(STATE, Class* exc_class, Object* reason, Object* loc);

    static Exception* make_argument_error(STATE, int expected, int given, const char* name=0);
    static Exception* make_encoding_compatibility_error(STATE, Object* a, Object* b);
    static Exception* make_frozen_exception(STATE, Object* obj);
    static Exception* make_no_method_error(STATE, Arguments& args);
    static Exception* make_interpreter_error(STATE, const char* reason);
    static Exception* make_deadlock_error(STATE, const char* reason);

    NORETURN(static void raise_argument_error(STATE, int expected, int given));
    NORETURN(static void raise_argument_error(STATE, const char* reason));
    NORETURN(static void raise_regexp_error(STATE, const char* reason));
    NORETURN(static void raise_type_error(STATE, const char* reason));
    NORETURN(static void raise_type_error(STATE, object_type type, Object* object,
                           const char* reason = NULL));
    NORETURN(static void raise_float_domain_error(STATE, const char* reason = NULL));
    NORETURN(static void raise_zero_division_error(STATE, const char* reason = NULL));
    NORETURN(static void raise_io_error(STATE, const char* reason));
    NORETURN(static void raise_range_error(STATE, const char* reason));
    NORETURN(static void raise_runtime_error(STATE, const char* reason));
    NORETURN(static void raise_concurrent_update_error(STATE, const char* reason));

    NORETURN(static void raise_assertion_error(STATE, const char* reason = NULL));
    NORETURN(static void raise_object_bounds_exceeded_error(STATE, Object* obj, int index));
    NORETURN(static void raise_object_bounds_exceeded_error(STATE, const char* reason));

    /** Raise a SystemCallError with given message. */
    NORETURN(static void raise_system_call_error(STATE, const char* reason));
    NORETURN(static void raise_system_call_error(STATE, const std::string& reason));

    NORETURN(static void bytecode_error(STATE, CompiledCode* code,
          int ip, const char* reason));
    NORETURN(static void raise_thread_error(STATE, const char* reason));
    NORETURN(static void raise_fiber_error(STATE, const char* reason));
    NORETURN(static void raise_memory_error(STATE));
    NORETURN(static void raise_frozen_error(STATE, Object* obj));
    NORETURN(static void raise_no_method_error(STATE, Arguments& args));
    NORETURN(static void raise_encoding_compatibility_error(STATE, Object* a, Object* b));
    NORETURN(static void raise_not_implemented_error(STATE, const char* feature));
    NORETURN(static void raise_errno_wait_readable(STATE, int error));
    NORETURN(static void raise_errno_wait_writable(STATE, int error));
    NORETURN(static void raise_errno_error(STATE, const char* reason = NULL,
          int ern = 0, const char* entity = 0));
    NORETURN(static void raise_deadlock_error(STATE, const char* reason));

    static Exception* make_lje(STATE);

    static void type_error(STATE, const char* reason);
    static void internal_error(STATE, const char* reason);
    static void frozen_error(STATE, Object* obj);
    static void interpreter_error(STATE, const char* reason);
    static void deadlock_error(STATE, const char* reason);

    static void encoding_compatibility_error(STATE, Object* a, Object* b);

    // Rubinius.primitive :exception_errno_error
    static Object* errno_error(STATE, Object* reason, Fixnum* ern, Object* loc);

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
    static bool system_call_error_p(STATE, Exception* exc);
    static bool io_error_p(STATE, Exception* exc);
    static bool runtime_error_p(STATE, Exception* exc);

    static Class* get_argument_error(STATE);
    static Class* get_type_error(STATE);
    static Class* get_zero_division_error(STATE);
    static Class* get_float_domain_error(STATE);
    static Class* get_range_error(STATE);
    static Class* get_assertion_error(STATE);
    static Class* get_object_bounds_exceeded_error(STATE);
    static Class* get_io_error(STATE);
    static Class* get_system_call_error(STATE);
    static Class* get_thread_error(STATE);
    static Class* get_fiber_error(STATE);
    static Class* get_errno_error(STATE, Fixnum* ern);
    static Class* get_runtime_error(STATE);
    static Class* get_concurrent_update_error(STATE);
    static Class* get_encoding_compatibility_error(STATE);
    static Class* get_not_implemented_error(STATE);
    static Class* get_no_method_error(STATE);
    static Class* get_interpreter_error(STATE);
    static Class* get_deadlock_error(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };

    friend class Info;
  };
};

#endif
