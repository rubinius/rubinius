#include "builtin/compactlookuptable.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "capi/capi.hpp"

using namespace capi;

extern "C" {
  void rb_check_frozen(VALUE obj_handle) {
    /* @todo  implement when rbx supports frozen objects. */
  }

  void rb_check_type(VALUE obj_handle, CApiType type) {
    /* @todo  implement; checks if obj_handle's type is 'type',
     * raises an exception if it is not.
     */
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
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE name = env->get_handle(String::create(env->state(), method_name));

    if(RTEST(rb_funcall(object_handle, rb_intern("respond_to?"), 1, name)) ) {
      return rb_funcall2(object_handle, rb_intern(method_name), 0, NULL);
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

  ID rb_to_id(VALUE object_handle) {
    return SYM2ID(rb_funcall2(object_handle, rb_intern("to_sym"), 0, NULL));
  }

  VALUE rb_obj_alloc(VALUE class_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    // TODO using Object as the template param means this can't allocate builtin
    // types properly!
    Object* object = env->state()->new_object<Object>(as<Class>(env->get_object(class_handle)));
    return env->get_handle(object);
  }

  VALUE rb_obj_as_string(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj_handle);

    if (kind_of<String>(object)) {
      return obj_handle;
    }

    return rb_funcall2(obj_handle, rb_intern("to_s"), 0, NULL);
  }

  VALUE rb_obj_clone(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj_handle);

    return env->get_handle(object->clone(env->state()));
  }

  VALUE rb_inspect(VALUE obj_handle) {
    return rb_obj_as_string(rb_funcall(obj_handle, rb_intern("inspect"), 0, NULL));
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

  ID rb_intern(const char* string) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return reinterpret_cast<ID>(env->state()->symbol(string));
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

    return env->get_handle(object->get_ivar(env->state(),
                               prefixed_by("@", ivar_name)));
  }

  VALUE rb_ivar_set(VALUE self_handle, ID ivar_name, VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* receiver = env->get_object(self_handle);

    receiver->set_ivar(env->state(),
                       prefixed_by("@", ivar_name),
                       env->get_object(value));

    return value;
  }

  VALUE rb_ivar_defined(VALUE obj_handle, ID ivar_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Symbol* ivar = prefixed_by("@", ivar_name);
    Object* obj = env->get_object(obj_handle);
    Object* ivars = obj->get_ivars(env->state());

    Object* ret;
    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars)) {
      ret = tbl->has_key(env->state(), ivar);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars)) {
      ret = tbl->has_key(env->state(), ivar);
    } else {
      ret = Qfalse;
    }

    return env->get_handle(ret);
  }

  int rb_respond_to(VALUE obj_handle, ID method_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE result = rb_funcall(obj_handle,
                              rb_intern("respond_to?"),
                              1,
                              ID2SYM(method_name)) ;

    return RBX_RTEST(env->get_object(result));
  }
}
