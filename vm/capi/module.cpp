#include "builtin/object.hpp"
#include "builtin/module.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/autoload.hpp"

#include "configuration.hpp"

#include "helpers.hpp"
#include "call_frame.hpp"
#include "exception_point.hpp"
#include "on_stack.hpp"
#include "version.h"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

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
    if(LANGUAGE_18_ENABLED) {
      return rb_funcall(module_handle,
          rb_intern("const_defined?"), 1, ID2SYM(const_id));
    } else {
      return rb_funcall(module_handle,
          rb_intern("const_defined?"), 2, ID2SYM(const_id), Qfalse);
    }
  }

  ID rb_frame_last_func() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    CallFrame* rcf = env->current_call_frame()->previous->top_ruby_frame();

    return env->get_handle(rcf->name());
  }

  ID rb_frame_this_func() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    CallFrame* rcf = env->current_call_frame()->top_ruby_frame();

    return env->get_handle(rcf->name());
  }

  static VALUE const_missing(VALUE klass, ID id) {
    return rb_funcall(klass, rb_intern("const_missing"), 1, ID2SYM(id));
  }

  VALUE rb_const_get_at(VALUE module_handle, ID id_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    State* state = env->state();
    Symbol* name = reinterpret_cast<Symbol*>(id_name);
    Module* module = c_as<Module>(env->get_object(module_handle));

    ConstantMissingReason reason = vNonExistent;
    Object* val = module->get_const(state, name, G(sym_private), &reason);

    if(reason != vFound) return const_missing(module_handle, id_name);

    if(Autoload* autoload = try_as<Autoload>(val)) {
      return capi_fast_call(env->get_handle(autoload), rb_intern("call"), 0);
    }

    return env->get_handle(val);
  }

  VALUE rb_const_get_from(VALUE module_handle, ID id_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    State* state = env->state();
    Symbol* name = reinterpret_cast<Symbol*>(id_name);
    Module* module = c_as<Module>(env->get_object(module_handle));

    ConstantMissingReason reason = vNonExistent;
    while(!module->nil_p()) {
      Object* val = module->get_const(state, name, G(sym_private), &reason);
      if(reason == vFound) {
        if(Autoload* autoload = try_as<Autoload>(val)) {
          return capi_fast_call(env->get_handle(autoload), rb_intern("call"), 0);
        }

        return env->get_handle(val);
      }

      module = module->superclass();
    }

    return const_missing(module_handle, id_name);
  }

  VALUE rb_const_get(VALUE module_handle, ID id_name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    State* state = env->state();
    Symbol* name = reinterpret_cast<Symbol*>(id_name);
    Module* module = c_as<Module>(env->get_object(module_handle));

    ConstantMissingReason reason = vNonExistent;
    while(!module->nil_p()) {
      Object* val = module->get_const(state, name, G(sym_private), &reason);
      if(reason == vFound) {
        if(Autoload* autoload = try_as<Autoload>(val)) {
          return capi_fast_call(env->get_handle(autoload), rb_intern("call"), 0);
        }

        return env->get_handle(val);
      }

      module = module->superclass();
    }

    // Try from Object as well.
    module = G(object);

    while(!module->nil_p()) {
      Object* val = module->get_const(state, name, G(sym_private), &reason);
      if(reason == vFound) {
        if(Autoload* autoload = try_as<Autoload>(val)) {
          return capi_fast_call(env->get_handle(autoload), rb_intern("call"), 0);
        }

        return env->get_handle(val);
      }

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

  void rb_alias(VALUE module_handle, ID id_new, ID id_old) {
    rb_funcall(module_handle, rb_intern("alias_method"), 2, id_new, id_old);
  }

  void rb_define_alloc_func(VALUE class_handle, CApiAllocFunction allocator) {
    rb_define_singleton_method(class_handle, "allocate", allocator, 0);
  }

  void rb_undef_alloc_func(VALUE class_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    rb_undef_method(
        env->get_handle(env->get_object(class_handle)->singleton_class(env->state())),
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

    LEAVE_CAPI(env->state());
    Module* module = NULL;

    // Create a scope so we know that the OnStack variables are popped off
    // before we possibly make a longjmp. Making a longjmp doesn't give
    // any guarantees about destructors being run
    {
      GCTokenImpl gct;
      OnStack<2> os(env->state(), parent, constant);

      module = rubinius::Helpers::open_module(env->state(), gct,
          env->current_call_frame(), parent, constant);
    }

    // The call above could have triggered an Autoload resolve, which may
    // raise an exception, so we have to check the value returned.
    if(!module) env->current_ep()->return_to(env);

    // Grab the module handle before grabbing the lock
    // so the Module isn't accidentally GC'ed.
    VALUE module_handle = env->get_handle(module);
    ENTER_CAPI(env->state());

    return module_handle;
  }

  void rb_include_module(VALUE includer_handle, VALUE includee_handle) {
    rb_funcall(includer_handle, rb_intern("include"), 1, includee_handle);
  }

  void rb_undef_method(VALUE module_handle, const char* name) {
    rb_funcall(module_handle, rb_intern("undef_method!"), 1, ID2SYM(rb_intern(name)));
  }

  void rb_undef(VALUE handle, ID name) {
    Symbol* sym = reinterpret_cast<Symbol*>(name);
    rb_funcall(handle, rb_intern("undef_method!"), 1, sym);
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

  const char* rb_class2name(VALUE module_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Module* module_object = c_as<Module>(env->get_object(module_handle));

    String* str = module_object->get_name(env->state());
    return RSTRING_PTR(env->get_handle(str));
  }
}
