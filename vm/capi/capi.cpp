#include "builtin/array.hpp"
#include "builtin/data.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/module.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/location.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/proc.hpp"

#include "lookup_data.hpp"
#include "dispatch.hpp"
#include "arguments.hpp"

#include "exception_point.hpp"
#include "global_cache.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "capi/capi.hpp"
#include "capi/include/ruby.h"
#include <string>
#include <vector>
#include <tr1/unordered_map>

namespace rubinius {
  namespace capi {

    typedef std::vector<std::string> CApiConstantNameMap;
    typedef std::tr1::unordered_map<int, VALUE> CApiConstantHandleMap;

    /**
     * This looks like a complicated scheme but there is a reason for
     * doing it this way. In MRI, rb_cObject, etc. are all global data.
     * We need to avoid global data to better support embedding and
     * other features like MVM. @see capi_get_constant().
     */
    std::string& capi_get_constant_name(int type) {
      static CApiConstantNameMap map;

      if(map.empty()) {
        map.resize(cCApiMaxConstant + 1);

        map[cCApiArray]      = "Array";
        map[cCApiBignum]     = "Bignum";
        map[cCApiClass]      = "Class";
        map[cCApiComparable] = "Comparable";
        map[cCApiData]       = "Data";
        map[cCApiEnumerable] = "Enumerable";
        map[cCApiFalse]      = "FalseClass";
        map[cCApiFile]       = "File";
        map[cCApiFixnum]     = "Fixnum";
        map[cCApiFloat]      = "Float";
        map[cCApiHash]       = "Hash";
        map[cCApiInteger]    = "Integer";
        map[cCApiIO]         = "IO";
        map[cCApiKernel]     = "Kernel";
        map[cCApiMatch]      = "MatchData";
        map[cCApiModule]     = "Module";
        map[cCApiNil]        = "NilClass";
        map[cCApiNumeric]    = "Numeric";
        map[cCApiObject]     = "Object";
        map[cCApiRange]      = "Range";
        map[cCApiRegexp]     = "Regexp";
        map[cCApiRubinius]   = "Rubinius";
        map[cCApiString]     = "String";
        map[cCApiStruct]     = "Struct";
        map[cCApiSymbol]     = "Symbol";
        map[cCApiThread]     = "Thread";
        map[cCApiTime]       = "Time";
        map[cCApiTrue]       = "TrueClass";
        map[cCApiProc]       = "Proc";
        map[cCApiGC]         = "GC";
        map[cCApiCAPI]       = "Rubinius::CAPI";
        map[cCApiMethod]     = "Method";

        map[cCApiArgumentError]       = "ArgumentError";
        map[cCApiEOFError]            = "EOFError";
        map[cCApiErrno]               = "Errno";
        map[cCApiException]           = "Exception";
        map[cCApiFatal]               = "FatalError";
        map[cCApiFloatDomainError]    = "FloatDomainError";
        map[cCApiIndexError]          = "IndexError";
        map[cCApiInterrupt]           = "Interrupt";
        map[cCApiIOError]             = "IOError";
        map[cCApiLoadError]           = "LoadError";
        map[cCApiLocalJumpError]      = "LocalJumpError";
        map[cCApiNameError]           = "NameError";
        map[cCApiNoMemoryError]       = "NoMemoryError";
        map[cCApiNoMethodError]       = "NoMethodError";
        map[cCApiNotImplementedError] = "NotImplementedError";
        map[cCApiRangeError]          = "RangeError";
        map[cCApiRegexpError]         = "RegexpError";
        map[cCApiRuntimeError]        = "RuntimeError";
        map[cCApiScriptError]         = "ScriptError";
        map[cCApiSecurityError]       = "SecurityError";
        map[cCApiSignalException]     = "SignalException";
        map[cCApiStandardError]       = "StandardError";
        map[cCApiSyntaxError]         = "SyntaxError";
        map[cCApiSystemCallError]     = "SystemCallError";
        map[cCApiSystemExit]          = "SystemExit";
        map[cCApiSystemStackError]    = "SystemStackError";
        map[cCApiTypeError]           = "TypeError";
        map[cCApiThreadError]         = "ThreadError";
        map[cCApiZeroDivisionError]   = "ZeroDivisionError";
      }

      if(type < 0 || type >= cCApiMaxConstant) {
        NativeMethodEnvironment* env = NativeMethodEnvironment::get();
        rb_raise(env->get_handle(env->state()->globals().exception.get()),
              "C-API: invalid constant index");
      }

      return map[type];
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

      Object* blk = RBX_Qnil;

      if(VALUE blk_handle = env->outgoing_block()) {
        blk = env->get_object(blk_handle);
        env->set_outgoing_block(0);
      }

      Object* recv = env->get_object(receiver);
      Object* ret = recv->send(env->state(), env->current_call_frame(),
          reinterpret_cast<Symbol*>(method_name), args, blk);
      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return env->get_handle(ret);
    }

    VALUE capi_funcall_backend_native(NativeMethodEnvironment* env,
                                      const char* file, int line,
                                      Object* recv, Symbol* method,
                                      size_t arg_count,
                                      Object** args, Object* block)
    {
      int marker = 0;
      if(!env->state()->check_stack(env->current_call_frame(), &marker)) {
        env->current_ep()->return_to(env);
      }

      env->flush_cached_data();

      LookupData lookup(recv, recv->lookup_begin(env->state()), true);
      Arguments args_o(recv, block, arg_count, args);
      Dispatch dis(method);

      Object* ret = dis.send(env->state(), env->current_call_frame(),
                             lookup, args_o);

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return env->get_handle(ret);
    }

    VALUE capi_yield_backend(NativeMethodEnvironment* env,
                              Object* blk,
                              size_t arg_count, Object** arg_vals)
    {
      int marker = 0;
      if(!env->state()->check_stack(env->current_call_frame(), &marker)) {
        env->current_ep()->return_to(env);
      }

      env->flush_cached_data();
      Object* ret = RBX_Qnil;
      STATE = env->state();

      CallFrame* call_frame = env->current_call_frame();
      Arguments args(blk, arg_count, arg_vals);

      if(BlockEnvironment* be = try_as<BlockEnvironment>(blk)) {
        ret = be->call(state, call_frame, args);
      } else if(Proc* proc = try_as<Proc>(blk)) {
        ret = proc->yield(state, call_frame, args);
      } else if(blk->nil_p()) {
        state->thread_state()->raise_exception(
            Exception::make_lje(state, call_frame));
        ret = NULL;
      } else {
        Dispatch dis(G(sym_call));
        ret = dis.send(state, call_frame, args);
      }

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return env->get_handle(ret);
    }

    VALUE capi_call_super_native(NativeMethodEnvironment* env,
                                 size_t arg_count, Object** args)
    {
      int marker = 0;
      if(!env->state()->check_stack(env->current_call_frame(), &marker)) {
        env->current_ep()->return_to(env);
      }

      env->flush_cached_data();

      NativeMethodFrame* frame = NativeMethodFrame::current();

      Object* recv = env->get_object(frame->receiver());
      Module* mod =  c_as<Module>(env->get_object(frame->module()));
      Symbol* name = c_as<NativeMethod>(env->get_object(frame->method()))->name();

      LookupData lookup(recv, mod->superclass(), true);
      Arguments args_o(recv, arg_count, args);
      Dispatch dis(name);

      Object* ret = dis.send(env->state(), env->current_call_frame(),
                             lookup, args_o);

      env->update_cached_data();

      // An exception occurred
      if(!ret) env->current_ep()->return_to(env);

      return env->get_handle(ret);
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
      std::string& sym = state->shared.symbols.lookup_cppstring(sym_obj);

      if(sym.compare(0UL, len, prefix) == 0) return sym_obj;

      std::ostringstream stream;
      stream << prefix << sym;

      return state->symbol(stream.str().c_str());
    }

    Symbol* prefixed_by(STATE, const char prefix, ID name) {
      Symbol* sym_obj = reinterpret_cast<Symbol*>(name);
      std::string& sym = state->shared.symbols.lookup_cppstring(sym_obj);

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

      TypeInfo* expected = env->state()->find_type(type);
      TypeInfo* actual = env->state()->find_type(object->get_type());

      rb_raise(rb_eTypeError, "wrong argument type %s (expected %s)",
          actual->type_name.c_str(), expected->type_name.c_str());
    }

    void capi_raise_backend(Exception* exception) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      exception->locations(env->state(), Location::from_call_stack(env->state(),
                           env->current_call_frame()));
      env->state()->thread_state()->raise_exception(exception);

      env->current_ep()->return_to(env);
    }


    Proc* wrap_c_function(void* cb, VALUE cb_data, int arity) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      NativeMethod* nm = NativeMethod::create(env->state(),
                          nil<String>(), env->state()->shared.globals.rubinius.get(),
                          env->state()->symbol("call"), cb,
                          Fixnum::from(arity));

      nm->set_ivar(env->state(), env->state()->symbol("cb_data"),
                   env->get_object(cb_data));

      Proc* prc = Proc::create(env->state(), env->state()->shared.globals.proc.get());
      prc->bound_method(env->state(), nm);

      return prc;
    }
  }
}

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE capi_get_constant(CApiConstant type) {
    static CApiConstantHandleMap map;

    CApiConstantHandleMap::iterator entry = map.find(type);
    if(entry == map.end()) {
      VALUE val = rb_path2class(capi_get_constant_name(type).c_str());
      capi::Handle::from(val)->ref(); // Extra ref, since we save it in the map
      map[type] = val;
      return val;
    } else {
      return entry->second;
    }
  }

  VALUE rb_call_super(int argc, const VALUE *argv) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object** args = reinterpret_cast<Object**>(alloca(sizeof(Object*) * argc));
    for(int i = 0; i < argc; i++) {
      args[i] = env->get_object(argv[i]);
    }

    return capi_call_super_native(env, argc, args);
  }

  VALUE rb_funcall(VALUE receiver, ID method_name, int arg_count, ...) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    va_list varargs;
    va_start(varargs, arg_count);

    Object** args = reinterpret_cast<Object**>(alloca(sizeof(Object*) * arg_count));

    for(int i = 0; i < arg_count; i++) {
      args[i] = env->get_object(va_arg(varargs, VALUE));
    }

    va_end(varargs);

    Object* blk = RBX_Qnil;

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

    Object** args = reinterpret_cast<Object**>(alloca(sizeof(Object**) * arg_count));

    for(int i = 0; i < arg_count; i++) {
      args[i] = env->get_object(v_args[i]);
    }

    Object* blk = RBX_Qnil;

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

    Object** args = reinterpret_cast<Object**>(alloca(sizeof(Object**) * arg_count));

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

    if(!RBX_RTEST(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    Object* arg = env->get_object(argument_handle);

    return capi_yield_backend(env, blk, 1, &arg);
  }

  VALUE rb_yield_values(int n, ...) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* blk = env->block();

    if(!RBX_RTEST(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    if(n == 0) {
      return capi_yield_backend(env, blk, 0, 0);
    }

    Object** vars = reinterpret_cast<Object**>(alloca(sizeof(Object*) * n));

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

    if(!RBX_RTEST(blk)) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    if(Array* ary = try_as<Array>(env->get_object(array_handle))) {
      int count = ary->size();
      Object** vars = reinterpret_cast<Object**>(alloca(sizeof(Object*) * count));

      for(int i = 0; i < count; i++) {
        vars[i] = ary->get(env->state(), i);
      }

      return capi_yield_backend(env, blk, count, vars);
    }

    return capi_yield_backend(env, blk, 0, 0);
  }

  int rb_block_given_p() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return RBX_RTEST(env->block());
  }

  void rb_need_block() {
    if (!rb_block_given_p()) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }
  }

  VALUE rb_apply(VALUE recv, ID mid, VALUE args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    env->flush_cached_data();

    Array* ary = capi::c_as<Array>(env->get_object(args));

    Object* obj = env->get_object(recv);
    Object* ret = obj->send(env->state(), env->current_call_frame(),
        reinterpret_cast<Symbol*>(mid), ary, RBX_Qnil);
    env->update_cached_data();

    // An exception occurred
    if(!ret) env->current_ep()->return_to(env);

    return env->get_handle(ret);
  }


  void capi_infect(VALUE obj1, VALUE obj2) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object1 = env->get_object(obj1);
    Object* object2 = env->get_object(obj2);

    object1->infect(env->state(), object2);
  }

  int capi_nil_p(VALUE expression_result) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return RBX_NIL_P(env->get_object(expression_result));
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

    VM* state = env->state();
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
                                  (void*)fptr, Fixnum::from(arity));

    Symbol* visibility;

    switch(kind) {
    case cCApiPrivateMethod:
      visibility = state->symbol("private");
      break;

    case cCApiProtectedMethod:
      visibility = state->symbol("protected");
      break;

    default:  /* Also catches singletons for now. @todo Verify OK. --rue */
      visibility = state->symbol("public");
      break;
    }

    module->add_method(state, method_name, method, visibility);
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
