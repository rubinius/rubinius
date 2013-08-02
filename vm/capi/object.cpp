#include "builtin/compactlookuptable.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

#include "configuration.hpp"
#include "version.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  void rb_error_frozen(const char* what) {
    if(LANGUAGE_18_ENABLED){
      rb_raise(rb_eTypeError, "can't modify frozen %s", what);
    } else {
      rb_raise(rb_eRuntimeError, "can't modify frozen %s", what);
    }
  }

  VALUE rb_obj_frozen_p(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    if(CBOOL(env->get_object(obj)->frozen_p(env->state()))) {
      return Qtrue;
    }

    return Qfalse;
  }

  void rb_check_frozen(VALUE obj_handle) {
    if(rb_obj_frozen_p(obj_handle)){
      const char *class_name = rb_obj_classname(obj_handle);
      rb_error_frozen(class_name);
    }
  }

  VALUE rb_obj_freeze(VALUE hndl) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    env->get_object(hndl)->freeze(env->state());
    return hndl;
  }

  // Copied from MRI
  static struct types {
    int type;
    const char *name;
  } builtin_types[] = {
    {T_NIL,      "nil"},
    {T_OBJECT,  "Object"},
    {T_CLASS,    "Class"},
    {T_ICLASS,  "iClass"},    /* internal use: mixed-in module holder */
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
    {T_TRUE,    "true"},
    {T_FALSE,   "false"},
    {T_SYMBOL,  "Symbol"},    /* :symbol */
    {T_DATA,    "Data"},      /* internal use: wrapped C pointers */
    {T_MATCH,   "MatchData"}, /* data of $~ */
    {T_VARMAP,  "Varmap"},    /* internal use: dynamic variables */
    {T_SCOPE,   "Scope"},     /* internal use: variable scope */
    {T_NODE,    "Node"},      /* internal use: syntax tree node */
    {T_UNDEF,   "undef"},     /* internal use: #undef; should not happen */
    {T_RATIONAL, "Rational" },
    {T_COMPLEX,  "Complex" },
    {-1,  0}
  };

  // Copied from MRI
  void rb_check_type(VALUE x, int t) {
    struct types *type = builtin_types;

    if(x == Qundef) {
      rb_bug("undef leaked to the Ruby space");
    }

    if(TYPE(x) != t) {
      while (type->type >= 0) {
        if(type->type == t) {
          const char *etype;

          if(NIL_P(x)) {
            etype = "nil";
          } else if(FIXNUM_P(x)) {
            etype = "Fixnum";
          } else if(SYMBOL_P(x)) {
            etype = "Symbol";
          } else if(rb_special_const_p(x)) {
            etype = RSTRING_PTR(rb_obj_as_string(x));
          } else {
            etype = rb_obj_classname(x);
          }

          rb_raise(rb_eTypeError, "wrong argument type %s (expected %s)",
              etype, type->name);
        }
        type++;
      }

      rb_raise(rb_eRuntimeError, "unknown type 0x%x", t);
    }
  }

  VALUE rb_check_array_type(VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return rb_funcall(env->get_handle(env->state()->globals().type.get()), rb_intern("try_convert"), 3, object_handle, rb_cArray, rb_intern("to_ary"));
  }

  VALUE rb_check_string_type(VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return rb_funcall(env->get_handle(env->state()->globals().type.get()), rb_intern("try_convert"), 3, object_handle, rb_cString, rb_intern("to_str"));
  }

  VALUE rb_check_convert_type(VALUE object_handle, int /*type*/,
                              const char* type_name, const char* method_name)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE name = env->get_handle(String::create(env->state(), method_name));

    if(RTEST(rb_funcall(object_handle, rb_intern("respond_to?"), 1, name)) ) {
      return rb_funcall2(object_handle, rb_intern(method_name), 0, NULL);
    }

    return Qnil;
  }

  VALUE rb_check_to_integer(VALUE object_handle, const char *method_name) {
    if(FIXNUM_P(object_handle)) {
      return object_handle;
    }
    VALUE result = rb_check_convert_type(object_handle, 0, "Integer", method_name);
    if(rb_obj_is_kind_of(result, rb_cInteger)) {
      return result;
    }
    return Qnil;
  }

  /** @todo   This is horrible. Refactor. --rue */
  VALUE rb_convert_type(VALUE object_handle, int type,
                        const char* type_name, const char* method_name)
  {
    VALUE return_handle = rb_check_convert_type(object_handle, type,
                                                type_name, method_name);

    if(NIL_P(return_handle)) {
      rb_raise(rb_eTypeError, "can't convert %s into %s",
               NIL_P(object_handle) ? "nil" :
                TRUE_P(object_handle) ? "true" :
                  FALSE_P(object_handle) ? "false" :
                    rb_obj_classname(object_handle),
               type_name);
    }

    VALUE klass = rb_const_get(rb_cObject, rb_intern(type_name));

    if(!RTEST(rb_obj_is_kind_of(return_handle, klass))) {
      rb_raise(rb_eTypeError, "%s#%s should return %s",
               rb_obj_classname(object_handle), method_name, type_name);
    }

    return return_handle;
  }

  int rb_type(VALUE obj) {
    if(FIXNUM_P(obj)) return T_FIXNUM;
    if(obj == Qnil) return T_NIL;
    if(obj == Qfalse) return T_FALSE;
    if(obj == Qtrue) return T_TRUE;
    if(obj == Qundef) return T_UNDEF;
    if(SYMBOL_P(obj)) return T_SYMBOL;

    capi::Handle* handle = capi::Handle::from(obj);
    Object* object = handle->object();
    switch(object->type_id()) {
    case ArrayType:
      return T_ARRAY;
    case BignumType:
      return T_BIGNUM;
    case ClassType:
      return T_CLASS;
    case DataType:
      return T_DATA;
    case FloatType:
      return T_FLOAT;
    case ModuleType:
      return T_MODULE;
    case RegexpType:
      return T_REGEXP;
    case StringType:
      return T_STRING;
    case TimeType:
      return T_DATA;
    default:
      // This is in the default branch to avoid compiler warnings
      // about other enum values for type_id() not being present.
      if(rb_obj_is_kind_of(obj, rb_cHash)) return T_HASH;
      if(rb_obj_is_kind_of(obj, rb_cStruct)) return T_STRUCT;
      if(rb_obj_is_kind_of(obj, rb_cIO)) return T_FILE;
      if(rb_obj_is_kind_of(obj, rb_cMatch)) return T_MATCH;
      if(!LANGUAGE_18_ENABLED) {
        if(rb_obj_is_kind_of(obj, rb_cRational)) return T_RATIONAL;
        if(rb_obj_is_kind_of(obj, rb_cComplex)) return T_COMPLEX;
        if(rb_obj_is_kind_of(obj, rb_cEncoding)) return T_ENCODING;
      }
    }

    return T_OBJECT;
  }

  ID rb_to_id(VALUE object_handle) {
    return SYM2ID(rb_funcall2(object_handle, rb_intern("to_sym"), 0, NULL));
  }

  VALUE rb_obj_alloc(VALUE class_handle) {
    return rb_funcall(class_handle, rb_intern("allocate"), 0);
  }

  VALUE rb_obj_dup(VALUE obj) {
    if(rb_special_const_p(obj))
      rb_raise(rb_eTypeError, "can't dup %s", rb_obj_classname(obj));

    return rb_funcall(obj, rb_intern("dup"), 0);
  }

  VALUE rb_obj_as_string(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj_handle);

    if(kind_of<String>(object)) {
      return obj_handle;
    }

    return rb_funcall2(obj_handle, rb_intern("to_s"), 0, NULL);
  }

  VALUE rb_obj_clone(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    env->flush_cached_data();
    return rb_funcall(obj_handle, rb_intern("clone"), 0);
  }

  VALUE rb_inspect(VALUE obj_handle) {
    return rb_obj_as_string(rb_funcall(obj_handle, rb_intern("inspect"), 0, NULL));
  }

  void rb_obj_call_init(VALUE object_handle, int arg_count, VALUE* args) {
    (void) rb_funcall2(object_handle, rb_intern("initialize"), arg_count, args);
  }

  const char* rb_obj_classname(VALUE object_handle) {
    return rb_class2name(rb_class_of(object_handle));
  }

  VALUE rb_obj_is_instance_of(VALUE object_handle, VALUE class_handle) {
    return rb_funcall(object_handle, rb_intern("instance_of?"), 1, class_handle);
  }

  VALUE rb_obj_is_kind_of(VALUE object_handle, VALUE module_handle) {
    return rb_funcall(object_handle, rb_intern("kind_of?"), 1, module_handle);
  }

  ID rb_intern(const char* string) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return reinterpret_cast<ID>(env->state()->symbol(string));
  }

  ID rb_intern2(const char* string, long len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return reinterpret_cast<ID>(env->state()->symbol(string, len));
  }

  VALUE rb_iv_get(VALUE self_handle, const char* name) {
    return rb_ivar_get(self_handle, rb_intern(name));
  }

  VALUE rb_iv_set(VALUE self_handle, const char* name, VALUE value) {
    return rb_ivar_set(self_handle, rb_intern(name), value);
  }

  VALUE rb_ivar_get(VALUE self_handle, ID ivar_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(self_handle);

    Symbol* sym = reinterpret_cast<Symbol*>(ivar_name);
    return env->get_handle(object->get_ivar(env->state(), sym));
  }

  VALUE rb_ivar_set(VALUE self_handle, ID ivar_name, VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* receiver = env->get_object(self_handle);

    Symbol* sym = reinterpret_cast<Symbol*>(ivar_name);
    receiver->set_ivar(env->state(), sym,
                       env->get_object(value));

    return value;
  }

  VALUE rb_ivar_defined(VALUE obj_handle, ID ivar_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Symbol* ivar = reinterpret_cast<Symbol*>(ivar_name);
    Object* obj = env->get_object(obj_handle);
    Object* ret = obj->ivar_defined(env->state(), ivar);

    return env->get_handle(ret);
  }

  VALUE rb_attr_get(VALUE obj_handle, ID attr_name) {
    return rb_ivar_get(obj_handle, attr_name);
  }

  VALUE rb_obj_instance_variables(VALUE obj_handle) {
    return rb_funcall(obj_handle, rb_intern("instance_variables"), 0);
  }

  int rb_respond_to(VALUE obj_handle, ID method_name) {
    return RTEST(rb_funcall(obj_handle, rb_intern("respond_to?"),
          1, ID2SYM(method_name)));
  }

  void rb_extend_object(VALUE obj, VALUE mod) {
    rb_funcall(obj, rb_intern("extend"), 1, mod);
  }

  VALUE rb_obj_id(VALUE self) {
    return rb_funcall(self, rb_intern("object_id"), 0);
  }

  VALUE rb_obj_taint(VALUE obj) {
    if(!OBJ_TAINTED(obj)) {
      rb_check_frozen(obj);
      OBJ_TAINT(obj);
    }

    return obj;
  }

  void rb_check_safe_obj(VALUE obj) {
  }

  void rb_check_safe_str(VALUE obj) {
  }

  void rb_secure_update(VALUE obj) {
  }

  VALUE rb_any_to_s(VALUE obj) {
    return rb_obj_as_string(obj);
  }

  VALUE rb_obj_instance_eval(int argc, VALUE* argv, VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE block = env->get_handle(env->block());

    return rb_funcall2b(self, rb_intern("instance_eval"), argc,
                        (const VALUE*)argv, block);
  }

  VALUE rb_to_int(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "Integer", "to_int");
  }

  VALUE rb_hash(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* hash = env->get_object(rb_funcall(obj, rb_intern("hash"), 0));

    retry:

    if(try_as<Fixnum>(hash)) {
      return env->get_handle(hash);
    } else if(Bignum* big = try_as<Bignum>(hash)) {
      return LONG2FIX(big->to_native());
    } else {
      hash = env->get_object(rb_to_int(env->get_handle(hash)));
      goto retry;
    }
  }

  VALUE rb_equal(VALUE obj1, VALUE obj2) {
    if(obj1 == obj2) return Qtrue;
    VALUE result = rb_funcall(obj1, rb_intern("=="), 1, obj2);
    if(RTEST(result)) return Qtrue;
    return Qfalse;
  }

  VALUE rb_class_inherited_p(VALUE mod, VALUE arg) {
    if(TYPE(arg) != T_MODULE && TYPE(arg) != T_CLASS) {
      rb_raise(rb_eTypeError, "compared with non class/module");
    }

    return rb_funcall(mod, rb_intern("<="), 1, arg);
  }

  int rb_obj_respond_to(VALUE obj, ID method_name, int priv) {
    VALUE include_private = priv == 1 ? Qtrue : Qfalse;
    return RTEST(rb_funcall(obj, rb_intern("respond_to?"), 2, ID2SYM(method_name), include_private));
  }

  int rb_method_boundp(VALUE cls, ID method_name, int exclude_priv) {
    VALUE public_defined = rb_funcall(cls, rb_intern("method_defined?"), 1, ID2SYM(method_name));
    if(RTEST(public_defined)) return 1;
    if(!exclude_priv) {
      return RTEST(rb_funcall(cls, rb_intern("private_method_defined?"), 1, ID2SYM(method_name)));
    }
    return 0;
  }
}
