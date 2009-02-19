#include <sstream>
#include <string>

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "builtin/array.hpp"
#include "builtin/bignum.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/data.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/module.hpp"
#include "builtin/methodvisibility.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"

#include "vm/global_cache.hpp"
#include "vm/message.hpp"
#include "vm/object_utils.hpp"
#include "vm/objectmemory.hpp"
#include "vm/vm.hpp"

#include "vm/helpers.hpp"

#include "subtend/ruby.h"


/* Bring names to front where needed. */
using rubinius::Array;
using rubinius::Bignum;
using rubinius::ByteArray;
using rubinius::Class;
using rubinius::ClassType;
using rubinius::Data;
using rubinius::Fixnum;
using rubinius::Integer;
using rubinius::Message;
using rubinius::MethodVisibility;
using rubinius::Module;
using rubinius::ModuleType;
using rubinius::NativeMethod;
using rubinius::NativeMethodFraming;
using rubinius::Object;
using rubinius::SendSite;
using rubinius::String;
using rubinius::Symbol;
using rubinius::VM;

using rubinius::as;
using rubinius::kind_of;
using rubinius::native_int;
using rubinius::try_as;

namespace {

  /**
   *  Common implementation for rb_funcall*
   *
   *  @todo   Set up permanent SendSites through macroing?
   *  @todo   Stricter action check?
   */
  static VALUE hidden_funcall_backend(const char* file,
                                      int line,
                                      VALUE receiver,
                                      ID method_name,
                                      std::size_t arg_count,
                                      VALUE* arg_array)
  {
    NativeMethodFraming* framing = NativeMethodFraming::get();
    Array* args = Array::create(framing->state(), arg_count);

    for(size_t i = 0; i < arg_count; i++) {
      args->set(framing->state(), i, framing->get_object(arg_array[i]));
    }

    Object* recv = framing->get_object(receiver);
    Object* ret = recv->send(framing->state(), framing->current_call_frame(),
        reinterpret_cast<Symbol*>(method_name), args, RBX_Qnil);

    return framing->get_handle(ret);
  }

  /** Converts a native type (int, uint, long) to a suitable Integer. */
  template<typename NativeType>
    VALUE hidden_native2num(NativeType number) {
      NativeMethodFraming* framing = NativeMethodFraming::get();
      return framing->get_handle(Integer::from(framing->state(), number));
    }

  /** Make sure the name has the given prefix. */
  static Symbol* prefixed_by(std::string prefix, std::string name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    if(name.compare(0UL, prefix.size(), prefix) != 0) {
      std::ostringstream str;
      str << prefix << name;
      name.assign(str.str());
    }

    /* @todo Need to strdup here to not point to junk but can it leak? */
    return framing->state()->symbol(strdup(name.c_str()));
  }

  /** Make sure the name has the given prefix. */
  static Symbol* prefixed_by(std::string prefix, ID name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return prefixed_by(prefix, reinterpret_cast<Symbol*>(name)->c_str(framing->state()));
  }

}


/** API */
extern "C" {

  /**
   *  All of the default error class VALUEs are macroed
   *  to an invocation of this function to avoid tracking
   *  the classes on this side.
   *
   *  @todo Vectorise this or something.
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

  VALUE rbx_subtend_hidden_rb_funcall(const char* file, int line,
                                      VALUE receiver, ID method_name,
                                      int arg_count, ...)
  {
    va_list varargs;
    va_start(varargs, arg_count);

    VALUE* args = new VALUE[arg_count];

    for (int i = 0; i < arg_count; ++i) {
      args[i] = va_arg(varargs, VALUE);
    }

    va_end(varargs);

    VALUE ret = hidden_funcall_backend(file, line, receiver, method_name, arg_count, args);

    delete args;
    return ret;
  }

  VALUE rbx_subtend_hidden_rb_funcall2(const char* file, int line,
                                       VALUE receiver, ID method_name,
                                       int arg_count, VALUE* args)
  {
    return hidden_funcall_backend(file, line, receiver, method_name, arg_count, args);
  }


  /**
   *  All of the default rb_{c,m}* VALUEs are macroed
   *  to an invocation of this function, so we can
   *  avoid the hassle of tracking the objects on
   *  this side.
   */
  VALUE rbx_subtend_hidden_global(RbxSubtendHiddenGlobal type) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    /* @todo Move these to static */
    switch (type) {
    case RbxArray:
      return framing->get_handle_global(framing->state()->globals.array.get());
    case RbxBignum:
      return framing->get_handle_global(framing->state()->globals.bignum.get());
    case RbxClass:
      return framing->get_handle_global(framing->state()->globals.klass.get());
    case RbxData:
      return framing->get_handle_global(framing->state()->globals.data.get());
    case RbxFalse:
      return framing->get_handle_global(framing->state()->globals.false_class.get());
    case RbxFixnum:
      return framing->get_handle_global(framing->state()->globals.fixnum_class.get());
    case RbxFloat:
      return framing->get_handle_global(framing->state()->globals.floatpoint.get());
    /* Hash is not builtin, @see ruby.h */
    case RbxInteger:
      return framing->get_handle_global(framing->state()->globals.integer.get());
    case RbxIO:
      return framing->get_handle_global(framing->state()->globals.io.get());
    case RbxModule:
      return framing->get_handle_global(framing->state()->globals.module.get());
    case RbxNil:
      return framing->get_handle_global(framing->state()->globals.nil_class.get());
    case RbxObject:
      return framing->get_handle_global(framing->state()->globals.object.get());
    case RbxRegexp:
      return framing->get_handle_global(framing->state()->globals.regexp.get());
    case RbxString:
      return framing->get_handle_global(framing->state()->globals.string.get());
    case RbxSymbol:
      return framing->get_handle_global(framing->state()->globals.symbol.get());
    case RbxThread:
      return framing->get_handle_global(framing->state()->globals.thread.get());
    case RbxTrue:
      return framing->get_handle_global(framing->state()->globals.true_class.get());
    default:
      return Qnil;
    }
  }

  VALUE rbx_subtend_hidden_id2sym(ID id) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return framing->get_handle(reinterpret_cast<Symbol*>(id));
  }

  void rbx_subtend_hidden_infect(VALUE obj1, VALUE obj2) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* object1 = framing->get_object(obj1);
    Object* object2 = framing->get_object(obj2);

    object1->infect(object2);
  }

  int rbx_subtend_hidden_nil_p(VALUE expression_result) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return RBX_NIL_P(framing->get_object(expression_result));
  }

  long rbx_subtend_hidden_rstring_len(VALUE string_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* string = as<String>(framing->get_object(string_handle));

    return string->size();
  }

  char* rbx_subtend_hidden_rstring_ptr(VALUE string_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* string = as<String>(framing->get_object(string_handle));

    return string->byte_address();
  }

  int rbx_subtend_hidden_rtest(VALUE expression_result) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return RBX_RTEST(framing->get_object(expression_result));
  }

  ID rbx_subtend_hidden_sym2id(VALUE symbol_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return reinterpret_cast<ID>(framing->get_object(symbol_handle));
  }

  void rbx_subtend_hidden_define_method(const char* file,
                                        VALUE target,
                                        const char* name,
                                        SubtendGenericFunction fptr,
                                        int arity,
                                        RbxMethodKind kind)
  {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    VM* state = framing->state();
    Symbol* method_name = state->symbol(name);

    Module* module = NULL;

    if (kind == RbxSingletonMethod) {
      module = as<Module>(framing->get_object(target)->metaclass(framing->state()));
    }
    else {
      module = as<Module>(framing->get_object(target));
    }

    NativeMethod* method = NULL;
    method = NativeMethod::create(state,
                                  String::create(state, file),
                                  module,
                                  method_name,
                                  fptr,
                                  Fixnum::from(arity));

    MethodVisibility* visibility = MethodVisibility::create(state);
    visibility->method(state, method);

    switch(kind) {
    case RbxPrivateMethod:
      visibility->visibility(state, state->symbol("private"));
      break;

    case RbxProtectedMethod:
      visibility->visibility(state, state->symbol("protected"));
      break;

    default:  /* Also catches singletons for now. @todo Verify OK. --rue */
      visibility->visibility(state, state->symbol("public"));
      break;
    }

    module->method_table()->store(state, method_name, visibility);
    state->global_cache->clear(module, method_name);
  }



/* Real interface */


  /** Shares impl. with the other NUM2*, change all if modifying. */
  int NUM2INT(VALUE num_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* number = framing->get_object(num_handle);

    if(Fixnum* fix = try_as<Fixnum>(number)) {
      return fix->to_int();
    }
    else if(Bignum* big = try_as<Bignum>(number)) {
      return big->to_int();
    }
    else {
      rb_raise(rb_eArgError, "Argument must be an Integer!");
    }

    /* Compiler Appreciation Project */
    return -1;
  }

  /** Shares impl. with the other NUM2*, change all if modifying. */
  long int NUM2LONG(VALUE num_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* number = framing->get_object(num_handle);

    if(Fixnum* fix = try_as<Fixnum>(number)) {
      return fix->to_long();
    }
    else if(Bignum* big = try_as<Bignum>(number)) {
      return big->to_long();
    }
    else {
      rb_raise(rb_eArgError, "Argument must be an Integer!");
    }

    /* Compiler Appreciation Project */
    return -1;
  }

  /** Shares impl. with the other NUM2*, change all if modifying. */
  unsigned int NUM2UINT(VALUE num_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* number = framing->get_object(num_handle);

    if(Fixnum* fix = try_as<Fixnum>(number)) {
      return fix->to_uint();
    }
    else if(Bignum* big = try_as<Bignum>(number)) {
      return big->to_uint();
    }
    else {
      rb_raise(rb_eArgError, "Argument must be an Integer!");
    }

    /* Compiler Appreciation Project */
    return -1;
  }

  VALUE INT2NUM(int number) {
    return hidden_native2num<int>(number);
  }

  VALUE LONG2NUM(long int number) {
    return hidden_native2num<long int>(number);
  }

  VALUE UINT2NUM(unsigned int number) {
    return hidden_native2num<unsigned int>(number);
  }

  VALUE rb_Array(VALUE obj_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* obj = framing->get_object(obj_handle);

    if (kind_of<Array>(obj)) {
      return obj_handle;
    }

    Array* array = Array::create(framing->state(), 1);
    array->set(framing->state(), 0, obj);

    return framing->get_handle(array);
  }

  VALUE rb_ary_clear(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("clear"), 0, NULL);
  }

  VALUE rb_ary_dup(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("dup"), 0, NULL);
  }

  /* @todo Check 64-bit? */
  VALUE rb_ary_entry(VALUE self_handle, int index) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));
    return framing->get_handle(self->get(framing->state(), index));
  }

  VALUE rb_ary_join(VALUE self_handle, VALUE separator_handle) {
    return rb_funcall(self_handle, rb_intern("join"), 1, separator_handle);
  }

  /** By default, Arrays have space for 16 elements. */
  static const unsigned long RbxArrayDefaultCapacity = 16;

  VALUE rb_ary_new() {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* array = Array::create(framing->state(), RbxArrayDefaultCapacity);
    return framing->get_handle(array);
  }

  /* Shares implementation with rb_ary_new4! Change both if needed. */
  VALUE rb_ary_new2(unsigned long length) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* array = Array::create(framing->state(), (length * 2));
    array->start(framing->state(), Fixnum::from(0));
    array->total(framing->state(), Fixnum::from(length));
    /* OK, so we are probably screwed anyway if a Fixnum is too small. :) */

    return framing->get_handle(array);
  }

  /* Shares implementation with rb_ary_new2! Change both if needed. */
  VALUE rb_ary_new4(unsigned long length, const VALUE* object_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* array = Array::create(framing->state(), (length * 2));
    array->start(framing->state(), Fixnum::from(0));
    array->total(framing->state(), Fixnum::from(length));

    if (object_handle) {
      Object* object = framing->get_object(*object_handle);

      for(std::size_t i = 0; i < length; ++i) {
        array->set(framing->state(), i, object);
      }
    }

    return framing->get_handle(array);
  }

  VALUE rb_ary_pop(VALUE self_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));
    return framing->get_handle(self->pop(framing->state()));
  }

  VALUE rb_ary_push(VALUE self_handle, VALUE object_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));
    self->append(framing->state(), framing->get_object(object_handle));

    return self_handle;
  }

  VALUE rb_ary_reverse(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("reverse"), 0, NULL);
  }

  VALUE rb_ary_shift(VALUE self_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));
    return framing->get_handle(self->shift(framing->state()));
  }

  size_t rb_ary_size(VALUE self_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));

    return self->size();
  }

  void rb_ary_store(VALUE self_handle, long int index, VALUE object_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));
    size_t total = self->size();

    if(index < 0) {
      index += total;
    }

    if(index < 0) {
      std::ostringstream error;
      error << "Index " << (index - total) << " out of range!";
      rb_raise(rb_eIndexError, error.str().c_str());
    }

    self->set(framing->state(), index, framing->get_object(object_handle));
  }

  VALUE rb_ary_unshift(VALUE self_handle, VALUE object_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Array* self = as<Array>(framing->get_object(self_handle));
    self->unshift(framing->state(), framing->get_object(object_handle));

    return self_handle;
  }

  VALUE rb_attr_get(VALUE obj_handle, ID attr_name) {
    return rb_ivar_get(obj_handle, attr_name);
  }

  int rb_block_given_p() {
    NativeMethodFraming* framing = NativeMethodFraming::get();
    return RBX_RTEST(framing->block());
  }

  VALUE rb_check_array_type(VALUE object_handle) {
    return rb_check_convert_type(object_handle, 0, "Array", "to_ary");
  }

  VALUE rb_check_string_type(VALUE object_handle) {
    return rb_check_convert_type(object_handle, 0, "String", "to_str");
  }

  VALUE rb_check_convert_type(VALUE object_handle, int /*type*/,
                              const char* type_name, const char* method_name)
  {
    NativeMethodFraming* framing = NativeMethodFraming::get();
    VALUE name = framing->get_handle(String::create(framing->state(), method_name));

    if(RTEST(rb_funcall(object_handle, rb_intern("respond_to?"), 1, name)) ) {
      return rb_funcall2(object_handle, rb_intern(method_name), 0, NULL);
    }

    return Qnil;
  }

  VALUE rb_class_name(VALUE class_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();
    Class* class_object = as<Class>(framing->get_object(class_handle));
    return framing->get_handle(class_object->name()->to_str(framing->state()));
  }

  VALUE rb_class_new_instance(int arg_count, VALUE* args, VALUE class_handle) {
    return rb_funcall2(class_handle, rb_intern("new"), arg_count, args);
  }

  VALUE rb_class_of(VALUE object_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();
    Class* class_object = framing->get_object(object_handle)->class_object(framing->state());
    return framing->get_handle_global(class_object);
  }

  char* rb_class2name(VALUE class_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();
    Class* class_object = as<Class>(framing->get_object(class_handle));

    return ::strdup(class_object->name()->c_str(framing->state()));
  }

  /** @todo   This is horrible. Refactor. --rue */
  VALUE rb_convert_type(VALUE object_handle, int type,
                        const char* type_name, const char* method_name)
  {
    VALUE return_handle = rb_check_convert_type(object_handle, type,
                                                type_name, method_name);

    if(NIL_P(return_handle)) {
      rb_raise(rb_eTypeError, "can't convert %s into %s",
               RBX_NIL_P(object_handle) ? "nil" :
                RBX_TRUE_P(object_handle) ? "true" :
                  RBX_FALSE_P(object_handle) ? "false" :
                    rb_obj_classname(object_handle),
               type_name);
    }

    VALUE klass = rb_const_get(rb_cObject, rb_intern(type_name));

    if(!RTEST(rb_obj_is_kind_of(return_handle, klass))) {
      rb_raise(rb_eTypeError, "%s#to_str should return %s",
               rb_obj_classname(return_handle), type_name);
    }

    return return_handle;
  }

  int rb_const_defined(VALUE module_handle, ID const_id) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    VALUE result = rb_funcall(module_handle, rb_intern("const_defined?"), 1, ID2SYM(const_id));
    return RBX_RTEST(framing->get_object(result));
  }

  VALUE rb_const_get(VALUE module_handle, ID name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Module* module = as<Module>(framing->get_object(module_handle));

    return framing->get_handle(module->get_const(framing->state(),
                                                 reinterpret_cast<Symbol*>(name)));
  }

  VALUE rb_cstr2inum(const char* string, int base) {
    return rb_str2inum(rb_str_new2(string), base);
  }

  VALUE rb_cv_get(VALUE module_handle, const char* name) {
    return rb_cvar_get(module_handle, rb_intern(name));
  }

  VALUE rb_cv_set(VALUE module_handle, const char* name, VALUE value) {
    return rb_cvar_set(module_handle, rb_intern(name), value);
  }

  VALUE rb_cvar_defined(VALUE module_handle, ID name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return rb_funcall(module_handle, rb_intern("class_variable_defined?"),
                      1,
                      framing->get_handle(prefixed_by("@@", name)));
  }

  VALUE rb_cvar_get(VALUE module_handle, ID name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return rb_funcall(module_handle, rb_intern("class_variable_set"),
                      1,
                      framing->get_handle(prefixed_by("@@", name)));
  }

  VALUE rb_cvar_set(VALUE module_handle, ID name, VALUE value) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return rb_funcall(module_handle, rb_intern("class_variable_set"),
                      2,
                      framing->get_handle(prefixed_by("@@", name)),
                      value);
  }

  VALUE rb_data_object_alloc(VALUE klass, RUBY_DATA_FUNC mark,
                             RUBY_DATA_FUNC free, void* ptr) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Class* data_klass = as<Class>(framing->get_object(klass));

    Data* data = Data::create(framing->state(), ptr, mark, free);

    data->klass(framing->state(), data_klass);

    return framing->get_handle(data);
  }

  void rb_define_alias(VALUE module_handle, const char* new_name, const char* old_name) {
    ID id_new = rb_intern(new_name);
    ID id_old = rb_intern(old_name);

    rb_funcall(module_handle, rb_intern("alias_method_cv"), 2, id_new, id_old);
  }

  void rb_define_alloc_func(VALUE class_handle, SubtendAllocFunction allocator) {
    rb_define_singleton_method(class_handle, "allocate", allocator, 0);
  }

  void rb_define_attr(VALUE module_handle, const char* attr_name, int readable, int writable) {
    if(readable) {
      rb_funcall(module_handle, rb_intern("attr_reader_cv"), 1, rb_intern(attr_name));
    }

    if(writable) {
      rb_funcall(module_handle, rb_intern("attr_writer_cv"), 1, rb_intern(attr_name));
    }
  }

  VALUE rb_define_class(const char* name, VALUE superclass_handle) {
    return rb_define_class_under(rb_cObject, name, superclass_handle);
  }

  /** @note   Shares code with rb_define_module_under, change there too. --rue */
  VALUE rb_define_class_under(VALUE parent_handle, const char* name, VALUE superclass_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Module* parent = as<Module>(framing->get_object(parent_handle));
    Class* superclass = as<Class>(framing->get_object(superclass_handle));
    Symbol* constant = framing->state()->symbol(name);

    bool created = false;
    Class* cls = rubinius::Helpers::open_class(framing->state(),
        framing->current_call_frame(), parent, superclass, constant, &created);

    return framing->get_handle_global(cls);
  }

  void rb_define_const(VALUE module_handle, const char* name, VALUE obj_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Module* module = as<Module>(framing->get_object(module_handle));
    Object* object = framing->get_object(obj_handle);

    module->set_const(framing->state(), name,  object);
  }

  VALUE rb_define_module(const char* name) {
    return rb_define_module_under(rb_cObject, name);
  }

  void rb_define_module_function(VALUE module_handle, const char* name, SubtendGenericFunction func, int args) {
    rb_define_private_method(module_handle, name, func, args);
    rb_define_singleton_method(module_handle, name, func, args);
  }

  /** @note   Shares code with rb_define_class_under, change there too. --rue */
  VALUE rb_define_module_under(VALUE parent_handle, const char* name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Module* parent = as<Module>(framing->get_object(parent_handle));
    Symbol* constant = framing->state()->symbol(name);

    Module* module = rubinius::Helpers::open_module(framing->state(),
        framing->current_call_frame(), parent, constant);

    return framing->get_handle_global(module);
  }

  void rb_free_global(VALUE global_handle) {
    /* Failing silently is OK by MRI. */
    if(global_handle < 0) {
      NativeMethodFraming* framing = NativeMethodFraming::get();
      framing->delete_global(global_handle);
    }
  }

  void rb_gc_mark(VALUE ptr) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* object = framing->get_object(ptr);

    if(object->reference_p()) {
      Object* res = VM::current_state()->current_mark.call(object);

      if(res) {
        framing->handles()[ptr]->set(res);
      }
    }
  }

  /** @todo   Check logic. This alters the VALUE to be a global handler. --rue */
  void rb_global_variable(VALUE* address) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* object = framing->get_object(*address);

    if(REFERENCE_P(object)) {
      *address = framing->get_handle_global(object);
    }
  }

  VALUE rb_gv_get(const char* name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    VALUE Globals = rb_const_get(rb_cObject, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]"),
                      1,
                      framing->get_handle(prefixed_by("$", name)));
  }

  VALUE rb_gv_set(const char* name, VALUE value) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    VALUE Globals = rb_const_get(rb_cObject, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]="),
                      2,
                      framing->get_handle(prefixed_by("$", name)),
                      value);
  }

  void rb_include_module(VALUE includer_handle, VALUE includee_handle) {
    rb_funcall(includer_handle, rb_intern("include"), 1, includee_handle);
  }

  ID rb_intern(const char* string) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return reinterpret_cast<ID>(framing->state()->symbol(string));
  }

  VALUE rb_obj_alloc(VALUE class_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    // TODO using Object as the template param means this can't allocate builtin
    // types properly!
    Object* object = framing->state()->new_object<Object>(as<Class>(framing->get_object(class_handle)));
    return framing->get_handle(object);
  }

  VALUE rb_obj_as_string(VALUE obj_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* object = framing->get_object(obj_handle);

    if (kind_of<String>(object)) {
      return obj_handle;
    }

    return rb_funcall2(obj_handle, rb_intern("to_s"), 0, NULL);
  }

  void rb_obj_call_init(VALUE object_handle, int arg_count, VALUE* args) {
    (void) rb_funcall2(object_handle, rb_intern("initialize"), arg_count, args);
  }

  char* rb_obj_classname(VALUE object_handle) {
    return rb_class2name(rb_class_of(object_handle));
  }

  VALUE rb_obj_is_instance_of(VALUE object_handle, VALUE class_handle) {
    return rb_funcall(object_handle, rb_intern("instance_of?"), 1, class_handle);
  }

  VALUE rb_obj_is_kind_of(VALUE object_handle, VALUE module_handle) {
    return rb_funcall(object_handle, rb_intern("kind_of?"), 1, module_handle);
  }

  VALUE rb_iv_get(VALUE self_handle, const char* name) {
    return rb_ivar_get(self_handle, rb_intern(name));
  }

  VALUE rb_iv_set(VALUE self_handle, const char* name, VALUE value) {
    return rb_ivar_set(self_handle, rb_intern(name), value);
  }

  VALUE rb_ivar_get(VALUE self_handle, ID ivar_name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* object = framing->get_object(self_handle);

    return framing->get_handle(object->get_ivar(framing->state(),
                               prefixed_by("@", ivar_name)));
  }

  VALUE rb_ivar_set(VALUE self_handle, ID ivar_name, VALUE value) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* receiver = framing->get_object(self_handle);

    receiver->set_ivar(framing->state(),
                       prefixed_by("@", ivar_name),
                       framing->get_object(value));

    return value;
  }

  void rb_raise(VALUE error_handle, const char* format_string, ...) {
    throw std::runtime_error(std::string("rb_raise: ") + format_string); /* Yeah, not 'exactly' correct. */
  }

  VALUE rb_require(const char* name) {
    return rb_funcall(rb_mKernel, rb_intern("require"), 1, rb_str_new2(name));
  }

  int rb_respond_to(VALUE obj_handle, ID method_name) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    VALUE result = rb_funcall(obj_handle,
                              rb_intern("respond_to?"),
                              1,
                              ID2SYM(method_name)) ;

    return RBX_RTEST(framing->get_object(result));
  }

  int rb_safe_level() {
    return FIX2INT(rb_gv_get("$SAFE"));
  }

  int rb_scan_args(int argc, const VALUE* argv, const char* spec, ...) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    int n, i = 0;
    const char *p = spec;
    VALUE *var;
    va_list vargs;

    va_start(vargs, spec);

    if (*p == '*') goto rest_arg;

    if (ISDIGIT(*p)) {
      n = *p - '0';
      if (n > argc)
        rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, n);
      for (i=0; i<n; i++) {
        var = va_arg(vargs, VALUE*);
        if (var) *var = argv[i];
      }
      p++;
    }
    else {
      goto error;
    }

    if (ISDIGIT(*p)) {
      n = i + *p - '0';
      for (; i<n; i++) {
        var = va_arg(vargs, VALUE*);
        if (argc > i) {
          if (var) *var = argv[i];
        }
        else {
          if (var) *var = Qnil;
        }
      }
      p++;
    }

    if(*p == '*') {
      rest_arg:
      var = va_arg(vargs, VALUE*);
      if (argc > i) {
        if (var) *var = rb_ary_new4(argc-i, argv+i);
        i = argc;
      }
      else {
        if (var) *var = rb_ary_new();
      }
      p++;
    }

    if (*p == '&') {
      var = va_arg(vargs, VALUE*);
      *var = framing->get_handle(framing->block());
      p++;
    }
    va_end(vargs);

    if (*p != '\0') {
      goto error;
    }

    if (argc > i) {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, i);
    }

    return argc;

    error:
    rb_raise(rb_eFatal, "bad scan arg format: %s", spec);
    return 0;
  }

  /** Some arbitrary high max for $SAFE. */
  static const int RBX_ARBITRARY_MAX_SAFE_LEVEL = 256;

  /**
   *  @todo   Error out if level is invalid?
   *          Or should it fail silently as
   *          a security measure? --rue
   */
  void rb_set_safe_level(int new_level) {
    int safe_level = rb_safe_level();

    if(new_level > safe_level && new_level < RBX_ARBITRARY_MAX_SAFE_LEVEL) {
      rb_gv_set("$SAFE", INT2FIX(new_level));
    }
  }

  void rb_secure(int level) {
    if (level <= rb_safe_level())
    {
      rb_raise(rb_eSecurityError, "Insecure operation at level %d", rb_safe_level());
    }
  }

  /** @todo   Should this be a global handle? Surely not.. --rue */
  VALUE rb_singleton_class(VALUE object_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Class* metaclass = framing->get_object(object_handle)->metaclass(framing->state());
    return framing->get_handle(metaclass);
  }

  VALUE rb_String(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "String", "to_s");
  }

  VALUE rb_str_append(VALUE self_handle, VALUE other_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* self = as<String>(framing->get_object(self_handle));
    self->append(framing->state(), as<String>(framing->get_object(other_handle)));

    return self_handle;
  }

  VALUE rb_str_buf_append(VALUE self_handle, VALUE other_handle) {
    return rb_str_append(self_handle, other_handle);
  }

  VALUE rb_str_buf_cat(VALUE string_handle, const char* other, size_t size) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* string = as<String>(framing->get_object(string_handle));
    string->append(framing->state(), other, size);

    return string_handle;
  }

  VALUE rb_str_buf_cat2(VALUE string_handle, const char* other) {
    return rb_str_buf_cat(string_handle, other, std::strlen(other));
  }

  VALUE rb_str_cat(VALUE self_handle, const char* other, size_t length) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* self = as<String>(framing->get_object(self_handle));
    String* combo = self->string_dup(framing->state());

    return framing->get_handle(combo->append(framing->state(), other, length));
  }

  VALUE rb_str_cat2(VALUE string_handle, const char* other) {
    return rb_str_cat(string_handle, other, std::strlen(other));
  }

  int rb_str_cmp(VALUE first_handle, VALUE second_handle) {
    return NUM2INT(rb_funcall(first_handle, rb_intern("<=>"), 1, second_handle));
  }

  VALUE rb_str_concat(VALUE self_handle, VALUE other_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    Object* other = framing->get_object(other_handle);

    /* Could be a character code. Only up to 256 supported. */
    if(Fixnum* character = try_as<Fixnum>(other)) {
      char byte = character->to_uint();

      return rb_str_cat(self_handle, &byte, 1);
    }

    return rb_str_append(self_handle, other_handle);
  }

  VALUE rb_str_dup(VALUE self_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* self = as<String>(framing->get_object(self_handle));
    return framing->get_handle(self->string_dup(framing->state()));
  }

  /** @todo Refactor into a String::replace(). --rue */
  void rb_str_flush_char_ptr(VALUE string_handle, char* c_string, size_t length) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    ByteArray* data = ByteArray::create(framing->state(), (length + 1));
    std::memcpy(data->bytes, c_string, length);
    data->bytes[length] = '\0';

    Integer* bytes = Integer::from(framing->state(), length);

    String* string = as<String>(framing->get_object(string_handle));
    string->num_bytes(framing->state(), bytes);
    string->characters(framing->state(), bytes);
    string->hash_value(framing->state(), reinterpret_cast<Integer*>(Qnil));
    /* Assume the encoding stays the same. */

    string->data(framing->state(), data);
  }

  char rb_str_get_char(VALUE self_handle, int offset) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* self = as<String>(framing->get_object(self_handle));

    /* @todo What kind of OOB checking is required? */
    size_t offset_as_size = offset;

    if (offset < 0 || offset_as_size >= self->size()) {
      return '\0';
    }

    return self->c_str()[offset_as_size];
  }

  size_t rb_str_get_char_len(VALUE self_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    /* @todo Is this correct? Is assuming no wide characters valid? */
    return as<String>(framing->get_object(self_handle))->size();
  }

  char* rb_str_get_char_ptr(VALUE str_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    String* string = as<String>(framing->get_object(str_handle));
    size_t length = string->size();

    char* buffer = ALLOC_N(char, (length + 1));
    std::memcpy(buffer, string->c_str(), length);
    buffer[length] = '\0';

    return buffer;
  }

  VALUE rb_str_new(const char* string, size_t length) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    return framing->get_handle(String::create(framing->state(), string, length));
  }

  VALUE rb_str_new2(const char* string) {
    if(string == NULL) {
      rb_raise(rb_eArgError, "NULL pointer given");
    }

    return rb_str_new(string, std::strlen(string));
  }

  VALUE rb_str_plus(VALUE self_handle, VALUE other_handle) {
    return rb_str_append(rb_str_dup(self_handle), other_handle);
  }

  VALUE rb_str_split(VALUE self_handle, const char* separator) {
    return rb_funcall(self_handle, rb_intern("split"), 1, rb_str_new2(separator));
  }

  VALUE rb_str_substr(VALUE self_handle, size_t starting_index, size_t length) {
    return rb_funcall(self_handle, rb_intern("slice"), 2,
                      LONG2NUM(starting_index), LONG2NUM(length) );
  }

  VALUE rb_str_times(VALUE self_handle, VALUE times) {
    return rb_funcall(self_handle, rb_intern("*"), 1, times);
  }

  VALUE rb_str2inum(VALUE self_handle, int base) {
    return rb_funcall(self_handle, rb_intern("to_i"), 1, INT2NUM(base));
  }

  VALUE rb_str_to_str(VALUE object_handle) {
    return rb_String(object_handle);
  }

  VALUE rb_string_value(VALUE* object_variable) {
    *object_variable = rb_obj_as_string(*object_variable);
    return *object_variable;
  }

  char* rb_string_value_cstr(VALUE* object_variable) {
    return rb_str_get_char_ptr(rb_string_value(object_variable));
  }

  VALUE rb_tainted_str_new2(const char* string) {
    if(string == NULL) {
      rb_raise(rb_eArgError, "NULL pointer given");
    }

    NativeMethodFraming* framing = NativeMethodFraming::get();

    return framing->get_handle(
      String::create(framing->state(), string, std::strlen(string))->taint()
    );
  }

  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
  }

  ID rb_to_id(VALUE object_handle) {
    return SYM2ID(rb_funcall2(object_handle, rb_intern("to_sym"), 0, NULL));
  }

  /** @todo This is horrible. Refactor. --rue */
  int rb_type(VALUE object_handle) {
    struct type_map {
      int type;
      const char *name;
    } tmap[] = {
      {T_NIL,     "NilClass"},
      {T_SYMBOL,  "Symbol"},
      {T_CLASS,   "Class"},
      {T_MODULE,  "Module"},
      {T_FLOAT,   "Float"},
      {T_STRING,  "String"},
      {T_REGEXP,  "Regexp"},
      {T_ARRAY,   "Array"},
      {T_FIXNUM,  "Fixnum"},
      {T_HASH,    "Hash"},
      {T_STRUCT,  "Struct"},
      {T_BIGNUM,  "Bignum"},
      {T_FILE,    "File"},
      {T_TRUE,    "TrueClass"},
      {T_FALSE,   "FalseClass"},
      {T_MATCH,   "MatchData"},
      {T_OBJECT,  "Object"},
      {0,         0}
    };

    int i;

    for(i = 0; tmap[i].name != 0; ++i) {
      VALUE class_handle = rb_const_get(rb_cObject, rb_intern(tmap[i].name));

      if(rb_obj_is_kind_of(object_handle, class_handle) == Qtrue) {
        return tmap[i].type;
      }
    }

    return T_OBJECT;
  }

  void rb_undef_method(VALUE module_handle, const char* name) {
    rb_funcall(module_handle, rb_intern("undef_method"), 1, ID2SYM(rb_intern(name)));
  }

  VALUE rb_yield(VALUE argument_handle) {
    NativeMethodFraming* framing = NativeMethodFraming::get();

    if (!rb_block_given_p()) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    VALUE block_handle = framing->get_handle(framing->block());

    return rb_funcall(block_handle, rb_intern("call"), 1, argument_handle);
  }

  void ruby_xfree(void* x) {
    if(x) free(x);
  }

}
