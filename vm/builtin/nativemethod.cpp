#include <iostream>

#include "vm.hpp"

#include "exception.hpp"
#include "exception_point.hpp"
#include "message.hpp"
#include "native_libraries.hpp"
#include "primitives.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"

#include "capi/capi.hpp"

namespace rubinius {

  using namespace capi;

  typedef TypedRoot<Object*> RootHandle;

  /** Thread-local NativeMethodEnvironment instance. */
  thread::ThreadData<NativeMethodEnvironment*> native_method_environment;

/* Class methods */

  NativeMethodEnvironment* NativeMethodEnvironment::get() {
    return native_method_environment.get();
  }

  Handle NativeMethodFrame::get_handle(STATE, Object* obj) {
    Handle hndl = handles_.size() + cHandleOffset;
    handles_.push_back(new RootHandle(state, obj));
    return hndl;
  }

  Object* NativeMethodFrame::get_object(Handle hndl) {
    Object* obj = handles_[hndl - cHandleOffset]->get();
    if(!obj) {
      capi_raise_runtime_error("NativeMethod handle refers to NULL object");
    }
    return obj;
  }

  CApiStructs& NativeMethodFrame::strings() {
    if(!strings_) strings_ = new CApiStructs;
    return *strings_;
  }

  CApiStructs& NativeMethodFrame::arrays() {
    if(!arrays_) arrays_ = new CApiStructs;
    return *arrays_;
  }

  void NativeMethodFrame::cleanup() {
    if(arrays_) {
      capi_rarray_flush();
      delete arrays_;
    }
    if(strings_) {
      capi_rstring_flush();
      delete strings_;
    }
  }

  Handle NativeMethodEnvironment::get_handle(Object* obj) {
    return current_native_frame_->get_handle(state_, obj);
  }

  Handle NativeMethodEnvironment::get_handle_global(Object* obj) {
    Handles& global_handles = state_->shared.global_handles();
    Handle handle = cGlobalHandleStart - global_handles.size() - cHandleOffset;
    global_handles.push_back(new RootHandle(state_, obj));
    return handle;
  }

  Object* NativeMethodEnvironment::get_object(Handle handle) {
    if(handle <= 0) {
      switch(handle) {
      case cCApiHandleQfalse:
        return Qfalse;
      case cCApiHandleQtrue:
        return Qtrue;
      case cCApiHandleQnil:
        return Qnil;
      case cCApiHandleQundef:
        return Qundef;
      default:
        Handles& global_handles = state_->shared.global_handles();
        RootHandle* root = global_handles[cGlobalHandleStart - handle - cHandleOffset];
        if(!root) {
          capi_raise_runtime_error("Attempted to use deleted NativeMethod global handle");
        }

        /* No Ruby object should ever be NULL, so if it is, likely an
         * exception occurred that was not caught correctly. Rather
         * than letting it propogate to mysterious corners of the code,
         * we raise an exception here. @see NativeMethodFrame::get_object()
         */
        Object* obj = root->get();
        if(!obj) {
          capi_raise_runtime_error("NativeMethod global handle refers to NULL object");
        }
        return obj;
      }
    } else {
      return current_native_frame_->get_object(handle);
    }
  }

  void NativeMethodEnvironment::delete_global(Handle handle) {
    Handles& global_handles = state_->shared.global_handles();
    RootHandle* root = global_handles[cGlobalHandleStart - handle];
    delete root;
    global_handles[cGlobalHandleStart - handle] = 0;
  }

  Object* NativeMethodEnvironment::block() {
    return current_call_frame_->top_scope->block();
  }

  Handles& NativeMethodEnvironment::handles() {
    return current_native_frame_->handles();
  }

  CApiStructs& NativeMethodEnvironment::strings() {
    return current_native_frame_->strings();
  }

  CApiStructs& NativeMethodEnvironment::arrays() {
    return current_native_frame_->arrays();
  }

  void NativeMethod::init(STATE) {
    state->globals.nmethod.set(state->new_class("NativeMethod", G(executable)));
    state->globals.nmethod.get()->set_object_type(state, NativeMethodType);

    init_thread(state);
  }

  void NativeMethod::init_thread(STATE) {
    NativeMethodEnvironment* env = new NativeMethodEnvironment;
    env->set_state(state);
    native_method_environment.set(env);
  }

  NativeMethod* NativeMethod::allocate(STATE) {
    return create<GenericFunctor>(state);
  }

  Object* NativeMethod::executor_implementation(STATE, CallFrame* call_frame, Message& msg) {
    NativeMethodEnvironment* env = native_method_environment.get();
    NativeMethodFrame nmf(env->current_native_frame());

    env->set_current_call_frame(call_frame);
    env->set_current_native_frame(&nmf);

    NativeMethod* nm = as<NativeMethod>(msg.method);

    Object* ret;
    ExceptionPoint ep(env);

    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      ret = NULL;
    } else {
      ret = nm->call(state, env, msg);
    }

    env->current_native_frame()->cleanup();
    env->set_current_native_frame(nmf.previous());
    ep.pop(env);

    return ret;
  }

  NativeMethod* NativeMethod::load_extension_entry_point(STATE, String* path, String* name) {
    void* func = NativeLibrary::find_symbol(state, name, path);

    NativeMethod* m = NativeMethod::create(state,
                                           path,
                                           state->globals.rubinius.get(),
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
  Object* NativeMethod::call(STATE, NativeMethodEnvironment* env, Message& msg) {
    Handle receiver = env->get_handle(msg.recv);

    switch(arity()->to_int()) {
    case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
      Handle args = env->get_handle(msg.as_array(state));

      Handle ret_handle = functor_as<OneArgFunctor>()(args);

      return env->get_object(ret_handle);
    }

    case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
      Handle args = env->get_handle(msg.as_array(state));

      Handle ret_handle = functor_as<TwoArgFunctor>()(receiver, args);

      return env->get_object(ret_handle);
    }

    case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
      Handle* args = new Handle[msg.args()];

      for (std::size_t i = 0; i < msg.args(); ++i) {
        args[i] = env->get_handle(msg.get_argument(i));
      }

      Handle ret_handle = functor_as<ArgcFunctor>()(msg.args(), args, receiver);

      delete[] args;

      return env->get_object(ret_handle);
    }

      /*
       *  Normal arg counts
       *
       *  Yes, it is ugly as fuck. It is intended as an encouragement
       *  to get rid of the concept of a separate Handle and Object.
       */

    case 0: {
      OneArgFunctor functor = functor_as<OneArgFunctor>();

      Handle ret_handle = functor(receiver);

      return env->get_object(ret_handle);
    }

    case 1: {
      TwoArgFunctor functor = functor_as<TwoArgFunctor>();

      Handle a1 = env->get_handle(msg.get_argument(0));

      Handle ret_handle = functor(receiver, a1);

      return env->get_object(ret_handle);
    }

    case 2: {
      ThreeArgFunctor functor = functor_as<ThreeArgFunctor>();

      Handle a1 = env->get_handle(msg.get_argument(0));
      Handle a2 = env->get_handle(msg.get_argument(1));

      Handle ret_handle = functor(receiver, a1, a2);

      return env->get_object(ret_handle);
    }

    case 3: {
      FourArgFunctor functor = functor_as<FourArgFunctor>();
      Handle a1 = env->get_handle(msg.get_argument(0));
      Handle a2 = env->get_handle(msg.get_argument(1));
      Handle a3 = env->get_handle(msg.get_argument(2));

      Handle ret_handle = functor(receiver, a1, a2, a3);

      return env->get_object(ret_handle);
    }

    case 4: {
      FiveArgFunctor functor = functor_as<FiveArgFunctor>();
      Handle a1 = env->get_handle(msg.get_argument(0));
      Handle a2 = env->get_handle(msg.get_argument(1));
      Handle a3 = env->get_handle(msg.get_argument(2));
      Handle a4 = env->get_handle(msg.get_argument(3));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4);

      return env->get_object(ret_handle);
    }

    case 5: {
      SixArgFunctor functor = functor_as<SixArgFunctor>();
      Handle a1 = env->get_handle(msg.get_argument(0));
      Handle a2 = env->get_handle(msg.get_argument(1));
      Handle a3 = env->get_handle(msg.get_argument(2));
      Handle a4 = env->get_handle(msg.get_argument(3));
      Handle a5 = env->get_handle(msg.get_argument(4));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4, a5);

      return env->get_object(ret_handle);
    }

      /* Extension entry point, should never occur for user code. */
    case INIT_FUNCTION: {
      InitFunctor functor = functor_as<InitFunctor>();

      functor();

      return Qnil;
    }

    default:
      assert(false && "Not a valid arity");
      return Qnil;
    }
  }

}
