#include <sstream>
#include <string>

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/module.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/nativemethodcontext.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"

#include "vm/global_cache.hpp"
#include "vm/message.hpp"
#include "vm/object.hpp"
#include "vm/objectmemory.hpp"
#include "vm/quantum_stack_leap.hpp"
#include "vm/vm.hpp"

#include "subtend/ruby.h"



/* Bring names to front where needed. */
using rubinius::Array;
using rubinius::as;
using rubinius::Bignum;
using rubinius::Class;
using rubinius::Fixnum;
using rubinius::Integer;
using rubinius::kind_of;
using rubinius::Message;
using rubinius::Module;
using rubinius::native_int;
using rubinius::NativeMethod;
using rubinius::NativeMethodContext;
using rubinius::Object;
using rubinius::SendSite;
using rubinius::String;
using rubinius::Symbol;
using rubinius::VM;


namespace {

  /**
   *  Common implementation for rb_funcall*
   *
   *  TODO:   Set up permanent SendSites through macroing?
   *  TODO:   Stricter action check?
   */
  static VALUE hidden_funcall_backend(VALUE receiver,
                                      ID method_name,
                                      std::size_t arg_count,
                                      VALUE* arg_array)
  {
    NativeMethodContext* context = NativeMethodContext::current();

    // Create Message.. bit more involved than usual because we bypass some conveniences
    Message& msg = context->message_from_c();

    msg.send_site   = static_cast<SendSite*>(RBX_Qnil);
    msg.name        = reinterpret_cast<Symbol*>(method_name);
    msg.recv        = context->object_from(receiver);
    msg.block       = RBX_Qnil;
    msg.splat       = RBX_Qnil;
    msg.stack       = 0;
    msg.priv        = true;           /* TODO: Double-check this */

    msg.lookup_from = (msg.recv->lookup_begin(context->state()));

    Array* args = Array::create(context->state(), arg_count);

    for (std::size_t i = 0; i < arg_count; ++i) {
      args->set(context->state(), i, context->object_from(arg_array[i]));
    }

    msg.set_arguments(context->state(), args);

    context->action(NativeMethodContext::CALL_FROM_C);
    store_current_execution_point_in(context->inside_c_method_point());
    /* Execution resumes here when returning */

    context = NativeMethodContext::current();

    if (context->action() != NativeMethodContext::RETURNED_BACK_TO_C) {
      jump_to_execution_point_in(context->dispatch_point());
    }

    context->action(NativeMethodContext::ORIGINAL_CALL);

    VALUE ret = context->value_returned_to_c();

    context->value_returned_to_c(RBX_Qnil);
    context->message_from_c().reset();

    return ret;
  }

  /** Make sure the name has the given prefix. */
  static Symbol* prefixed_by(std::string prefix, std::string name) {
    NativeMethodContext* context = NativeMethodContext::current();

    if(name.compare(0UL, prefix.size(), prefix) != 0) {
      std::ostringstream str;
      str << prefix << name;
      name.assign(str.str());
    }

    /* TODO: Need to strdup here to not point to junk but can it leak? */
    return context->state()->symbol(strdup(name.c_str()));
  }

  /** Make sure the name has the given prefix. */
  static Symbol* prefixed_by(std::string prefix, ID name) {
    NativeMethodContext* context = NativeMethodContext::current();

    return prefixed_by(prefix, reinterpret_cast<Symbol*>(name)->c_str(context->state()));
  }

}


/** API */
extern "C" {

  /**
   *  All of the default error class VALUEs are macroed
   *  to an invocation of this function to avoid tracking
   *  the classes on this side.
   *
   *  TODO: Vectorise this or something.
   */
  VALUE rbx_subtend_hidden_error(RbxSubtendHiddenError type) {
    switch (type) {
    case RbxArgumentError:
      return rb_const_get(rb_cObject, rb_intern("ArgumentError"));
    case RbxEOFError:
      return rb_const_get(rb_cObject, rb_intern("EOFError"));
    case RbxErrno:
      return rb_const_get(rb_cObject, rb_intern("Errno"));
    case RbxException:
      return rb_const_get(rb_cObject, rb_intern("Exception"));
    case RbxFatal:
      return rb_const_get(rb_cObject, rb_intern("Fatal"));
    case RbxFloatDomainError:
      return rb_const_get(rb_cObject, rb_intern("FloatDomainError"));
    case RbxIndexError:
      return rb_const_get(rb_cObject, rb_intern("IndexError"));
    case RbxInterrupt:
      return rb_const_get(rb_cObject, rb_intern("Interrupt"));
    case RbxIOError:
      return rb_const_get(rb_cObject, rb_intern("IOError"));
    case RbxLoadError:
      return rb_const_get(rb_cObject, rb_intern("LoadError"));
    case RbxLocalJumpError:
      return rb_const_get(rb_cObject, rb_intern("LocalJumpError"));
    case RbxNameError:
      return rb_const_get(rb_cObject, rb_intern("NameError"));
    case RbxNoMemoryError:
      return rb_const_get(rb_cObject, rb_intern("NoMemoryError"));
    case RbxNoMethodError:
      return rb_const_get(rb_cObject, rb_intern("NoMethodError"));
    case RbxNotImplementedError:
      return rb_const_get(rb_cObject, rb_intern("NotImplementedError"));
    case RbxRangeError:
      return rb_const_get(rb_cObject, rb_intern("RangeError"));
    case RbxRegexpError:
      return rb_const_get(rb_cObject, rb_intern("RegexpError"));
    case RbxRuntimeError:
      return rb_const_get(rb_cObject, rb_intern("RuntimeError"));
    case RbxScriptError:
      return rb_const_get(rb_cObject, rb_intern("ScriptError"));
    case RbxSecurityError:
      return rb_const_get(rb_cObject, rb_intern("SecurityError"));
    case RbxSignalException:
      return rb_const_get(rb_cObject, rb_intern("SignalException"));
    case RbxStandardError:
      return rb_const_get(rb_cObject, rb_intern("StandardError"));
    case RbxSyntaxError:
      return rb_const_get(rb_cObject, rb_intern("SyntaxError"));
    case RbxSystemCallError:
      return rb_const_get(rb_cObject, rb_intern("SystemCallError"));
    case RbxSystemExit:
      return rb_const_get(rb_cObject, rb_intern("SystemExit"));
    case RbxSystemStackError:
      return rb_const_get(rb_cObject, rb_intern("SystemStackError"));
    case RbxTypeError:
      return rb_const_get(rb_cObject, rb_intern("TypeError"));
    case RbxThreadError:
      return rb_const_get(rb_cObject, rb_intern("ThreadError"));
    case RbxZeroDivisionError:
      return rb_const_get(rb_cObject, rb_intern("ZeroDivisionError"));
    default:
      std::runtime_error("rbx_subtend_hidden_error(): Invalid type given!");
      return Qnil;
    }
  }

  /**
   *  All of the default rb_{c,m}* VALUEs are macroed
   *  to an invocation of this function, so we can
   *  avoid the hassle of tracking the objects on
   *  this side.
   */
  VALUE rbx_subtend_hidden_global(RbxSubtendHiddenGlobal type) {
    NativeMethodContext* context = NativeMethodContext::current();

    /* TODO: Move these to static */
    switch (type) {
    case RbxArray:
      return context->handle_for_global(context->state()->globals.array.get());
    case RbxBignum:
      return context->handle_for_global(context->state()->globals.bignum.get());
    case RbxClass:
      return context->handle_for_global(context->state()->globals.klass.get());
    case RbxData:
      return context->handle_for_global(context->state()->globals.data.get());
    case RbxFalse:
      return context->handle_for_global(context->state()->globals.false_class.get());
    case RbxFixnum:
      return context->handle_for_global(context->state()->globals.fixnum_class.get());
    case RbxFloat:
      return context->handle_for_global(context->state()->globals.floatpoint.get());
    /* Hash is not builtin, @see ruby.h */
    case RbxInteger:
      return context->handle_for_global(context->state()->globals.integer.get());
    case RbxIO:
      return context->handle_for_global(context->state()->globals.io.get());
    case RbxModule:
      return context->handle_for_global(context->state()->globals.module.get());
    case RbxNil:
      return context->handle_for_global(context->state()->globals.nil_class.get());
    case RbxObject:
      return context->handle_for_global(context->state()->globals.object.get());
    case RbxRegexp:
      return context->handle_for_global(context->state()->globals.regexp.get());
    case RbxString:
      return context->handle_for_global(context->state()->globals.string.get());
    case RbxSymbol:
      return context->handle_for_global(context->state()->globals.symbol.get());
    case RbxThread:
      return context->handle_for_global(context->state()->globals.thread.get());
    case RbxTrue:
      return context->handle_for_global(context->state()->globals.true_class.get());
    default:
      return Qnil;
    }
  }

  VALUE rbx_subtend_hidden_id2sym(ID id) {
    NativeMethodContext* context = NativeMethodContext::current();

    return context->handle_for(reinterpret_cast<Symbol*>(id));
  }

  int rbx_subtend_hidden_rtest(VALUE expression_result) {
    NativeMethodContext* context = NativeMethodContext::current();

    return RBX_RTEST(context->object_from(expression_result));
  }

  ID rbx_subtend_hidden_sym2id(VALUE symbol_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    return reinterpret_cast<ID>(context->object_from(symbol_handle));
  }

  void rbx_subtend_hidden_define_method(const char* file,
                                        VALUE target,
                                        const char* name,
                                        SubtendGenericFunction fptr,
                                        int arity,
                                        RbxMethodKind kind)
  {
    NativeMethodContext* context = NativeMethodContext::current();

    VM* state = context->state();
    Symbol* method_name = state->symbol(name);

    Module* module = NULL;

    if (kind == RbxSingletonMethod) {
      module = as<Module>(context->object_from(target)->metaclass(context->state()));
    }
    else {
      module = as<Module>(context->object_from(target));
    }


    NativeMethod* method = NULL;         // Fortran FTW.
    method = NativeMethod::create(state,
                                  String::create(state, file),
                                  module,
                                  method_name,
                                  fptr,
                                  Fixnum::from(arity));

    module->method_table()->store(state, method_name, method);
    state->global_cache->clear(module, method_name);
  }


  /** TODO: Handle overflow properly. */
  int FIX2INT(VALUE num_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    native_int original = as<Integer>(context->object_from(num_handle))->to_native();
    int number = original;

    if (original != number) {
      throw std::runtime_error("FIX2INT: Number too big to convert to int!");
    }

    return number;
  }

  VALUE INT2NUM(int number) {
    NativeMethodContext* context = NativeMethodContext::current();

    return context->handle_for(Integer::from(context->state(), number));
  }

  VALUE rb_Array(VALUE obj_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    Object* obj = context->object_from(obj_handle);

    if (kind_of<Array>(obj)) {
      return obj_handle;
    }

    Array* array = Array::create(context->state(), 1);
    array->set(context->state(), 0, obj);

    return context->handle_for(array);
  }

  /* TODO: Check 64-bit? */
  VALUE rb_ary_entry(VALUE self_handle, int index) {
    NativeMethodContext* context = NativeMethodContext::current();

    Array* self = as<Array>(context->object_from(self_handle));

    size_t size = self->size();

    if (size == 0) {
      return Qnil;
    }

    if (index < 0) {
      index += size;
    }

    size_t as_size = index;

    /* No error for out-of-bounds. */
    if (index < 0 || as_size >= size) {
      return Qnil;
    }

    return context->handle_for(self->get(context->state(), as_size));
  }

  size_t rb_ary_size(VALUE self_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    Array* self = as<Array>(context->object_from(self_handle));

    return self->size();
  }

  VALUE rb_attr_get(VALUE obj_handle, ID attr_name) {
    return rb_ivar_get(obj_handle, attr_name);
  }

  int rb_block_given_p() {
    NativeMethodContext* context = NativeMethodContext::current();

    return RBX_RTEST(context->block());
  }

  VALUE rb_const_get(VALUE module_handle, ID name) {
    NativeMethodContext* context = NativeMethodContext::current();

    Module* module = as<Module>(context->object_from(module_handle));

    return context->handle_for(module->get_const(context->state(),
                                                 reinterpret_cast<Symbol*>(name)));
  }

  /** TODO: Raise TypeError if given nil module. */
  void rb_define_const(VALUE module_handle, const char* name, VALUE obj_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    Module* module = as<Module>(context->object_from(module_handle));
    Object* object = context->object_from(obj_handle);

    module->set_const(context->state(), name,  object);
  }

  VALUE rb_define_module(const char* name) {
    return rb_define_module_under(rb_cObject, name);
  }

  VALUE rb_define_module_under(VALUE parent_handle, const char* name) {
    NativeMethodContext* context = NativeMethodContext::current();

    Module* parent = as<Module>(context->object_from(parent_handle));
    Module* module = context->task()->open_module(parent, context->state()->symbol(name));

    return context->handle_for_global(module);
  }

  VALUE rb_funcall(VALUE receiver, ID method_name, int arg_count, ...) {
    va_list varargs;
    va_start(varargs, arg_count);

    VALUE* args = new VALUE[arg_count];

    for (int i = 0; i < arg_count; ++i) {
      args[i] = va_arg(varargs, VALUE);
    }

    va_end(varargs);

    VALUE ret = hidden_funcall_backend(receiver, method_name, arg_count, args);

    delete args;
    return ret;
  }

  VALUE rb_funcall2(VALUE receiver, ID method_name, int arg_count, VALUE* args) {
    return hidden_funcall_backend(receiver, method_name, arg_count, args);
  }

  VALUE rb_gv_get(const char* name) {
    NativeMethodContext* context = NativeMethodContext::current();

    VALUE Globals = rb_const_get(rb_cObject, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]"),
                      1,
                      context->handle_for(prefixed_by("$", name)));
  }

  ID rb_intern(const char* string) {
    NativeMethodContext* context = NativeMethodContext::current();

    return reinterpret_cast<ID>(context->state()->symbol(string));
  }

  VALUE rb_ivar_get(VALUE obj_handle, ID ivar_name) {
    NativeMethodContext* context = NativeMethodContext::current();

    Object* object = context->object_from(obj_handle);

    return context->handle_for(object->get_ivar(context->state(),
                               prefixed_by("@", ivar_name)));
  }

  VALUE rb_ivar_set(VALUE obj_handle, ID ivar_name, VALUE value) {
    NativeMethodContext* context = NativeMethodContext::current();

    Object* receiver = context->object_from(obj_handle);

    receiver->set_ivar(context->state(),
                       prefixed_by("@", ivar_name),
                       context->object_from(value));

    return value;
  }

  VALUE rb_obj_alloc(VALUE klass) {
    NativeMethodContext* context = NativeMethodContext::current();

    Class* cls = as<Class>(context->object_from(klass));
    Object* object = context->state()->new_object(cls);

    return context->handle_for(object);
  }

  VALUE rb_obj_as_string(VALUE obj_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    Object* object = context->object_from(obj_handle);

    if (kind_of<String>(object)) {
      return obj_handle;
    }

    return rb_funcall2(obj_handle, rb_intern("to_s"), 0, NULL);
  }

  void rb_raise(VALUE error_handle, const char* format_string, ...) {
    throw std::runtime_error(std::string("rb_raise: ") + format_string); /* Yeah, not 'exactly' correct. */
  }

  int rb_respond_to(VALUE obj_handle, ID method_name) {
    NativeMethodContext* context = NativeMethodContext::current();

    VALUE result = rb_funcall(obj_handle,
                              rb_intern("respond_to?"),
                              1,
                              ID2SYM(method_name)) ;

    return RBX_RTEST(context->object_from(result));
  }

  int rb_safe_level() {
    VALUE safe = rb_gv_get("$SAFE");

    return FIX2INT(safe);
  }

//  void rb_set_safe_level(int newlevel)
//  {
//    int safe_level = rb_safe_level();
//
//    if(newlevel > safe_level && newlevel < SAFE_LEVEL_MAX)
//    {
//      rb_gv_set("$SAFE", INT2FIX(newlevel) );
//    }
//  }

  void rb_secure(int level) {
    if (level <= rb_safe_level())
    {
      throw std::runtime_error("Given safety level too low.");
      //rb_raise(rb_eSecurityError, "Insecure operation at level %d", rb_safe_level());
    }
  }

  char rb_str_get_char(VALUE self_handle, int offset) {
    NativeMethodContext* context = NativeMethodContext::current();

    String* self = as<String>(context->object_from(self_handle));

    /* TODO: What kind of OOB checking is required? */
    size_t offset_as_size = offset;

    if (offset < 0 || offset_as_size >= self->size()) {
      return '\0';
    }

    return self->c_str()[offset_as_size];
  }

  size_t rb_str_get_char_len(VALUE self_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    /* TODO: Is this correct? Is assuming no wide characters valid? */
    return as<String>(context->object_from(self_handle))->size();
  }

  /* Replaces RSTRING */
  char* rb_str_get_char_ptr(VALUE str_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    String* string = as<String>(context->object_from(str_handle));

    return strdup(string->c_str());
  }

  VALUE rb_str_new(const char* string, size_t length) {
    NativeMethodContext* context = NativeMethodContext::current();

    return context->handle_for(String::create(context->state(), string, length));
  }

  VALUE rb_str_new2(const char* string) {
    if (string == NULL) {
      throw std::runtime_error("rb_str_new2(): NULL pointer given!");
      // rb_raise(rb_eArgError, "NULL pointer given");
    }

    return rb_str_new(string, std::strlen(string));
  }

  /* WTF is this idiocy? */
  VALUE rb_string_value(VALUE* obj_handle) {
    *obj_handle = rb_obj_as_string(*obj_handle);
    return *obj_handle;
  }

  VALUE rb_tainted_str_new2(const char* str) {
    return rb_str_new2(str);
  }

  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
  }

  ID rb_to_id(VALUE obj) {
    return SYM2ID(rb_funcall2(obj, rb_intern("to_sym"), 0, NULL));
  }

  VALUE rb_yield(VALUE argument_handle) {
    NativeMethodContext* context = NativeMethodContext::current();

    if (!rb_block_given_p()) {
      throw std::runtime_error("rb_yield(): No block given.");
      //rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    VALUE block_handle = context->handle_for(context->block());

    return rb_funcall(block_handle, rb_intern("call"), 1, argument_handle);
  }

}
