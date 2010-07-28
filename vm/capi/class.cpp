#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"

#include "helpers.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_class_new_instance(int arg_count, VALUE* args, VALUE class_handle) {
    return rb_funcall2(class_handle, rb_intern("new"), arg_count, args);
  }

  VALUE rb_class_of(VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* class_object = env->get_object(object_handle)->class_object(env->state());
    return env->get_handle(class_object);
  }

  // MUST return the immediate object in the class field, not the real class!
  VALUE CLASS_OF(VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* class_object = env->get_object(object_handle)->lookup_begin(env->state());
    return env->get_handle(class_object);
  }

  VALUE rb_class_name(VALUE class_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* class_object = c_as<Class>(env->get_object(class_handle));
    return env->get_handle(class_object->name()->to_str(env->state()));
  }

  VALUE rb_class_inherited(VALUE super_handle, VALUE class_handle)
  {
    if(!super_handle) super_handle = rb_cObject;
    return rb_funcall(super_handle, rb_intern("inherited"), 1, class_handle);
  }

  VALUE rb_class_new(VALUE super_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(super_handle == rb_cClass) {
      rb_raise(rb_eTypeError, "can't make subclass of Class");
    }

    if(try_as<MetaClass>(env->get_object(super_handle))) {
      rb_raise(rb_eTypeError, "can't make subclass of virtual class");
    }

    Class* klass = Class::create(env->state(), c_as<Class>(env->get_object(super_handle)));

    return env->get_handle(klass);
  }

  VALUE rb_path2class(const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* mod = env->state()->shared.globals.object.get();

    char* base = strdup(name);
    char* str = base;
    char* pos = strstr(str, "::");

    while(pos) {
      pos[0] = 0;

      mod = try_as<Module>(mod->get_const(env->state(), str));
      if(!mod) {
        free(base);
        capi_raise_type_error(Module::type, mod);
        return Qnil;
      }

      str = pos+2;
      pos = strstr(str, "::");
    }

    Object* val = mod->get_const(env->state(), str);

    free(base);

    // Make sure val is a module.
    if(!kind_of<Module>(val)) {
      capi_raise_type_error(Module::type, val);
    }

    return env->get_handle(val);
  }

  VALUE rb_cv_get(VALUE module_handle, const char* name) {
    return rb_cvar_get(module_handle, rb_intern(name));
  }

  VALUE rb_cv_set(VALUE module_handle, const char* name, VALUE value) {
    return rb_cvar_set(module_handle, rb_intern(name), value, 0);
  }

  VALUE rb_cvar_defined(VALUE module_handle, ID name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if (((Symbol *)name)->is_cvar_p(env->state())->true_p()) {
       return rb_funcall(module_handle, rb_intern("class_variable_defined?"),
                         1, name);
     } else {
      return rb_funcall(module_handle, rb_intern("instance_variable_defined?"),
                        1, name);
     }
  }

  VALUE rb_cvar_get(VALUE module_handle, ID name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return rb_funcall(module_handle, rb_intern("class_variable_get"),
                      1,
                      env->get_handle(prefixed_by(env->state(), "@@", 2, name)));
  }

  VALUE rb_cvar_set(VALUE module_handle, ID name, VALUE value, int unused) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return rb_funcall(module_handle, rb_intern("class_variable_set"),
                      2,
                      env->get_handle(prefixed_by(env->state(), "@@", 2, name)),
                      value);
  }

  VALUE rb_define_class(const char* name, VALUE superclass_handle) {
    return rb_define_class_under(rb_cObject, name, superclass_handle);
  }

  void rb_define_class_variable(VALUE klass, const char* name, VALUE val) {
    ID id = rb_intern(name);
    rb_cvar_set(klass, id,  val, 0);
  }

  /** @note   Shares code with rb_define_module_under, change there too. --rue */
  VALUE rb_define_class_under(VALUE outer, const char* name, VALUE super) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* module = c_as<Module>(env->get_object(outer));
    Class* superclass = c_as<Class>(env->get_object(super ? super : rb_cObject));
    Symbol* constant = env->state()->symbol(name);

    bool created = false;
    VALUE klass = env->get_handle(rubinius::Helpers::open_class(env->state(),
        env->current_call_frame(), module, superclass, constant, &created));

    if(super) rb_funcall(super, rb_intern("inherited"), 1, klass);

    return klass;
  }

  void rb_attr(VALUE klass, ID id, int read, int write, int ex) {
    // TODO don't ignore ex.
    if(read) {
      rb_funcall(klass, rb_intern("attr_reader"), 1, ID2SYM(id));
    }

    if(write) {
      rb_funcall(klass, rb_intern("attr_writer"), 1, ID2SYM(id));
    }
  }

  /** @todo   Should this be a global handle? Surely not.. --rue */
  VALUE rb_singleton_class(VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Class* metaclass = env->get_object(object_handle)->metaclass(env->state());
    return env->get_handle(metaclass);
  }
}
