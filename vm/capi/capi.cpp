#include "config.h"

#include "builtin/array.hpp"
#include "builtin/data.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/module.hpp"
#include "builtin/native_method.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/location.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/proc.hpp"
#include "builtin/exception.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"
#include "lookup_data.hpp"
#include "dispatch.hpp"
#include "arguments.hpp"

#include "exception_point.hpp"
#include "global_cache.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

namespace rubinius {
  namespace capi {

    /**
     * This looks like a complicated scheme but there is a reason for
     * doing it this way. In MRI, rb_cObject, etc. are all global data.
     * We need to avoid global data to better support embedding and
     * other features like MVM. @see capi_get_constant().
     */
    std::string capi_get_constant_name(int type) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      if(type < 0 || type >= cCApiMaxConstant) {
        rb_raise(env->get_handle(env->state()->globals().exception.get()),
              "C-API: invalid constant index");
      }

      CApiConstantNameMap map = env->state()->shared().capi_constant_name_map();
      return map[type];
    }

    bool capi_check_interrupts(STATE, CallFrame* call_frame, void* end) {
      if(!state->check_stack(call_frame, end)) {
        return false;
      }

      if(unlikely(state->check_local_interrupts())) {
        if(!state->process_async(call_frame)) return false;
      }
      return true;
    }

    /**
     *  Common implementation for rb_funcall*
     */
    VALUE capi_funcall_backend(const char* file, int line,
                               VALUE receiver, ID method_name,
                               size_t arg_count, VALUE* arg_array)
    {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      env->flush_cached_data();

      Array* args = Array::create(env->state(), arg_count);
      for(size_t i = 0; i < arg_count; i++) {
        args->set(env->state(), i, env->get_object(arg_array[i]));
      }

      Object* blk = cNil;

      if(VALUE blk_handle = env->outgoing_block()) {
        blk = env->get_object(blk_handle);
        env->set_outgoing_block(0);
      }

      Object* recv = env->get_object(receiver);

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = recv->send(env->state(), env->current_call_frame(),
          reinterpret_cast<Symbol*>(method_name), args, blk);

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = env->get_handle(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    VALUE capi_funcall_backend_native(NativeMethodEnvironment* env,
                                      const char* file, int line,
                                      Object* recv, Symbol* method,
                                      size_t arg_count,
                                      Object** args, Object* block)
    {
      int marker = 0;
      if(!capi_check_interrupts(env->state(), env->current_call_frame(), &marker)) {
        env->current_ep()->return_to(env);
      }

      env->flush_cached_data();

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = cNil;

      // Run in a block so objects are properly deconstructed when we
      // do a longjmp because of an exception.
      {
        LookupData lookup(recv, recv->lookup_begin(env->state()), env->state()->globals().sym_private.get());
        Arguments args_o(method, recv, block, arg_count, args);
        Dispatch dis(method);

        ret = dis.send(env->state(), env->current_call_frame(),
                      lookup, args_o);
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = env->get_handle(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    VALUE capi_fast_call(VALUE receiver, ID method_name, int arg_count, ...) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      va_list varargs;
      va_start(varargs, arg_count);

      Object* args[arg_count];

      for(int i = 0; i < arg_count; i++) {
        args[i] = env->get_object(va_arg(varargs, VALUE));
      }

      va_end(varargs);

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* recv = env->get_object(receiver);
      Symbol* method = (Symbol*)method_name;

      Object* ret = cNil;
      // Run in block so we properly deconstruct objects allocated
      // on the stack if we do a longjmp because of an exception.
      {
        LookupData lookup(recv, recv->lookup_begin(env->state()), env->state()->globals().sym_private.get());
        Arguments args_o(method, recv, cNil, arg_count, args);
        Dispatch dis(method);

        ret = dis.send(env->state(), env->current_call_frame(),
                       lookup, args_o);
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = env->get_handle(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    VALUE capi_yield_backend(NativeMethodEnvironment* env,
                              Object* blk,
                              size_t arg_count, Object** arg_vals)
    {
      int marker = 0;
      if(!capi_check_interrupts(env->state(), env->current_call_frame(), &marker)) {
        env->current_ep()->return_to(env);
      }

      env->flush_cached_data();

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = cNil;
      STATE = env->state();

      CallFrame* call_frame = env->current_call_frame();

      // Run in separate block so the arguments are destructed
      // properly before we make a potential longjmp.
      {
        Arguments args(G(sym_call), blk, arg_count, arg_vals);

        if(BlockEnvironment* be = try_as<BlockEnvironment>(blk)) {
          ret = be->call(state, call_frame, args);
        } else if(Proc* proc = try_as<Proc>(blk)) {
          ret = proc->yield(state, call_frame, args);
        } else if(blk->nil_p()) {
          state->raise_exception(Exception::make_lje(state, call_frame));
          ret = NULL;
        } else {
          Dispatch dis(G(sym_call));
          ret = dis.send(state, call_frame, args);
        }
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = env->get_handle(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    VALUE capi_call_super_native(NativeMethodEnvironment* env,
                                 size_t arg_count, Object** args)
    {
      int marker = 0;
      if(!capi_check_interrupts(env->state(), env->current_call_frame(), &marker)) {
        env->current_ep()->return_to(env);
      }

      env->flush_cached_data();

      NativeMethodFrame* frame = NativeMethodFrame::current();

      Object* recv = env->get_object(frame->receiver());
      Module* mod =  c_as<Module>(env->get_object(frame->module()));
      Symbol* name = c_as<NativeMethod>(env->get_object(frame->method()))->name();

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = cNil;
      // Use a block objects on the stack are properly deconstructed when
      // we do a potential longjmp.
      {
        LookupData lookup(recv, mod->superclass(), env->state()->globals().sym_private.get());
        Arguments args_o(name, recv, arg_count, args);
        Dispatch dis(name);

        ret = dis.send(env->state(), env->current_call_frame(),
                       lookup, args_o);
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = env->get_handle(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    /** Make sure the name has the given prefix. */
    Symbol* prefixed_by(std::string prefix, std::string name) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      if(name.compare(0UL, prefix.size(), prefix) != 0) {
        std::ostringstream str;
        str << prefix << name;
        name.assign(str.str());
      }

      return env->state()->symbol(name.c_str());
    }

    /** Make sure the name has the given prefix. */
    Symbol* prefixed_by(STATE, const char* prefix, size_t len, ID name) {
      Symbol* sym_obj = reinterpret_cast<Symbol*>(name);
      std::string& sym = state->shared().symbols.lookup_cppstring(sym_obj);

      if(sym.compare(0UL, len, prefix) == 0) return sym_obj;

      std::ostringstream stream;
      stream << prefix << sym;

      return state->symbol(stream.str().c_str());
    }

    Symbol* prefixed_by(STATE, const char prefix, ID name) {
      Symbol* sym_obj = reinterpret_cast<Symbol*>(name);
      std::string& sym = state->shared().symbols.lookup_cppstring(sym_obj);

      if(sym.c_str()[0] == prefix) return sym_obj;

      std::ostringstream stream;
      stream << prefix << sym;

      return state->symbol(stream.str().c_str());
    }

    void capi_raise_runtime_error(const char* reason) {
      rb_raise(rb_eRuntimeError, reason);
    }

    void capi_raise_type_error(object_type type, Object* object) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      TypeInfo* expected = env->state()->vm()->find_type(type);
      TypeInfo* actual = env->state()->vm()->find_type(object->get_type());

      rb_raise(rb_eTypeError, "wrong argument type %s (expected %s)",
          actual->type_name.c_str(), expected->type_name.c_str());
    }

    void capi_raise_backend(Exception* exception) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      exception->locations(env->state(), Location::from_call_stack(env->state(),
                           env->current_call_frame()));
      env->state()->raise_exception(exception);

      env->current_ep()->return_to(env);
    }

    void capi_raise_backend(VALUE exception) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      Exception *exc = capi::c_as<Exception>(env->get_object(exception));
      capi_raise_backend(exc);
    }

    void capi_raise_backend(VALUE klass, const char* reason) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      Class* cls = c_as<Class>(env->get_object(klass));
      Exception* exc = Exception::make_exception(env->state(), cls, reason);
      capi_raise_backend(exc);
    }

    void capi_raise_break(VALUE obj) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      env->state()->vm()->thread_state()->raise_break(env->get_object(obj), env->scope()->parent());
      env->current_ep()->return_to(env);
    }

    Proc* wrap_c_function(void* cb, VALUE cb_data, int arity) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      NativeMethod* nm = NativeMethod::create(env->state(),
                          nil<String>(), env->state()->vm()->shared.globals.rubinius.get(),
                          env->state()->symbol("call"), cb,
                          Fixnum::from(arity), 0);

      nm->set_ivar(env->state(), env->state()->symbol("cb_data"),
                   env->get_object(cb_data));

      Object* current_block = env->block();
      if(!current_block->nil_p()) {
        nm->set_ivar(env->state(), env->state()->symbol("original_block"),
                     current_block);
      }

      Proc* prc = Proc::create(env->state(), env->state()->vm()->shared.globals.proc.get());
      prc->bound_method(env->state(), nm);

      return prc;
    }
  }
}

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE capi_get_constant(CApiConstant type) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    env->shared().capi_constant_lock().lock();
    CApiConstantHandleMap& map = env->state()->shared().capi_constant_handle_map();

    CApiConstantHandleMap::iterator entry = map.find(type);
    if(entry == map.end()) {
      std::string constant_name = capi_get_constant_name(type);

      VALUE val = rb_path2class(constant_name.c_str());
      capi::Handle* hdl = capi::Handle::from(val);
      hdl->ref(); // Extra ref, since we save it in the map
      map[type] = hdl;
      env->shared().capi_constant_lock().unlock();
      return val;
    } else {
      VALUE val = entry->second->as_value();
      env->shared().capi_constant_lock().unlock();
      return val;
    }
  }

  VALUE rb_call_super(int argc, const VALUE *argv) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[argc];
    for(int i = 0; i < argc; i++) {
      args[i] = env->get_object(argv[i]);
    }

    return capi_call_super_native(env, argc, args);
  }

  VALUE rb_funcall(VALUE receiver, ID method_name, int arg_count, ...) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    va_list varargs;
    va_start(varargs, arg_count);

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = env->get_object(va_arg(varargs, VALUE));
    }

    va_end(varargs);

    Object* blk = cNil;

    if(VALUE blk_handle = env->outgoing_block()) {
      blk = env->get_object(blk_handle);
      env->set_outgoing_block(0);
    }

    return capi_funcall_backend_native(env, "", 0,
        env->get_object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, blk);
  }

  VALUE rb_funcall2(VALUE receiver, ID method_name, int arg_count, const VALUE* v_args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = env->get_object(v_args[i]);
    }

    Object* blk = cNil;

    if(VALUE blk_handle = env->outgoing_block()) {
      blk = env->get_object(blk_handle);
      env->set_outgoing_block(0);
    }

    return capi_funcall_backend_native(env, "", 0,
        env->get_object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, blk);
  }

  VALUE rb_funcall2b(VALUE receiver, ID method_name, int arg_count,
                     const VALUE* v_args, VALUE block)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = env->get_object(v_args[i]);
    }

    return capi_funcall_backend_native(env, "", 0,
        env->get_object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, env->get_object(block));
  }

  VALUE rb_yield(VALUE argument_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* blk = env->block();

    if(!CBOOL(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    Object* arg = env->get_object(argument_handle);

    return capi_yield_backend(env, blk, 1, &arg);
  }

  VALUE rb_yield_values(int n, ...) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* blk = env->block();

    if(!CBOOL(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    if(n == 0) {
      return capi_yield_backend(env, blk, 0, 0);
    }

    Object* vars[n];

    va_list args;
    va_start(args, n);

    for(int i = 0; i < n; ++i) {
      vars[i] = env->get_object(va_arg(args, VALUE));
    }

    va_end(args);

    return capi_yield_backend(env, blk, n, vars);
  }

  VALUE rb_yield_splat(VALUE array_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* blk = env->block();

    if(!CBOOL(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    if(Array* ary = try_as<Array>(env->get_object(array_handle))) {
      int count = ary->size();
      Object* vars[count];

      for(int i = 0; i < count; i++) {
        vars[i] = ary->get(env->state(), i);
      }

      return capi_yield_backend(env, blk, count, vars);
    }

    return capi_yield_backend(env, blk, 0, 0);
  }

  int rb_block_given_p() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return CBOOL(env->block());
  }

  void rb_need_block() {
    if(!rb_block_given_p()) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }
  }

  VALUE rb_block_call(VALUE obj, ID meth, int argc, VALUE* argv,
                      VALUE(*cb)(ANYARGS), VALUE cb_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(cb) {
      Proc* prc = capi::wrap_c_function((void*)cb, cb_data, C_BLOCK_CALL);
      env->set_outgoing_block(env->get_handle(prc));
    } else {
      env->set_outgoing_block(env->get_handle(env->block()));
    }

    return rb_funcall2(obj, meth, argc, argv);
  }

  VALUE rb_apply(VALUE recv, ID mid, VALUE args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    env->flush_cached_data();

    Array* ary = capi::c_as<Array>(env->get_object(args));

    Object* obj = env->get_object(recv);

    // Unlock, we're leaving extension code.
    LEAVE_CAPI(env->state());

    Object* ret = obj->send(env->state(), env->current_call_frame(),
        reinterpret_cast<Symbol*>(mid), ary, cNil);

    // We need to get the handle for the return value before getting
    // the GEL so that ret isn't accidentally GCd while we wait.
    VALUE ret_handle = 0;
    if(ret) ret_handle = env->get_handle(ret);

    // Re-entering extension code
    ENTER_CAPI(env->state());

    env->update_cached_data();

    // An exception occurred
    if(!ret) env->current_ep()->return_to(env);

    return ret_handle;
  }


  void capi_infect(VALUE h, VALUE s) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* host   = env->get_object(h);
    Object* source = env->get_object(s);

    source->infect(env->state(), host);
  }

  int capi_nil_p(VALUE expression_result) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return env->get_object(expression_result)->nil_p();
  }

  void capi_taint(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    env->get_object(obj)->taint(env->state());
  }

  int rb_obj_tainted(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* tainted = env->get_object(obj)->tainted_p(env->state());

    return tainted->true_p() ? 1 : 0;
  }

  void capi_define_method(const char* file, VALUE target,
                          const char* name, CApiGenericFunction fptr,
                          int arity, CApiMethodKind kind)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    State* state = env->state();
    Symbol* method_name = state->symbol(name);

    Module* module = NULL;

    if(kind == cCApiSingletonMethod) {
      module = c_as<Module>(env->get_object(target)->singleton_class(env->state()));
    } else {
      module = c_as<Module>(env->get_object(target));
    }

    NativeMethod* method = NULL;
    method = NativeMethod::create(state, String::create(state, file),
                                  module, method_name,
                                  (void*)fptr, Fixnum::from(arity),
                                  env->current_native_frame()->capi_lock_index());

    Symbol* visibility;

    switch(kind) {
    case cCApiPrivateMethod:
      visibility = G(sym_private);
      break;

    case cCApiProtectedMethod:
      visibility = G(sym_protected);
      break;

    default:  /* Also catches singletons for now. @todo Verify OK. --rue */
      visibility = G(sym_public);
      break;
    }

    module->add_method(state, method_name, method, visibility);
    System::vm_reset_method_cache(env->state(), module, method_name, env->current_call_frame());
  }

  VALUE capi_class_superclass(VALUE class_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Module* module = c_as<Module>(env->get_object(class_handle));
    Module* super = module->superclass();

    if(super->nil_p()) {
      /* In MRI, the superclass chain terminates with a NULL pointer.
       * Since VALUE is an intptr_t in Rubinius, we use 0 instead.
       */
      return 0;
    } else {
      return env->get_handle(super);
    }
  }

  /* For debugging. */
  void __show_subtend__(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj_handle);
    if(!object) {
      std::cout << "the object is NULL, check if an exception was raised." << std::endl;
      return;
    }
    object->show(env->state());
  }
}
