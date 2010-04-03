#include <iostream>

#include "vm.hpp"

#include "exception.hpp"
#include "exception_point.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "native_libraries.hpp"
#include "primitives.hpp"
#include "call_frame.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"
#include "builtin/tuple.hpp"
#include "builtin/capi_handle.hpp"

#include "instruments/profiler.hpp"

#include "capi/capi.hpp"
#include "capi/handle.hpp"

namespace rubinius {
  /** Thread-local NativeMethodEnvironment instance. */
  thread::ThreadData<NativeMethodEnvironment*> native_method_environment;

/* Class methods */

  NativeMethodEnvironment* NativeMethodEnvironment::get() {
    return native_method_environment.get();
  }

  NativeMethodFrame::~NativeMethodFrame() {
    flush_cached_data();
    for(capi::HandleSet::iterator i = handles_.begin();
        i != handles_.end();
        i++) {
      capi::Handle* handle = *i;
      handle->deref();
    }
  }

  VALUE NativeMethodFrame::get_handle(STATE, Object* obj) {
    InflatedHeader* ih = state->om->inflate_header(obj);

    capi::Handle* handle = ih->handle();

    if(handle) {
      // ref() ONLY if it's not already in there!
      // otherwise the refcount is wrong and we leak handles.
      capi::HandleSet::iterator pos = handles_.find(handle);
      if(pos == handles_.end()) {
        handle->ref();
        handles_.insert(handle);
      }
    } else {
      handle = new capi::Handle(state, obj);
      ih->set_handle(handle);

      state->shared.global_handles()->add(handle);

      handle->ref();
      handles_.insert(handle);
    }

    return handle->as_value();
  }

  Object* NativeMethodFrame::get_object(VALUE val) {
    return capi::Handle::from(val)->object();
  }

  void NativeMethodFrame::flush_cached_data() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    capi::Handles* handles = env->state()->shared.cached_handles();

    if(handles->size() > 0) {
      for(capi::Handles::Iterator i(*handles); i.more(); i.advance()) {
        i->flush(env);
      }
    }
  }

  void NativeMethodFrame::update_cached_data() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    capi::Handles* handles = env->state()->shared.cached_handles();

    if(handles->size() > 0) {
      for(capi::Handles::Iterator i(*handles); i.more(); i.advance()) {
        i->update(env);
      }
    }
  }

  VALUE NativeMethodEnvironment::get_handle(Object* obj) {
    if(obj->reference_p()) {
      return current_native_frame_->get_handle(state_, obj);
    } else if(obj->fixnum_p() || obj->symbol_p()) {
      return reinterpret_cast<VALUE>(obj);
    } else if(obj->nil_p()) {
      return cCApiHandleQnil;
    } else if(obj->false_p()) {
      return cCApiHandleQfalse;
    } else if(obj->true_p()) {
      return cCApiHandleQtrue;
    } else if(obj == Qundef) {
      return cCApiHandleQundef;
    }

    capi::capi_raise_runtime_error("NativeMethod handle requested for unknown object type");
    return 0; // keep compiler happy
  }

  void NativeMethodEnvironment::delete_global(VALUE val) {
    abort();
  }

  Object* NativeMethodEnvironment::block() {
    return get_object(current_native_frame_->block());
  }

  capi::HandleSet& NativeMethodEnvironment::handles() {
    return current_native_frame_->handles();
  }

  void NativeMethodEnvironment::flush_cached_data() {
    current_native_frame_->flush_cached_data();
  }

  void NativeMethodEnvironment::update_cached_data() {
    current_native_frame_->update_cached_data();
  }

  void NativeMethod::init(STATE) {
    GO(nmethod).set(state->new_class("NativeMethod", G(executable), G(rubinius)));
    G(nmethod)->set_object_type(state, NativeMethodType);

    init_thread(state);
  }

  void NativeMethod::init_thread(STATE) {
    NativeMethodEnvironment* env = new NativeMethodEnvironment(state);
    native_method_environment.set(env);
  }

  void NativeMethod::cleanup_thread(STATE) {
    delete native_method_environment.get();
    native_method_environment.set(NULL);
  }

  NativeMethod* NativeMethod::allocate(STATE) {
    return create<GenericFunctor>(state);
  }

  Object* NativeMethod::executor_implementation(STATE,
      CallFrame* call_frame, Dispatch& msg, Arguments& args) {
    NativeMethod* nm = as<NativeMethod>(msg.method);

    int arity = nm->arity()->to_int();
    if(arity >= 0 && (size_t)arity != args.total()) {
      Exception* exc = Exception::make_argument_error(
          state, arity, args.total(), msg.name);
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(1), call_frame));
      state->thread_state()->raise_exception(exc);

      return NULL;
    }

    // Register the CallFrame, because we might GC below this.
    state->set_call_frame(call_frame);

    NativeMethodEnvironment* env = native_method_environment.get();
    NativeMethodFrame nmf(env->current_native_frame());

    CallFrame* saved_frame = env->current_call_frame();
    env->set_current_call_frame(call_frame);
    env->set_current_native_frame(&nmf);

    // Be sure to do this after installing nmf as the current
    // native frame.
    nmf.setup(
        env->get_handle(args.recv()),
        env->get_handle(args.block()),
        env->get_handle(msg.method),
        env->get_handle(msg.module));

    Object* ret;
    ExceptionPoint ep(env);

    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      ret = NULL;
    } else {
#ifdef RBX_PROFILER
      if(unlikely(state->shared.profiling())) {
        profiler::MethodEntry method(state, msg, args);
        ret = nm->call(state, env, args);
      } else {
        ret = nm->call(state, env, args);
      }
#else
      ret = nm->call(state, env, args);
#endif
    }

    env->set_current_call_frame(saved_frame);
    env->set_current_native_frame(nmf.previous());
    ep.pop(env);

    return ret;
  }

  NativeMethod* NativeMethod::load_extension_entry_point(STATE, String* path, String* name) {
    void* func = NativeLibrary::find_symbol(state, name, path);

    NativeMethod* m = NativeMethod::create(state,
                                           path,
                                           G(rubinius),
                                           name->to_sym(state),
                                           reinterpret_cast<GenericFunctor>(func),
                                           Fixnum::from(INIT_FUNCTION)
                                          );
    return m;
  }

  /**
   *    Arity -3:   VALUE func(VALUE argument_array);
   *    Arity -2:   VALUE func(VALUE receiver, VALUE argument_array);
   *    Arity -1:   VALUE func(int argument_count, VALUE*, VALUE receiver);
   *    Otherwise:  VALUE func(VALUE receiver, VALUE arg1[, VALUE arg2, ...]);
   *
   *  There is also a special-case arity, INIT_FUNCTION, which corresponds
   *  to void (*)(void) and should never appear in user code.
   *
   *  @note   Currently supports functions with up to receiver + 5 (separate) arguments only!
   *          Anything beyond that should use one of the special arities instead.
   *
   *  @todo   Check for inefficiencies.
   */
  Object* NativeMethod::call(STATE, NativeMethodEnvironment* env, Arguments& args) {
    VALUE receiver = env->get_handle(args.recv());

    switch(arity()->to_int()) {
    case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
      VALUE ary = env->get_handle(args.as_array(state));

      VALUE ret = functor_as<OneArgFunctor>()(ary);

      return env->get_object(ret);
    }

    case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
      VALUE ary = env->get_handle(args.as_array(state));

      VALUE ret = functor_as<TwoArgFunctor>()(receiver, ary);

      return env->get_object(ret);
    }

    case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
      VALUE* ary = (VALUE*)alloca(sizeof(VALUE) * args.total());

      for (std::size_t i = 0; i < args.total(); ++i) {
        ary[i] = env->get_handle(args.get_argument(i));
      }

      VALUE ret = functor_as<ArgcFunctor>()(args.total(), ary, receiver);

      return env->get_object(ret);
    }

      /*
       *  Normal arg counts
       *
       *  Yes, it is ugly as fuck. It is intended as an encouragement
       *  to get rid of the concept of a separate VALUE and Object.
       */

    case 0: {
      OneArgFunctor functor = functor_as<OneArgFunctor>();

      VALUE ret = functor(receiver);

      return env->get_object(ret);
    }

    case 1: {
      TwoArgFunctor functor = functor_as<TwoArgFunctor>();

      VALUE a1 = env->get_handle(args.get_argument(0));

      VALUE ret = functor(receiver, a1);

      return env->get_object(ret);
    }

    case 2: {
      ThreeArgFunctor functor = functor_as<ThreeArgFunctor>();

      VALUE a1 = env->get_handle(args.get_argument(0));
      VALUE a2 = env->get_handle(args.get_argument(1));

      VALUE ret = functor(receiver, a1, a2);

      return env->get_object(ret);
    }

    case 3: {
      FourArgFunctor functor = functor_as<FourArgFunctor>();
      VALUE a1 = env->get_handle(args.get_argument(0));
      VALUE a2 = env->get_handle(args.get_argument(1));
      VALUE a3 = env->get_handle(args.get_argument(2));

      VALUE ret = functor(receiver, a1, a2, a3);

      return env->get_object(ret);
    }

    case 4: {
      FiveArgFunctor functor = functor_as<FiveArgFunctor>();
      VALUE a1 = env->get_handle(args.get_argument(0));
      VALUE a2 = env->get_handle(args.get_argument(1));
      VALUE a3 = env->get_handle(args.get_argument(2));
      VALUE a4 = env->get_handle(args.get_argument(3));

      VALUE ret = functor(receiver, a1, a2, a3, a4);

      return env->get_object(ret);
    }

    case 5: {
      SixArgFunctor functor = functor_as<SixArgFunctor>();
      VALUE a1 = env->get_handle(args.get_argument(0));
      VALUE a2 = env->get_handle(args.get_argument(1));
      VALUE a3 = env->get_handle(args.get_argument(2));
      VALUE a4 = env->get_handle(args.get_argument(3));
      VALUE a5 = env->get_handle(args.get_argument(4));

      VALUE ret = functor(receiver, a1, a2, a3, a4, a5);

      return env->get_object(ret);
    }

      /* Extension entry point, should never occur for user code. */
    case INIT_FUNCTION: {
      InitFunctor functor = functor_as<InitFunctor>();

      functor();

      return Qnil;
    }

    default:
      capi::capi_raise_runtime_error("unrecognized arity for NativeMethod call");
      return Qnil;
    }
  }

}
