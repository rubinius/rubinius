#include "builtin/object.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"

#include "helpers.hpp"
#include "call_frame.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  int rb_const_defined(VALUE module_handle, ID const_id) {
    VALUE ret = rb_funcall(module_handle,
        rb_intern("const_defined?"), 1, ID2SYM(const_id));

    // This version also checks Object. le sigh.
    if(!RTEST(ret)) {
      ret = rb_funcall(rb_cObject,
          rb_intern("const_defined?"), 1, ID2SYM(const_id));
    }

    return ret;
  }

  int rb_const_defined_at(VALUE module_handle, ID const_id) {
    return rb_funcall(module_handle,
        rb_intern("const_defined?"), 1, ID2SYM(const_id));
  }

  ID rb_frame_last_func() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return reinterpret_cast<ID>(env->current_call_frame()->name());
  }

  static VALUE const_missing(VALUE klass, ID id) {
    return rb_funcall(klass, rb_intern("const_missing"), 1, ID2SYM(id));
  }

  VALUE rb_const_get_at(VALUE module_handle, ID id_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Symbol* name = reinterpret_cast<Symbol*>(id_name);
    Module* module = c_as<Module>(env->get_object(module_handle));

    bool found = false;
    Object* val = module->get_const(env->state(), name, &found);
    if(found) return env->get_handle(val);

    return const_missing(module_handle, id_name);
  }

  VALUE rb_const_get_from(VALUE module_handle, ID id_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Symbol* name = reinterpret_cast<Symbol*>(id_name);
    Module* module = c_as<Module>(env->get_object(module_handle));

    bool found = false;
    while(!module->nil_p()) {
      Object* val = module->get_const(env->state(), name, &found);
      if(found) return env->get_handle(val);

      module = module->superclass();
    }

    return const_missing(module_handle, id_name);
  }

  VALUE rb_const_get(VALUE module_handle, ID id_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Symbol* name = reinterpret_cast<Symbol*>(id_name);
    Module* module = c_as<Module>(env->get_object(module_handle));

    bool found = false;
    while(!module->nil_p()) {
      Object* val = module->get_const(env->state(), name, &found);
      if(found) return env->get_handle(val);

      module = module->superclass();
    }

    // Try from Object as well.
    module = env->state()->globals().object.get();

    while(!module->nil_p()) {
      Object* val = module->get_const(env->state(), name, &found);
      if(found) return env->get_handle(val);

      module = module->superclass();
    }

    return const_missing(module_handle, id_name);
  }

  void rb_const_set(VALUE module_handle, ID name, VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* module = c_as<Module>(env->get_object(module_handle));
    Object* object = env->get_object(obj_handle);

    module->set_const(env->state(), reinterpret_cast<Symbol*>(name),  object);
  }

  void rb_define_alias(VALUE module_handle, const char* new_name, const char* old_name) {
    ID id_new = rb_intern(new_name);
    ID id_old = rb_intern(old_name);

    rb_funcall(module_handle, rb_intern("alias_method"), 2, id_new, id_old);
  }

  void rb_define_alloc_func(VALUE class_handle, CApiAllocFunction allocator) {
    rb_define_singleton_method(class_handle, "allocate", allocator, 0);
  }

  void rb_undef_alloc_func(VALUE class_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    rb_undef_method(
        env->get_handle(env->get_object(class_handle)->metaclass(env->state())),
        "allocate");
  }

  void rb_define_attr(VALUE module_handle, const char* attr_name, int readable, int writable) {
    if(readable) {
      rb_funcall(module_handle, rb_intern("attr_reader"), 1, rb_intern(attr_name));
    }

    if(writable) {
      rb_funcall(module_handle, rb_intern("attr_writer"), 1, rb_intern(attr_name));
    }
  }

  void rb_define_const(VALUE module_handle, const char* name, VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* module = c_as<Module>(env->get_object(module_handle));
    Object* object = env->get_object(obj_handle);

    module->set_const(env->state(), name,  object);
  }

  void rb_define_global_const(const char* name, VALUE obj) {
    rb_define_const(rb_cObject, name, obj);
  }

  void rb_define_global_function(const char* name, CApiGenericFunction func, int argc) {
    rb_define_module_function(rb_mKernel, name, func, argc);
  }

  VALUE rb_define_module(const char* name) {
    return rb_define_module_under(rb_cObject, name);
  }

  void rb_define_module_function(VALUE module_handle, const char* name,
      CApiGenericFunction func, int args) {
    rb_define_private_method(module_handle, name, func, args);
    rb_define_singleton_method(module_handle, name, func, args);
  }

  /** @note   Shares code with rb_define_class_under, change there too. --rue */
  VALUE rb_define_module_under(VALUE parent_handle, const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* parent = c_as<Module>(env->get_object(parent_handle));
    Symbol* constant = env->state()->symbol(name);

    Module* module = rubinius::Helpers::open_module(env->state(),
        env->current_call_frame(), parent, constant);

    return env->get_handle(module);
  }

  void rb_include_module(VALUE includer_handle, VALUE includee_handle) {
    rb_funcall(includer_handle, rb_intern("include"), 1, includee_handle);
  }

  void rb_undef_method(VALUE module_handle, const char* name) {
    rb_funcall(module_handle, rb_intern("undef_method!"), 1, ID2SYM(rb_intern(name)));
  }

  void rb_undef(VALUE handle, ID name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Symbol* sym = reinterpret_cast<Symbol*>(name);
    rb_undef_method(handle, sym->c_str(env->state()));
    // In MRI, rb_undef also calls the undef_method hooks, maybe we should?
  }

  VALUE rb_mod_ancestors(VALUE mod) {
    return rb_funcall(mod, rb_intern("ancestors"), 0);
  }

  VALUE rb_mod_name(VALUE mod) {
    return rb_funcall(mod, rb_intern("name"), 0);
  }

  VALUE rb_module_new(void) {
    return rb_funcall(rb_cModule, rb_intern("new"), 0);
  }

  VALUE rb_mod_remove_const(VALUE mod, VALUE name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* module = c_as<Module>(env->get_object(mod));
    module->del_const(env->state(), reinterpret_cast<Symbol*>(name));
    return Qnil;
  }
}
