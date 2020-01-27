#include "memory.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"

#include "class/class.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/lookup_table.hpp"
#include "class/fixnum.hpp"
#include "class/symbol.hpp"
#include "class/string.hpp"

#include <sstream>

namespace rubinius {
  void Exception::bootstrap(STATE) {
    GO(exception).set(state->memory()->new_class<Class, Exception>(state, "Exception"));
  }

  Exception* Exception::allocate(STATE, Object* self) {
    return state->memory()->new_object<Exception>(state, as<Class>(self));
  }

  Exception* Exception::create(STATE) {
    return state->memory()->new_object<Exception>(state, G(exception));
  }

  void Exception::print_locations(STATE) {
    if(locations()->nil_p()) return;

    for(intptr_t i = 0; i < locations()->size(); i++) {
      if(Location* loc = try_as<Location>(locations()->get(state, i))) {
        if(CompiledCode* meth = try_as<CompiledCode>(loc->method())) {
          if(Symbol* file_sym = try_as<Symbol>(meth->file())) {
            std::cout << file_sym->debug_str(state) << ":"
                      << meth->line(state, loc->ip()->to_native())
                      << "\n";
            continue;
          }
        }
        std::cout << "<unknown>";
      } else {
        std::cout << "Bad locations entry detected.\n";
      }
    }
  }

  const char* Exception::message_c_str(STATE) {
    if(reason_message()->nil_p()) return "<no message>";

    if(String* str = try_as<String>(reason_message())) {
      return str->c_str(state);
    }

    return "<non-String Exception message>";
  }

  Exception* Exception::make_exception(STATE, Class* exc_class, const char* message) {
    Exception* exc = state->memory()->new_object<Exception>(state, exc_class);

    exc->reason_message(state, String::create(state, message));

    return exc;
  }

  Exception* Exception::make_argument_error(STATE, int expected, int given, const char* meth) {
    Exception* exc = state->memory()->new_object<Exception>(state, G(exc_arg));
    exc->set_ivar(state, state->symbol("@given"), Fixnum::from(given));
    exc->set_ivar(state, state->symbol("@expected"), Fixnum::from(expected));
    if(meth) {
      exc->set_ivar(state, state->symbol("@method_name"), state->symbol(meth));
    }
    return exc;
  }

  Exception* Exception::make_lje(STATE) {
    Exception* exc = Exception::make_exception(state, G(jump_error), "no block given");
    exc->locations(state, Location::from_call_stack(state));
    return exc;
  }

  void Exception::internal_error(STATE, const char* reason) {
    Exception* exc = Exception::make_exception(state, G(exc_vm_internal), reason);
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
  }

  void Exception::bytecode_error(STATE, CompiledCode* code, int ip, const char* reason) {
    std::ostringstream msg;
    msg << reason;
    msg << ": code: " << code->name()->cpp_str(state);
    msg << ", ip: " << ip;

    Exception* exc = Exception::make_exception(state,
        G(exc_vm_bad_bytecode), msg.str().c_str());
    exc->set_ivar(state, state->symbol("@compiled_code"), code);
    exc->set_ivar(state, state->symbol("@ip"), Fixnum::from(ip));
    exc->locations(state, Location::from_call_stack(state));
    RubyException::raise(exc);
  }

  Exception* Exception::make_no_method_error(STATE, Arguments& args) {
    std::ostringstream msg;
    msg << "undefined method `" << args.name()->cpp_str(state)
      << "' for " << args.recv()->to_s(state)->c_str(state);

    return Exception::make_exception(state,
        get_no_method_error(state), msg.str().c_str());
  }

  void Exception::raise_no_method_error(STATE, Arguments& args) {
    RubyException::raise(Exception::make_no_method_error(state, args), true);
  }

  Exception* Exception::make_interpreter_error(STATE, const char* reason) {
    return Exception::make_exception(state,
        get_interpreter_error(state), reason);
  }

  void Exception::interpreter_error(STATE, const char* reason) {
    Exception* exc = Exception::make_interpreter_error(state, reason);
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
  }

  Exception* Exception::make_deadlock_error(STATE, const char* reason) {
    return Exception::make_exception(state,
        get_deadlock_error(state), reason);
  }

  void Exception::deadlock_error(STATE, const char* reason) {
    Exception* exc = Exception::make_deadlock_error(state, reason);
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
  }

  void Exception::raise_deadlock_error(STATE, const char* reason) {
    RubyException::raise(Exception::make_deadlock_error(state, reason), true);
  }

  Exception* Exception::make_frozen_exception(STATE, Object* obj) {
    std::ostringstream msg;
    msg << "can't modify frozen ";
    msg << obj->class_object(state)->module_name()->debug_str(state);

    return Exception::make_exception(state, G(exc_rte), msg.str().c_str());
  }

  void Exception::raise_frozen_error(STATE, Object* obj) {
    RubyException::raise(Exception::make_frozen_exception(state, obj), true);
  }

  void Exception::frozen_error(STATE, Object* obj) {
    Exception* exc = Exception::make_frozen_exception(state, obj);
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
  }

  Exception* Exception::make_encoding_compatibility_error(STATE, Object* a, Object* b) {
    Encoding* enc_a = Encoding::get_object_encoding(state, a);
    Encoding* enc_b = Encoding::get_object_encoding(state, b);

    std::ostringstream msg;
    msg << "undefined conversion ";

    if(String* str = try_as<String>(a)) {
      msg << "for '" << str->c_str(state) << "' ";
    }

    msg << "from " << enc_a->name()->c_str(state);
    msg << " to " << enc_b->name()->c_str(state);

    return make_exception(state, get_encoding_compatibility_error(state),
                          msg.str().c_str());
  }

  void Exception::encoding_compatibility_error(STATE, Object* a, Object* b) {
    Exception* exc = Exception::make_encoding_compatibility_error(state, a, b);
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
  }

  void Exception::raise_encoding_compatibility_error(STATE, Object* a, Object* b) {
    RubyException::raise(make_encoding_compatibility_error(state, a, b));
  }

  void Exception::raise_argument_error(STATE, int expected, int given) {
    std::ostringstream msg;

    msg << "given " << given << ", expected " << expected;

    raise_argument_error(state, msg.str().c_str());
  }

  void Exception::raise_argument_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_argument_error(state), reason));
  }

  Exception* Exception::make_type_error(STATE, object_type type, Object* object, const char* reason) {
    if(reason) {
      return make_exception(state, get_type_error(state), reason);
    }

    std::ostringstream msg;

    TypeInfo* wanted = state->memory()->find_type(type);

    if(!object->reference_p()) {
      msg << "Tried to use non-reference value " << object;
    } else {
      TypeInfo* was = state->memory()->find_type(object->type_id());
      msg << "Tried to use object of type " <<
        was->type_name << " (" << was->type << ")";
    }

    msg << " as type " << wanted->type_name << " (" << wanted->type << ")";

    return make_exception(state, get_type_error(state), msg.str().c_str());
  }

  void Exception::raise_regexp_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, G(exc_rex), reason));
  }

  void Exception::raise_type_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_type_error(state), reason));
  }

  void Exception::raise_type_error(STATE, object_type type, Object* object,
                                   const char* reason) {
    // We code for TypeError being raised defensively, so it's ok, to raise it
    // since someone close by will catch it and propogate it back to ruby
    // properly.
    throw TypeError(type, object, reason);
  }

  void Exception::type_error(STATE, const char* reason) {
    Exception* exc = Exception::make_exception(state, G(exc_type), reason);
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
  }

  void Exception::raise_float_domain_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_float_domain_error(state), reason));
  }

  void Exception::raise_range_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_range_error(state), reason));
  }

  void Exception::raise_zero_division_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_zero_division_error(state), reason));
  }

  void Exception::raise_assertion_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_assertion_error(state), reason));
  }

  void Exception::raise_system_call_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_system_call_error(state), reason));
  }

  void Exception::raise_system_call_error(STATE, const std::string& reason) {
    raise_system_call_error(state, reason.c_str());
  }

  void Exception::raise_thread_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_thread_error(state), reason));
  }

  void Exception::raise_fiber_error(STATE, const char* reason) {
    Exception* exc = make_exception(state, get_fiber_error(state), reason);
    exc->locations(state, Location::from_call_stack(state, 0));

    RubyException::raise(exc);
  }

  void Exception::raise_memory_error(STATE) {
    char buf[RBX_STRERROR_BUFSIZE];
    char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
    RubyException::raise(make_exception(state, get_errno_error(state, Fixnum::from(errno)), err));
  }

  void Exception::raise_not_implemented_error(STATE, const char* feature) {
    RubyException::raise(make_exception(state, get_not_implemented_error(state), feature));
  }

  void Exception::raise_object_bounds_exceeded_error(STATE, Object* obj, int index) {
    TypeInfo* info = state->memory()->find_type(obj->type_id()); // HACK use object
    std::ostringstream msg;

    msg << "Bounds of object exceeded:" << std::endl;
    msg << "      type: " << info->type_name << ", bytes: " <<
           obj->body_in_bytes(state) << ", accessed: " << index << std::endl;

    RubyException::raise(make_exception(state, get_object_bounds_exceeded_error(state),
                                        msg.str().c_str()));
  }

  void Exception::raise_object_bounds_exceeded_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state,
          get_object_bounds_exceeded_error(state), reason));
  }

  Exception* Exception::make_errno_exception(STATE, Class* exc_class, Object* reason, Object* loc) {
    Exception* exc = state->memory()->new_object<Exception>(state, exc_class);

    String* message = force_as<String>(reason);
    if(String* str = try_as<String>(exc_class->get_const(state, "Strerror"))) {
      str = str->string_dup(state);
      if(String* l = try_as<String>(loc)) {
        str->append(state, " @ ");
        str->append(state, l);
        message = str;
      }
      if(String* r = try_as<String>(reason)) {
        str->append(state, " - ");
        str->append(state, r);
        message = str;
      }
      message = str;
    }
    exc->reason_message(state, message);

    exc->set_ivar(state, state->symbol("@errno"),
                  exc_class->get_const(state, "Errno"));

    return exc;
  }

  /* exception_errno_error primitive */
  Object* Exception::errno_error(STATE, Object* reason, Fixnum* ern, Object* loc) {
    Class* exc_class = get_errno_error(state, ern);
    if(exc_class->nil_p()) return cNil;

    return make_errno_exception(state, exc_class, reason, loc);
  }

  void Exception::raise_errno_error(STATE, const char* reason, int ern, const char* entity) {
    Exception* exc;

    if(ern == 0) ern = errno;
    Class* exc_class = get_errno_error(state, Fixnum::from(ern));

    if(exc_class->nil_p()) {
      std::ostringstream msg;
      msg << "Unknown errno ";
      if(reason) msg << ": " << reason;
      if(entity) msg << " - " << entity;
      exc = make_exception(state, get_system_call_error(state), msg.str().c_str());
    } else {
      String* message = nil<String>();

      if(reason) {
        std::ostringstream msg;
        msg << reason;
        if(entity) msg << " - " << entity;

        message = String::create(state, msg.str().c_str(), msg.str().size());
      }

      exc = make_errno_exception(state, exc_class, message, cNil);
    }

    RubyException::raise(exc);
  }

  void Exception::raise_errno_wait_readable(STATE, int error) {
    Exception* exc;
    Class* exc_class;

    if(error == EAGAIN) {
      exc_class = as<Class>(G(io)->get_const(state, "EAGAINWaitReadable"));
    }
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
    else if(error == EWOULDBLOCK) {
      exc_class = as<Class>(G(io)->get_const(state, "EWOULDBLOCKWaitReadable"));
    }
#endif
    else {
      exc_class = get_errno_error(state, Fixnum::from(error));
    }

    String* message = String::create(state, "read would block");

    exc = make_errno_exception(state, exc_class, message, cNil);

    RubyException::raise(exc);
  }

  void Exception::raise_errno_wait_writable(STATE, int error) {
    Exception* exc;
    Class* exc_class;

    if(error == EAGAIN) {
      exc_class = as<Class>(G(io)->get_const(state, "EAGAINWaitWritable"));
    }
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
    else if(error == EWOULDBLOCK) {
      exc_class = as<Class>(G(io)->get_const(state, "EWOULDBLOCKWaitWritable"));
    }
#endif
    else {
      exc_class = get_errno_error(state, Fixnum::from(error));
    }

    String* message = String::create(state, "write would block");

    exc = make_errno_exception(state, exc_class, message, cNil);

    RubyException::raise(exc);
  }

  void Exception::raise_io_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_io_error(state), reason));
  }

  void Exception::raise_runtime_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_runtime_error(state), reason));
  }

  void Exception::raise_concurrent_update_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state,
          get_concurrent_update_error(state), reason));
  }

  bool Exception::argument_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_argument_error(state));
  }

  bool Exception::type_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_type_error(state));
  }

  bool Exception::zero_division_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_zero_division_error(state));
  }

  bool Exception::float_domain_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_float_domain_error(state));
  }

  bool Exception::assertion_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_assertion_error(state));
  }

  bool Exception::object_bounds_exceeded_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_object_bounds_exceeded_error(state));
  }

  bool Exception::errno_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_system_call_error(state));
  }

  bool Exception::system_call_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_system_call_error(state));
  }

  bool Exception::io_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_io_error(state));
  }

  bool Exception::runtime_error_p(STATE, Exception* exc) {
    return exc->kind_of_p(state, get_runtime_error(state));
  }

  Class* Exception::get_argument_error(STATE) {
    return G(exc_arg);
  }

  Class* Exception::get_type_error(STATE) {
    return G(exc_type);
  }

  Class* Exception::get_zero_division_error(STATE) {
    return as<Class>(G(object)->get_const(state, "ZeroDivisionError"));
  }

  Class* Exception::get_float_domain_error(STATE) {
    return as<Class>(G(object)->get_const(state, "FloatDomainError"));
  }

  Class* Exception::get_range_error(STATE) {
    return as<Class>(G(object)->get_const(state, "RangeError"));
  }

  Class* Exception::get_assertion_error(STATE) {
    return as<Class>(G(rubinius)->get_const(state, "AssertionError"));
  }

  Class* Exception::get_object_bounds_exceeded_error(STATE) {
    return as<Class>(G(rubinius)->get_const(state, "ObjectBoundsExceededError"));
  }

  Class* Exception::get_system_call_error(STATE) {
    return as<Class>(G(object)->get_const(state, "SystemCallError"));
  }

  Class* Exception::get_thread_error(STATE) {
    return as<Class>(G(object)->get_const(state, "ThreadError"));
  }

  Class* Exception::get_fiber_error(STATE) {
    return as<Class>(G(object)->get_const(state, "FiberError"));
  }

  Class* Exception::get_runtime_error(STATE) {
    return as<Class>(G(object)->get_const(state, "RuntimeError"));
  }

  Class* Exception::get_concurrent_update_error(STATE) {
    return G(exc_cue);
  }

  Class* Exception::get_encoding_compatibility_error(STATE) {
    return as<Class>(G(encoding)->get_const(state, "CompatibilityError"));
  }


  Class* Exception::get_not_implemented_error(STATE) {
    return as<Class>(G(object)->get_const(state, "NotImplementedError"));
  }

  Class* Exception::get_no_method_error(STATE) {
    return as<Class>(G(object)->get_const(state, "NoMethodError"));
  }

  Class* Exception::get_errno_error(STATE, Fixnum* ern) {
    if(Class* cls = try_as<Class>(G(errno_mapping)->fetch(state, ern))) {
      return cls;
    }

    return nil<Class>();
  }

  Class* Exception::get_io_error(STATE) {
    return as<Class>(G(object)->get_const(state, "IOError"));
  }

  Class* Exception::get_interpreter_error(STATE) {
    return as<Class>(G(object)->get_const(state, "InterpreterError"));
  }

  Class* Exception::get_deadlock_error(STATE) {
    return as<Class>(G(object)->get_const(state, "DeadlockError"));
  }

  void Exception::Info::show(STATE, Object* self, int level) {
    Exception* exc = as<Exception>(self);

    class_header(state, self);
    indent_attribute(++level, "message"); exc->reason_message()->show(state, level);
    indent_attribute(level, "locations"); exc->locations()->show_simple(state, level);
    close_body(level);
  }
}
