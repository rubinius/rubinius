#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/location.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "version.h"

#include <sstream>

namespace rubinius {
  void Exception::init(STATE) {
    GO(exception).set(ontology::new_class(state, "Exception", G(object)));
    G(exception)->set_object_type(state, ExceptionType);
  }

  Exception* Exception::create(STATE) {
    return state->new_object<Exception>(G(exception));
  }

  void Exception::print_locations(STATE) {
    for(native_int i = 0; i < locations_->size(); i++) {
      if(Location* loc = try_as<Location>(locations_->get(state, i))) {
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
    if(message()->nil_p()) return "<no message>";

    if(String* str = try_as<String>(message())) {
      return str->c_str(state);
    }

    return "<non-String Exception message>";
  }

  Exception* Exception::make_exception(STATE, Class* exc_class, const char* message) {
    Exception* exc = state->new_object<Exception>(exc_class);

    exc->message(state, String::create(state, message));

    return exc;
  }

  Exception* Exception::make_argument_error(STATE, int expected, int given, Symbol* meth) {
    Exception* exc = state->new_object<Exception>(G(exc_arg));
    exc->set_ivar(state, state->symbol("@given"), Fixnum::from(given));
    exc->set_ivar(state, state->symbol("@expected"), Fixnum::from(expected));
    if(meth) {
      exc->set_ivar(state, state->symbol("@method_name"), meth);
    }
    return exc;
  }

  Exception* Exception::make_lje(STATE, CallFrame* call_frame) {
    Exception* exc = Exception::make_exception(state, G(jump_error), "no block given");
    exc->locations(state, Location::from_call_stack(state, call_frame));
    return exc;
  }

  void Exception::internal_error(STATE, CallFrame* call_frame, const char* reason) {
    Exception* exc = Exception::make_exception(state, G(exc_vm_internal), reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));
    state->raise_exception(exc);
  }

  void Exception::bytecode_error(STATE, CallFrame* call_frame,
                                 CompiledCode* code, int ip, const char* reason)
  {
    Exception* exc = Exception::make_exception(state, G(exc_vm_bad_bytecode), reason);
    exc->set_ivar(state, state->symbol("@compiled_code"), code);
    exc->set_ivar(state, state->symbol("@ip"), Fixnum::from(ip));
    exc->locations(state, Location::from_call_stack(state, call_frame));
    state->raise_exception(exc);
  }

  void Exception::frozen_error(STATE, CallFrame* call_frame) {
    Class* klass;
    if(LANGUAGE_18_ENABLED) {
      klass = G(exc_type);
    } else {
      klass = G(exc_rte);
    }

    Exception* exc = Exception::make_exception(state, klass,
                        "unable to modify frozen object");
    exc->locations(state, Location::from_call_stack(state, call_frame));
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

  void Exception::encoding_compatibility_error(STATE, Object* a, Object* b,
                                               CallFrame* call_frame)
  {
    Exception* exc = Exception::make_encoding_compatibility_error(state, a, b);
    exc->locations(state, Location::from_call_stack(state, call_frame));
    state->raise_exception(exc);
  }

  void Exception::encoding_compatibility_error(STATE, Object* a, Object* b) {
    RubyException::raise(make_encoding_compatibility_error(state, a, b));
  }

  void Exception::argument_error(STATE, int expected, int given) {
    std::ostringstream msg;

    msg << "given " << given << ", expected " << expected;

    argument_error(state, msg.str().c_str());
  }

  void Exception::argument_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_argument_error(state), reason));
  }

  Exception* Exception::make_type_error(STATE, object_type type, Object* object, const char* reason) {
    if(reason) {
      return make_exception(state, get_type_error(state), reason);
    }

    std::ostringstream msg;

    TypeInfo* wanted = state->vm()->find_type(type);

    if(!object->reference_p()) {
      msg << "Tried to use non-reference value " << object;
    } else {
      TypeInfo* was = state->vm()->find_type(object->type_id());
      msg << "Tried to use object of type " <<
        was->type_name << " (" << was->type << ")";
    }

    msg << " as type " << wanted->type_name << " (" << wanted->type << ")";

    return make_exception(state, get_type_error(state), msg.str().c_str());
  }

  void Exception::regexp_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, G(exc_rex), reason));
  }

  void Exception::type_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_type_error(state), reason));
  }

  void Exception::type_error(STATE, object_type type, Object* object,
                                   const char* reason) {
    // We code for TypeError being raised defensively, so it's ok, to raise it
    // since someone close by will catch it and propogate it back to ruby
    // properly.
    throw TypeError(type, object, reason);
  }

  void Exception::type_error(STATE, const char* reason, CallFrame* call_frame) {
    Exception* exc = Exception::make_exception(state, G(exc_type), reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));
    state->raise_exception(exc);
  }

  void Exception::float_domain_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_float_domain_error(state), reason));
  }

  void Exception::range_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_range_error(state), reason));
  }

  void Exception::zero_division_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_zero_division_error(state), reason));
  }

  void Exception::assertion_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_assertion_error(state), reason));
  }

  void Exception::system_call_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_system_call_error(state), reason));
  }

  void Exception::system_call_error(STATE, const std::string& reason) {
    system_call_error(state, reason.c_str());
  }

  void Exception::thread_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_thread_error(state), reason));
  }

  void Exception::fiber_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_fiber_error(state), reason));
  }

  void Exception::memory_error(STATE) {
    char buf[RBX_STRERROR_BUFSIZE];
    char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
    RubyException::raise(make_exception(state, get_errno_error(state, Fixnum::from(errno)), err));
  }

  void Exception::object_bounds_exceeded_error(STATE, Object* obj, int index) {
    TypeInfo* info = state->vm()->find_type(obj->type_id()); // HACK use object
    std::ostringstream msg;

    msg << "Bounds of object exceeded:" << std::endl;
    msg << "      type: " << info->type_name << ", bytes: " <<
           obj->body_in_bytes(state->vm()) << ", accessed: " << index << std::endl;

    RubyException::raise(make_exception(state, get_object_bounds_exceeded_error(state),
                                        msg.str().c_str()));
  }

  void Exception::object_bounds_exceeded_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state,
          get_object_bounds_exceeded_error(state), reason));
  }

  Exception* Exception::make_errno_exception(STATE, Class* exc_class, Object* reason) {
    Exception* exc = state->new_object<Exception>(exc_class);

    String* message = force_as<String>(reason);
    if(String* str = try_as<String>(exc_class->get_const(state, "Strerror"))) {
      str = str->string_dup(state);
      if(String* r = try_as<String>(reason)) {
        str->append(state, " - ");
        message = str->append(state, r);
      } else {
        message = str;
      }
    }
    exc->message(state, message);

    exc->set_ivar(state, state->symbol("@errno"),
                  exc_class->get_const(state, "Errno"));

    return exc;
  }

  /* exception_errno_error primitive */
  Object* Exception::errno_error(STATE, Object* reason, Fixnum* ern) {
    Class* exc_class = get_errno_error(state, ern);
    if(exc_class->nil_p()) return cNil;

    return make_errno_exception(state, exc_class, reason);
  }

  void Exception::errno_error(STATE, const char* reason, int ern, const char* entity) {
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

      exc = make_errno_exception(state, exc_class, message);
    }

    RubyException::raise(exc);
  }

  void Exception::errno_eagain_error(STATE, const char* reason) {
    Exception* exc;
    Class* exc_class;

    if(LANGUAGE_18_ENABLED) {
      exc_class = get_errno_error(state, Fixnum::from(EAGAIN));
    } else {
      exc_class = as<Class>(G(io)->get_const(state, "EAGAINWaitReadable"));
    }

    String* message = nil<String>();

    if(reason) {
      message = String::create(state, reason);
    }

    exc = make_errno_exception(state, exc_class, message);

    RubyException::raise(exc);
  }

  void Exception::io_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_io_error(state), reason));
  }

  void Exception::runtime_error(STATE, const char* reason) {
    RubyException::raise(make_exception(state, get_runtime_error(state), reason));
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

  Class* Exception::get_encoding_compatibility_error(STATE) {
    return as<Class>(G(encoding)->get_const(state, "CompatibilityError"));
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

  void Exception::Info::show(STATE, Object* self, int level) {
    Exception* exc = as<Exception>(self);

    class_header(state, self);
    indent_attribute(++level, "message"); exc->message()->show(state, level);
    indent_attribute(level, "locations"); exc->locations()->show_simple(state, level);
    close_body(level);
  }
}
