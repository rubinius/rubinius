#include "config.h"

#include "class/array.hpp"
#include "class/lexical_scope.hpp"
#include "class/data.hpp"
#include "class/fixnum.hpp"
#include "class/lookup_table.hpp"
#include "class/module.hpp"
#include "class/native_method.hpp"
#include "class/object.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/system.hpp"
#include "class/location.hpp"
#include "class/block_environment.hpp"
#include "class/proc.hpp"
#include "class/exception.hpp"
#include "class/unwind_state.hpp"

#include "bug.hpp"
#include "c_api.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "lookup_data.hpp"
#include "dispatch.hpp"
#include "arguments.hpp"

#include "exception_point.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

namespace rubinius {
  using namespace capi;

  NORETURN(static void abort_memory_handle_error(const char* error));

  static void abort_memory_handle_error(const char* error) {
    /* This is one of the rare cases where aborting with rubinius::bug is
     * acceptable. We MUST be able to do the conversions performed by the
     * functions that may call this abort function. We CANNOT raise exceptions
     * inside components like the GC because certain state invariants must be
     * maintained and those operations cannot be interrupted.
     */
    rubinius::bug(error);
  }

  MemoryHandle* MemoryHandle::from(VALUE value) {
    if(REFERENCE_P(value)) {
      return reinterpret_cast<MemoryHandle*>(STRIP_HANDLE_TAG(value));
    }

    abort_memory_handle_error("MemoryHandle requested for unknown VALUE type");
  }

  Object* MemoryHandle::object(VALUE value) {
    if(REFERENCE_P(value)) {
      MemoryHandle* handle = MemoryHandle::from(value);

      return handle->object();
    } else if(FIXNUM_P(value) || SYMBOL_P(value)) {
      return reinterpret_cast<Object*>(value);
    } else if(TRUE_P(value)) {
      return cTrue;
    } else if(FALSE_P(value)) {
      return cFalse;
    } else if(NIL_P(value)) {
      return cNil;
    } else if(UNDEF_P(value)) {
      return cUndef;
    }

    abort_memory_handle_error("Object reference requested for unknown VALUE type");
  }

  VALUE MemoryHandle::value(Object* object) {
    if(object->reference_p()) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      MemoryHandle* handle = object->get_handle(env->state());
      handle->access();

      return handle->as_value();
    } else if(object->fixnum_p() || object->symbol_p()) {
      return reinterpret_cast<VALUE>(object);
    } else if(object->true_p()) {
      return Qtrue;
    } else if(object->false_p()) {
      return Qfalse;
    } else if(object->nil_p()) {
      return Qnil;
    } else if(object->undef_p()) {
      return Qundef;
    }

    abort_memory_handle_error("VALUE requested for unknown Object type");
  }

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
        rb_raise(MemoryHandle::value(env->state()->globals().exception.get()),
              "C-API: invalid constant index");
      }

      C_API::CApiConstantNameMap map = env->state()->c_api()->capi_constant_name_map();
      return map[type];
    }

    bool capi_check_interrupts(STATE) {
      void* stack_address;

      if(!state->check_stack(state, &stack_address)) {
        return false;
      }

      return !state->thread_interrupted_p();
    }

    /**
     *  Common implementation for rb_funcall*
     */
    VALUE capi_funcall_backend(const char* file, int line,
                               VALUE receiver, ID method_name,
                               size_t arg_count, VALUE* arg_array)
    {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      Array* args = Array::create(env->state(), arg_count);
      for(size_t i = 0; i < arg_count; i++) {
        args->set(env->state(), i, MemoryHandle::object(arg_array[i]));
      }

      Object* blk = cNil;

      if(VALUE blk_handle = env->outgoing_block()) {
        blk = MemoryHandle::object(blk_handle);
        env->set_outgoing_block(0);
      }

      Object* recv = MemoryHandle::object(receiver);

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = recv->send(env->state(),
          reinterpret_cast<Symbol*>(method_name), args, blk);

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = MemoryHandle::value(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    VALUE capi_funcall_backend_native(NativeMethodEnvironment* env,
                                      const char* file, int line,
                                      Object* recv, Symbol* method,
                                      size_t arg_count,
                                      Object** args, Object* block,
                                      bool allow_private)
    {
      if(!capi_check_interrupts(env->state())) {
        env->current_ep()->return_to(env);
      }

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = cNil;

      // Run in a block so objects are properly deconstructed when we
      // do a longjmp because of an exception.
      {
        Symbol* min_visibility;
        if(allow_private) {
          min_visibility = env->state()->globals().sym_private.get();
        } else {
          min_visibility = env->state()->globals().sym_public.get();
        }
        LookupData lookup(recv, recv->lookup_begin(env->state()), min_visibility);
        Arguments args_o(method, recv, block, arg_count, args);
        Dispatch dispatch(method);

        ret = dispatch.send(env->state(), lookup, args_o);
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = MemoryHandle::value(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

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
        args[i] = MemoryHandle::object(va_arg(varargs, VALUE));
      }

      va_end(varargs);

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* recv = MemoryHandle::object(receiver);
      Symbol* method = (Symbol*)method_name;

      Object* ret = cNil;
      // Run in block so we properly deconstruct objects allocated
      // on the stack if we do a longjmp because of an exception.
      {
        LookupData lookup(recv, recv->lookup_begin(env->state()), env->state()->globals().sym_private.get());
        Arguments args_o(method, recv, cNil, arg_count, args);
        Dispatch dispatch(method);

        ret = dispatch.send(env->state(), lookup, args_o);
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = MemoryHandle::value(ret);

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
      if(!capi_check_interrupts(env->state())) {
        env->current_ep()->return_to(env);
      }

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = cNil;
      STATE = env->state();

      // Run in separate block so the arguments are destructed
      // properly before we make a potential longjmp.
      {
        Arguments args(G(sym_call), blk, arg_count, arg_vals);

        if(BlockEnvironment* be = try_as<BlockEnvironment>(blk)) {
          ret = be->call(state, args);
        } else if(Proc* proc = try_as<Proc>(blk)) {
          ret = proc->yield(state, args);
        } else if(blk->nil_p()) {
          state->raise_exception(Exception::make_lje(state));
          ret = NULL;
        } else {
          Dispatch dispatch(G(sym_call));
          ret = dispatch.send(state, args);
        }
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = MemoryHandle::value(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return ret_handle;
    }

    VALUE capi_call_super_native(NativeMethodEnvironment* env,
                                 size_t arg_count, Object** args)
    {
      if(!capi_check_interrupts(env->state())) {
        env->current_ep()->return_to(env);
      }

      NativeMethodFrame* frame = NativeMethodFrame::current();

      Object* recv = MemoryHandle::object(frame->receiver());
      Module* mod =  MemoryHandle::object<Module>(frame->module());
      Symbol* name = MemoryHandle::object<NativeMethod>(frame->method())->name();

      // Unlock, we're leaving extension code.
      LEAVE_CAPI(env->state());

      Object* ret = cNil;
      // Use a block objects on the stack are properly deconstructed when
      // we do a potential longjmp.
      {
        LookupData lookup(recv, mod->superclass(), env->state()->globals().sym_private.get());
        Arguments args_o(name, recv, arg_count, args);
        Dispatch dispatch(name);

        ret = dispatch.send(env->state(), lookup, args_o);
      }

      // We need to get the handle for the return value before getting
      // the GEL so that ret isn't accidentally GCd while we wait.
      VALUE ret_handle = 0;
      if(ret) ret_handle = MemoryHandle::value(ret);

      // Re-entering extension code
      ENTER_CAPI(env->state());

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
      std::string& sym = state->memory()->symbols.lookup_cppstring(sym_obj);

      if(sym.compare(0UL, len, prefix) == 0) return sym_obj;

      std::ostringstream stream;
      stream << prefix << sym;

      return state->symbol(stream.str().c_str());
    }

    Symbol* prefixed_by(STATE, const char prefix, ID name) {
      Symbol* sym_obj = reinterpret_cast<Symbol*>(name);
      std::string& sym = state->memory()->symbols.lookup_cppstring(sym_obj);

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

      TypeInfo* expected = env->state()->memory()->find_type(type);
      TypeInfo* actual = env->state()->memory()->find_type(object->get_type());

      rb_raise(rb_eTypeError, "wrong argument type %s (expected %s)",
          actual->type_name.c_str(), expected->type_name.c_str());
    }

    void capi_raise_backend(Exception* exception) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      exception->locations(env->state(), Location::from_call_stack(env->state()));
      env->state()->raise_exception(exception);

      env->current_ep()->return_to(env);
    }

    void capi_raise_backend(VALUE exception) {
      Exception *exc = MemoryHandle::object<Exception>(exception);
      capi_raise_backend(exc);
    }

    void capi_raise_backend(VALUE klass, const char* reason) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      Class* cls = MemoryHandle::object<Class>(klass);
      Exception* exc = Exception::make_exception(env->state(), cls, reason);
      capi_raise_backend(exc);
    }

    void capi_raise_break(VALUE obj) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      env->state()->unwind_state()->raise_break(
          MemoryHandle::object(obj), env->scope()->parent());
      env->current_ep()->return_to(env);
    }

    Proc* wrap_c_function(void* cb, VALUE cb_data, int arity) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      NativeMethod* nm = NativeMethod::create(env->state(),
                          nil<String>(), env->state()->memory()->globals.rubinius.get(),
                          env->state()->symbol("call"), cb,
                          Fixnum::from(arity), 0);

      nm->set_ivar(env->state(), env->state()->symbol("cb_data"),
                   MemoryHandle::object(cb_data));

      Object* current_block = env->block();
      if(!current_block->nil_p()) {
        nm->set_ivar(env->state(), env->state()->symbol("original_block"),
                     current_block);
      }

      Proc* prc = Proc::create(env->state(), env->state()->memory()->globals.proc.get());
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

    env->state()->c_api()->capi_constant_lock().lock();
    C_API::CApiConstantHandleMap& map = env->state()->c_api()->capi_constant_map();

    VALUE value;

    C_API::CApiConstantHandleMap::iterator entry = map.find(type);
    if(entry == map.end()) {
      std::string constant_name = capi_get_constant_name(type);

      value = rb_path2class(constant_name.c_str());
      MemoryHandle* handle = MemoryHandle::from(value);
      Object* object = handle->object();

      if(object->reference_p()) {
        object->add_reference(env->state());
      }

      map[type] = handle;
    } else {
      value = entry->second->as_value();
    }

    env->state()->c_api()->capi_constant_lock().unlock();
    return value;
  }

  VALUE rb_call_super(int argc, const VALUE *argv) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[argc];
    for(int i = 0; i < argc; i++) {
      args[i] = MemoryHandle::object(argv[i]);
    }

    return capi_call_super_native(env, argc, args);
  }

  VALUE rb_funcall(VALUE receiver, ID method_name, int arg_count, ...) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    va_list varargs;
    va_start(varargs, arg_count);

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = MemoryHandle::object(va_arg(varargs, VALUE));
    }

    va_end(varargs);

    Object* blk = cNil;

    if(VALUE blk_handle = env->outgoing_block()) {
      blk = MemoryHandle::object(blk_handle);
      env->set_outgoing_block(0);
    }

    return capi_funcall_backend_native(env, "", 0,
        MemoryHandle::object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, blk, true);
  }

  VALUE rb_funcall2(VALUE receiver, ID method_name, int arg_count, const VALUE* v_args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = MemoryHandle::object(v_args[i]);
    }

    Object* blk = cNil;

    if(VALUE blk_handle = env->outgoing_block()) {
      blk = MemoryHandle::object(blk_handle);
      env->set_outgoing_block(0);
    }

    return capi_funcall_backend_native(env, "", 0,
        MemoryHandle::object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, blk, true);
  }

  VALUE rb_funcall3(VALUE receiver, ID method_name, int arg_count, const VALUE* v_args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = MemoryHandle::object(v_args[i]);
    }

    Object* blk = cNil;

    if(VALUE blk_handle = env->outgoing_block()) {
      blk = MemoryHandle::object(blk_handle);
      env->set_outgoing_block(0);
    }

    return capi_funcall_backend_native(env, "", 0,
        MemoryHandle::object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, blk, false);
  }

  static VALUE call_function_with_block(VALUE receiver, ID method_name, int arg_count,
                                        const VALUE* v_args, VALUE block, bool allow_private)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* args[arg_count];

    for(int i = 0; i < arg_count; i++) {
      args[i] = MemoryHandle::object(v_args[i]);
    }

    return capi_funcall_backend_native(env, "", 0,
        MemoryHandle::object(receiver),
        reinterpret_cast<Symbol*>(method_name),
        arg_count, args, MemoryHandle::object(block), allow_private);
  }

  VALUE rb_funcall2b(VALUE receiver, ID method_name, int arg_count,
                     const VALUE* v_args, VALUE block)
  {
    return call_function_with_block(receiver, method_name, arg_count, v_args, block, true);
  }

  VALUE rb_funcall_with_block(VALUE receiver, ID method_name, int arg_count,
                     const VALUE* v_args, VALUE block)
  {
    return call_function_with_block(receiver, method_name, arg_count, v_args, block, false);
  }

  VALUE rb_yield(VALUE argument_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* blk = env->block();

    if(!CBOOL(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    Object* arg = MemoryHandle::object(argument_handle);

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
      vars[i] = MemoryHandle::object(va_arg(args, VALUE));
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

    if(Array* ary = MemoryHandle::try_as<Array>(array_handle)) {
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
      env->set_outgoing_block(MemoryHandle::value(prc));
    } else {
      env->set_outgoing_block(MemoryHandle::value(env->block()));
    }

    return rb_funcall2(obj, meth, argc, argv);
  }

  VALUE rb_apply(VALUE recv, ID mid, VALUE args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* ary = MemoryHandle::object<Array>(args);

    Object* obj = MemoryHandle::object(recv);

    // Unlock, we're leaving extension code.
    LEAVE_CAPI(env->state());

    Object* ret = obj->send(env->state(), reinterpret_cast<Symbol*>(mid), ary, cNil);

    // We need to get the handle for the return value before getting
    // the GEL so that ret isn't accidentally GCd while we wait.
    VALUE ret_handle = 0;
    if(ret) ret_handle = MemoryHandle::value(ret);

    // Re-entering extension code
    ENTER_CAPI(env->state());

    // An exception occurred
    if(!ret) env->current_ep()->return_to(env);

    return ret_handle;
  }

  void capi_infect(VALUE h, VALUE s) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* host   = MemoryHandle::object(h);
    Object* source = MemoryHandle::object(s);

    source->infect(env->state(), host);
  }

  int capi_nil_p(VALUE expression_result) {
    return MemoryHandle::object(expression_result)->nil_p();
  }

  void capi_taint(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    MemoryHandle::object(obj)->taint(env->state());
  }

  int rb_obj_tainted(VALUE value) {
    Object* obj = MemoryHandle::object(value);

    return obj->reference_p() && obj->tainted_p();
  }

  void capi_define_method(const char* file, VALUE target,
                          const char* name, CApiGenericFunction fptr,
                          int arity, CApiMethodKind kind)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    ThreadState* state = env->state();
    Symbol* method_name = state->symbol(name);

    Module* module = NULL;

    if(kind == cCApiSingletonMethod) {
      module = c_as<Module>(MemoryHandle::object(target)->singleton_class(env->state()));
    } else {
      module = MemoryHandle::object<Module>(target);
    }

    NativeMethod* method = NULL;
    method = NativeMethod::create(state, String::create_pinned(state, file),
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

    module->add_method(state, method_name, nil<String>(), method,
        nil<LexicalScope>(), visibility);
  }

  VALUE capi_class_superclass(VALUE class_handle) {
    Module* module = MemoryHandle::object<Module>(class_handle);
    Module* super = module->superclass();

    if(super->nil_p()) {
      /* In MRI, the superclass chain terminates with a NULL pointer.
       * Since VALUE is an intptr_t in Rubinius, we use 0 instead.
       */
      return 0;
    } else {
      return MemoryHandle::value(super);
    }
  }

  /* For debugging. */
  void __show_subtend__(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = MemoryHandle::object(obj_handle);
    if(!object) {
      std::cout << "the object is NULL, check if an exception was raised." << std::endl;
      return;
    }
    object->show(env->state());
  }
}
