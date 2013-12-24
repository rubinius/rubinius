#include "arguments.hpp"
#include "builtin/array.hpp"
#include "builtin/data.hpp"
#include "builtin/exception.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/float.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"
#include "call_frame.hpp"
#include "capi/capi.hpp"
#include "capi/handle.hpp"
#include "configuration.hpp"
#include "dtrace/dtrace.h"
#include "exception.hpp"
#include "exception_point.hpp"
#include "instruments/tooling.hpp"
#include "objectmemory.hpp"
#include "ontology.hpp"
#include "on_stack.hpp"

namespace rubinius {
  /** Thread-local NativeMethodEnvironment instance. */
  utilities::thread::ThreadData<NativeMethodEnvironment*> native_method_environment;

/* Class methods */

  NativeMethodEnvironment* NativeMethodEnvironment::get() {
    return native_method_environment.get();
  }

  NativeMethodFrame::NativeMethodFrame(NativeMethodEnvironment* env, NativeMethodFrame* prev, NativeMethod* method)
    : previous_(prev)
    , env_(env)
    , capi_lock_index_(method ? method->capi_lock_index() : 0)
    , check_handles_(false)
    , block_(Qnil)
  {}

  NativeMethodFrame::~NativeMethodFrame() {
    flush_cached_data();
    handles_.deref_all();
  }

  void NativeMethodFrame::check_tracked_handle(capi::Handle* handle,
                                               bool need_update)
  {
    if(need_update) {
      check_handles_ = true;
    }

    if(handles_.add_if_absent(handle)) {
      // We're seeing this object for the first time in this function.
      // Be sure that it's updated.
      handle->update(env_);
    }
  }

  VALUE NativeMethodFrame::get_handle(STATE, Object* obj) {

    capi::Handle* handle = obj->handle(state);

    if(handle) {
      if(handles_.add_if_absent(handle)) {
        // We're seeing this object for the first time in this function.
        // Be sure that it's updated.
        handle->update(env_);
      }
    } else {
      handle = state->memory()->add_capi_handle(state, obj);
      handles_.add_if_absent(handle);
    }

    if(!handle->valid_p()) {
      handle->debug_print();
      rubinius::abort();
    }

    return handle->as_value();
  }

  Object* NativeMethodFrame::get_object(VALUE val) {
    return capi::Handle::from(val)->object();
  }

  void NativeMethodFrame::flush_cached_data() {
    if(check_handles_) {
      handles_.flush_all(env_);
    }

    if(env_->state()->shared().config.capi_global_flush) {
      std::list<capi::Handle*>* handles = env_->state()->memory()->cached_capi_handles();

      for(std::list<capi::Handle*>::iterator i = handles->begin();
          i != handles->end();
          ++i) {
        (*i)->flush(env_);
      }
    }
  }

  void NativeMethodFrame::update_cached_data() {
    if(check_handles_) {
      handles_.update_all(env_);
    }

    if(env_->state()->shared().config.capi_global_flush) {
      std::list<capi::Handle*>* handles = env_->state()->memory()->cached_capi_handles();

      for(std::list<capi::Handle*>::iterator i = handles->begin();
          i != handles->end();
          ++i) {
        (*i)->update(env_);
      }
    }
  }

  VALUE NativeMethodEnvironment::get_handle(Object* obj) {
    if(obj->reference_p()) {
      if(!current_native_frame_) {
        rubinius::bug("Unable to create handles with no NMF");
      }

      return current_native_frame_->get_handle(&state_, obj);
    } else if(obj->fixnum_p() || obj->symbol_p()) {
      return reinterpret_cast<VALUE>(obj);
    } else if(obj->nil_p()) {
      return Qnil;
    } else if(obj->false_p()) {
      return Qfalse;
    } else if(obj->true_p()) {
      return Qtrue;
    } else if(obj->undef_p()) {
      return Qundef;
    }

    capi::capi_raise_runtime_error("NativeMethod handle requested for unknown object type");
    return 0; // keep compiler happy
  }

  Object* NativeMethodEnvironment::block() {
    if(!current_native_frame_) return cNil;
    return get_object(current_native_frame_->block());
  }

  capi::HandleSet& NativeMethodEnvironment::handles() {
    if(!current_native_frame_) {
      rubinius::bug("Requested handles with no frame");
    }

    return current_native_frame_->handles();
  }

  void NativeMethodEnvironment::flush_cached_data() {
    if(!current_native_frame_) return;
    current_native_frame_->flush_cached_data();
  }

  void NativeMethodEnvironment::check_tracked_handle(capi::Handle* hdl,
                                                     bool need_update)
  {
    if(!current_native_frame_) return;
    current_native_frame_->check_tracked_handle(hdl, need_update);
  }

  void NativeMethodEnvironment::update_cached_data() {
    if(!current_native_frame_) return;
    current_native_frame_->update_cached_data();
  }

  StackVariables* NativeMethodEnvironment::scope() {
    CallFrame* cur = current_call_frame();
    while(!cur->scope) {
      cur = cur->previous;
    }
    return cur->scope;
  }

  void NativeMethod::init(STATE) {
    GO(nmethod).set(ontology::new_class(state, "NativeMethod",
          G(executable), G(rubinius)));
    G(nmethod)->set_object_type(state, NativeMethodType);

    init_thread(state);
  }

  void NativeMethod::init_thread(STATE) {
    NativeMethodEnvironment* env = new NativeMethodEnvironment(state);
    native_method_environment.set(env);
    state->vm()->native_method_environment = env;
  }

  void NativeMethod::cleanup_thread(STATE) {
    delete state->vm()->native_method_environment;
    state->vm()->native_method_environment = NULL;
    native_method_environment.set(NULL);
  }

  /**
   *    Arity -3:   VALUE func(VALUE argument_array);
   *    Arity -2:   VALUE func(VALUE receiver, VALUE argument_array);
   *    Arity -1:   VALUE func(int argument_count, VALUE*, VALUE receiver);
   *    Otherwise:  VALUE func(VALUE receiver, [VALUE arg1, VALUE arg2, ...]);
   *
   *  There is also a special-case arity, INIT_FUNCTION, which corresponds
   *  to void (*)(void) and should never appear in user code.
   *
   *  @note   Currently supports functions with up to receiver + 15 (separate) arguments only!
   *          Anything beyond that should use one of the special arities instead.
   *          15 is the limit in MRI as well.
   */

  class ZeroArguments {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      return env->get_object(nm->func()(receiver));
    }
  };

  class OneArgument {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      VALUE a1 = env->get_handle(args.get_argument(0));

      return env->get_object(nm->func()(receiver, a1));
    }
  };

  class TwoArguments {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      VALUE a1 = env->get_handle(args.get_argument(0));
      VALUE a2 = env->get_handle(args.get_argument(1));

      return env->get_object(nm->func()(receiver, a1, a2));
    }
  };

  class ThreeArguments {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      VALUE a1 = env->get_handle(args.get_argument(0));
      VALUE a2 = env->get_handle(args.get_argument(1));
      VALUE a3 = env->get_handle(args.get_argument(2));

      return env->get_object(nm->func()(receiver, a1, a2, a3));
    }
  };

  class GenericArguments {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();

      switch(nm->arity()->to_int()) {

        // This one is not in MRI.
      case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
        VALUE ary = env->get_handle(args.as_array(state));

        VALUE ret = nm->func()(ary);

        return env->get_object(ret);
      }

      case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
        VALUE ary = env->get_handle(args.as_array(state));

        VALUE ret = nm->func()(receiver, ary);

        return env->get_object(ret);
      }

      case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
        VALUE ary[args.total()];

        for (std::size_t i = 0; i < args.total(); ++i) {
          ary[i] = env->get_handle(args.get_argument(i));
        }

        VALUE ret = nm->func_as<ArgcFunction>()(args.total(), ary, receiver);

        return env->get_object(ret);
      }

        /*
         *  Normal arg counts
         *
         */

      case 0:
        return env->get_object(nm->func()(receiver));

      case 1: {
        VALUE a1 = env->get_handle(args.get_argument(0));

        VALUE ret = nm->func()(receiver, a1);

        return env->get_object(ret);
      }

      case 2: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));

        VALUE ret = nm->func()(receiver, a1, a2);

        return env->get_object(ret);
      }

      case 3: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));

        VALUE ret = nm->func()(receiver, a1, a2, a3);

        return env->get_object(ret);
      }

      case 4: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4);

        return env->get_object(ret);
      }

      case 5: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5);

        return env->get_object(ret);
      }

      case 6: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6);

        return env->get_object(ret);
      }

      case 7: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7);

        return env->get_object(ret);
      }

      case 8: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7, a8);

        return env->get_object(ret);
      }

      case 9: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7, a8, a9);

        return env->get_object(ret);
      }

      case 10: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));
        VALUE a10 = env->get_handle(args.get_argument(9));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10);

        return env->get_object(ret);
      }

      case 11: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));
        VALUE a10 = env->get_handle(args.get_argument(9));
        VALUE a11 = env->get_handle(args.get_argument(10));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11);

        return env->get_object(ret);
      }

      case 12: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));
        VALUE a10 = env->get_handle(args.get_argument(9));
        VALUE a11 = env->get_handle(args.get_argument(10));
        VALUE a12 = env->get_handle(args.get_argument(11));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12);

        return env->get_object(ret);
      }

      case 13: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));
        VALUE a10 = env->get_handle(args.get_argument(9));
        VALUE a11 = env->get_handle(args.get_argument(10));
        VALUE a12 = env->get_handle(args.get_argument(11));
        VALUE a13 = env->get_handle(args.get_argument(12));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12, a13);

        return env->get_object(ret);
      }

      case 14: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));
        VALUE a10 = env->get_handle(args.get_argument(9));
        VALUE a11 = env->get_handle(args.get_argument(10));
        VALUE a12 = env->get_handle(args.get_argument(11));
        VALUE a13 = env->get_handle(args.get_argument(12));
        VALUE a14 = env->get_handle(args.get_argument(13));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12, a13, a14);

        return env->get_object(ret);
      }

      case 15: {
        VALUE a1 = env->get_handle(args.get_argument(0));
        VALUE a2 = env->get_handle(args.get_argument(1));
        VALUE a3 = env->get_handle(args.get_argument(2));
        VALUE a4 = env->get_handle(args.get_argument(3));
        VALUE a5 = env->get_handle(args.get_argument(4));
        VALUE a6 = env->get_handle(args.get_argument(5));
        VALUE a7 = env->get_handle(args.get_argument(6));
        VALUE a8 = env->get_handle(args.get_argument(7));
        VALUE a9 = env->get_handle(args.get_argument(8));
        VALUE a10 = env->get_handle(args.get_argument(9));
        VALUE a11 = env->get_handle(args.get_argument(10));
        VALUE a12 = env->get_handle(args.get_argument(11));
        VALUE a13 = env->get_handle(args.get_argument(12));
        VALUE a14 = env->get_handle(args.get_argument(13));
        VALUE a15 = env->get_handle(args.get_argument(14));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12, a13, a14, a15);

        return env->get_object(ret);
      }
        /* Extension entry point, should never occur for user code. */
      case INIT_FUNCTION: {
        nm->func_as<InitFunction>()();

        return cNil;
      }

        /* A C function being used as a block */
      case ITERATE_BLOCK: {
        VALUE cb_data = env->get_handle(nm->get_ivar(state, state->symbol("cb_data")));

        Object* ob = nm->get_ivar(state, state->symbol("original_block"));
        if(!ob->nil_p()) {
          env->current_native_frame()->set_block(env->get_handle(ob));
        }

        VALUE val;

        switch(args.total()) {
        case 0:
          val = env->get_handle(cNil);
          break;
        case 1:
          val = env->get_handle(args.get_argument(0));
          break;
        default:
          val = env->get_handle(args.as_array(state));
          break;
        }

        VALUE ret = nm->func()(val, cb_data, receiver);
        return env->get_object(ret);
      }

      case C_BLOCK_CALL: {
        VALUE val;
        VALUE ary[args.total()];
        VALUE cb_data = env->get_handle(nm->get_ivar(state, state->symbol("cb_data")));

        if(args.total() > 0) {
          val = env->get_handle(args.get_argument(0));
        } else {
          val = env->get_handle(cNil);
        }

        for (std::size_t i = 0; i < args.total(); ++i) {
          ary[i] = env->get_handle(args.get_argument(i));
        }

        VALUE ret = nm->func()(val, cb_data, args.total(), ary);

        return env->get_object(ret);
      }

      case C_LAMBDA: {
        VALUE cb_data = env->get_handle(nm->get_ivar(state, state->symbol("cb_data")));
        VALUE val = env->get_handle(args.as_array(state));
        VALUE ret = nm->func()(val, cb_data);
        return env->get_object(ret);
      }

      case C_CALLBACK: {
        VALUE cb_data = env->get_handle(nm->get_ivar(state, state->symbol("cb_data")));

        nm->func()(cb_data);

        return cNil;
      }

      default:
        capi::capi_raise_runtime_error("unrecognized arity for NativeMethod call");
        return cNil;
      }

    }

  };

  template <class ArgumentHandler>
  Object* NativeMethod::executor_implementation(STATE,
      CallFrame* previous, Executable* exec, Module* mod, Arguments& args) {
    NativeMethod* nm = as<NativeMethod>(exec);

    int arity = nm->arity()->to_int();

    if(arity >= 0 && (size_t)arity != args.total()) {
      Exception* exc = Exception::make_argument_error(
          state, arity, args.total(), args.name());
      exc->locations(state, Location::from_call_stack(state, previous));
      state->raise_exception(exc);

      return NULL;
    }

    NativeMethodEnvironment* env = state->vm()->native_method_environment;

    // Optionally get the handles back to the proper state.
    if(state->shared().config.capi_global_flush) {
      std::list<capi::Handle*>* handles = env->state()->memory()->cached_capi_handles();

      for(std::list<capi::Handle*>::iterator i = handles->begin();
          i != handles->end();
          ++i) {
        (*i)->update(env);
      }
    }

    NativeMethodFrame nmf(env, env->current_native_frame(), nm);
    CallFrame* call_frame = ALLOCA_CALLFRAME(0);
    call_frame->previous = previous;
    call_frame->constant_scope_ = 0;
    call_frame->dispatch_data = (void*)&nmf;
    call_frame->compiled_code = 0;
    call_frame->flags = CallFrame::cNativeMethod;
    call_frame->optional_jit_data = 0;
    call_frame->top_scope_ = 0;
    call_frame->scope = 0;
    call_frame->arguments = &args;

    CallFrame* saved_frame = env->current_call_frame();
    env->set_current_call_frame(call_frame);
    env->set_current_native_frame(&nmf);

    // Register the CallFrame, because we might GC below this.
    state->set_call_frame(call_frame);

    // Be sure to do this after installing nmf as the current
    // native frame.
    nmf.setup(
        env->get_handle(args.recv()),
        env->get_handle(args.block()),
        env->get_handle(exec),
        env->get_handle(mod));

    // We've got things setup (they can be GC'd properly), so we need to
    // wait before entering the extension code.
    ENTER_CAPI(state);

    Object* ret;
    ExceptionPoint ep(env);

#ifdef RBX_PROFILER
    // This is organized like this so that we don't jump past the destructor of
    // MethodEntry. It's duplicated, but it's much easier to understand than
    // trying to de-dup it.

    OnStack<2> os(state, exec, mod);
    if(unlikely(state->vm()->tooling())) {
      tooling::MethodEntry method(state, exec, mod, args);
      RUBINIUS_METHOD_NATIVE_ENTRY_HOOK(state, mod, args.name(), call_frame);

      PLACE_EXCEPTION_POINT(ep);

      if(unlikely(ep.jumped_to())) {
        ret = NULL;
      } else {
        ret = ArgumentHandler::invoke(state, nm, env, args);
      }
      RUBINIUS_METHOD_NATIVE_RETURN_HOOK(state, mod, args.name(), call_frame);
    } else {
      RUBINIUS_METHOD_NATIVE_ENTRY_HOOK(state, mod, args.name(), call_frame);

      PLACE_EXCEPTION_POINT(ep);

      if(unlikely(ep.jumped_to())) {
        ret = NULL;
      } else {
        ret = ArgumentHandler::invoke(state, nm, env, args);
      }
      RUBINIUS_METHOD_NATIVE_RETURN_HOOK(state, mod, args.name(), call_frame);
    }
#else
    RUBINIUS_METHOD_NATIVE_ENTRY_HOOK(state, mod, args.name(), call_frame);

    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      ret = NULL;
    } else {
      ret = ArgumentHandler::invoke(state, nm, env, args);
    }
    RUBINIUS_METHOD_NATIVE_RETURN_HOOK(state, mod, args.name(), call_frame);
#endif

    LEAVE_CAPI(state);

    env->set_current_call_frame(saved_frame);
    env->set_current_native_frame(nmf.previous());
    ep.pop(env);

    OnStack<1> os_ret(state, ret);

    // Handle any signals that occurred while the native method
    // was running.
    if(!state->check_async(call_frame)) return NULL;

    return ret;
  }

  NativeMethod* NativeMethod::load_extension_entry_point(STATE,
      String* library, Symbol* name, Pointer* ptr) {
    void* func = ptr->pointer;

    int capi_lock_index = state->shared().capi_lock_index(name->debug_str(state));

    return NativeMethod::create(state, library, G(rubinius),
                                name, func,
                                Fixnum::from(INIT_FUNCTION),
                                capi_lock_index);
  }

  NativeMethod* NativeMethod::create(State* state, String* file_name,
                                     Module* module, Symbol* method_name,
                                     void* func, Fixnum* arity,
                                     int capi_lock_index)
  {
    NativeMethod* nmethod = state->new_object<NativeMethod>(G(nmethod));

    nmethod->arity(state, arity);
    nmethod->file(state, file_name);
    nmethod->name(state, method_name);
    nmethod->module(state, module);

    nmethod->func_ = func;

    switch(arity->to_native()) {
    case 0:
      nmethod->set_executor(&NativeMethod::executor_implementation<ZeroArguments>);
      break;

    case 1:
      nmethod->set_executor(&NativeMethod::executor_implementation<OneArgument>);
      break;

    case 2:
      nmethod->set_executor(&NativeMethod::executor_implementation<TwoArguments>);
      break;

    case 3:
      nmethod->set_executor(&NativeMethod::executor_implementation<ThreeArguments>);
      break;

    default:
      nmethod->set_executor(&NativeMethod::executor_implementation<GenericArguments>);
      break;
    }

    nmethod->primitive(state, state->symbol("nativemethod_call"));
    nmethod->serial(state, Fixnum::from(0));
    nmethod->inliners_ = 0;
    nmethod->prim_index_ = -1;
    nmethod->custom_call_site_ = false;
    nmethod->capi_lock_index_ = capi_lock_index;

    return nmethod;
  }


}
