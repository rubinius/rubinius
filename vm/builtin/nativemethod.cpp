#include <iostream>

#include "vm.hpp"

#include "exception.hpp"
#include "exception_point.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "native_libraries.hpp"
#include "primitives.hpp"
#include "call_frame.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"

#include "instruments/profiler.hpp"

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
    Handle handle = CAPI_APPLY_LOCAL_TAG(handles_.size());
    handles_.push_back(new RootHandle(state, obj));
    return handle;
  }

  Object* NativeMethodFrame::get_object(Handle handle) {
    Object* obj = handles_[CAPI_STRIP_LOCAL_TAG(handle)]->get();
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

  CApiStructs& NativeMethodFrame::data() {
    if(!data_) data_ = new CApiStructs;
    return *data_;
  }

  void NativeMethodFrame::flush_cached_data(bool release_memory) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(arrays_) {
      capi_rarray_flush(env, *arrays_, release_memory);
      if(release_memory) delete arrays_;
    }
    if(strings_) {
      capi_rstring_flush(env, *strings_, release_memory);
      if(release_memory) delete strings_;
    }
    if(data_) {
      capi_rdata_flush(env, *data_, release_memory);
      if(release_memory) delete data_;
    }
  }

  void NativeMethodFrame::update_cached_data() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(arrays_) capi_rarray_update(env, *arrays_);
    if(strings_) capi_rstring_update(env, *strings_);
  }

  Handle NativeMethodEnvironment::get_handle(Object* obj) {
    if(obj->reference_p()) {
      return current_native_frame_->get_handle(state_, obj);
    } else if(obj->fixnum_p()) {
      return reinterpret_cast<VALUE>(obj);
    } else if(obj->symbol_p()) {
      return reinterpret_cast<ID>(obj);
    } else if(obj->nil_p()) {
      return cCApiHandleQnil;
    } else if(obj->false_p()) {
      return cCApiHandleQfalse;
    } else if(obj->true_p()) {
      return cCApiHandleQtrue;
    } else if(obj->undef_p()) {
      return cCApiHandleQundef;
    }

    capi_raise_runtime_error("NativeMethod handle requested for unknown object type");
    return 0; // keep compiler happy
  }

  Handle NativeMethodEnvironment::get_handle_global(Object* obj) {
    Handles& global_handles = state_->shared.global_handles();
    Handle handle = CAPI_APPLY_GLOBAL_TAG(global_handles.size());
    global_handles.push_back(new RootHandle(state_, obj));
    return handle;
  }

  Object* NativeMethodEnvironment::get_object(Handle handle) {
    if(CAPI_REFERENCE_P(handle)) {
      if(CAPI_GLOBAL_HANDLE_P(handle)) {
        Handles& global_handles = state_->shared.global_handles();
        RootHandle* root = global_handles[CAPI_STRIP_GLOBAL_TAG(handle)];
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
      } else {
        return current_native_frame_->get_object(handle);
      }
    } else if(FIXNUM_P(handle)) {
      return reinterpret_cast<Fixnum*>(handle);
    } else if(SYMBOL_P(handle)) {
      return reinterpret_cast<Symbol*>(handle);
    } else if(CAPI_FALSE_P(handle)) {
      return Qfalse;
    } else if(CAPI_TRUE_P(handle)) {
      return Qtrue;
    } else if(CAPI_NIL_P(handle)) {
      return Qnil;
    } else if(CAPI_UNDEF_P(handle)) {
      return Qundef;
    }

    capi_raise_runtime_error("requested Object for unknown NativeMethod handle type");
    return Qnil; // keep compiler happy
  }

  void NativeMethodEnvironment::delete_global(Handle handle) {
    Handles& global_handles = state_->shared.global_handles();
    handle = CAPI_STRIP_GLOBAL_TAG(handle);
    RootHandle* root = global_handles[handle];
    delete root;
    global_handles[handle] = 0;
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

  CApiStructs& NativeMethodEnvironment::data() {
    return current_native_frame_->data();
  }

  void NativeMethodEnvironment::flush_cached_data(bool release_memory) {
    current_native_frame_->flush_cached_data(release_memory);
  }

  void NativeMethodEnvironment::update_cached_data() {
    current_native_frame_->update_cached_data();
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

  Object* NativeMethod::executor_implementation(STATE, CallFrame* call_frame, Dispatch& msg,
                                                Arguments& args) {
    NativeMethodEnvironment* env = native_method_environment.get();
    NativeMethodFrame nmf(env->current_native_frame());

    env->set_current_call_frame(call_frame);
    env->set_current_native_frame(&nmf);

    NativeMethod* nm = as<NativeMethod>(msg.method);

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling()))
      state->profiler()->enter_method(msg, args);
#endif

    Object* ret;
    ExceptionPoint ep(env);

    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      ret = NULL;
    } else {
      ret = nm->call(state, env, args);
    }

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling()))
      state->profiler()->leave();
#endif

    env->current_native_frame()->flush_cached_data(true);
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
  Object* NativeMethod::call(STATE, NativeMethodEnvironment* env, Arguments& args) {
    Handle receiver = env->get_handle(args.recv());

    switch(arity()->to_int()) {
    case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
      Handle ary = env->get_handle(args.as_array(state));

      Handle ret_handle = functor_as<OneArgFunctor>()(ary);

      return env->get_object(ret_handle);
    }

    case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
      Handle ary = env->get_handle(args.as_array(state));

      Handle ret_handle = functor_as<TwoArgFunctor>()(receiver, ary);

      return env->get_object(ret_handle);
    }

    case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
      Handle* ary = new Handle[args.total()];

      for (std::size_t i = 0; i < args.total(); ++i) {
        ary[i] = env->get_handle(args.get_argument(i));
      }

      Handle ret_handle = functor_as<ArgcFunctor>()(args.total(), ary, receiver);

      delete[] ary;

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

      Handle a1 = env->get_handle(args.get_argument(0));

      Handle ret_handle = functor(receiver, a1);

      return env->get_object(ret_handle);
    }

    case 2: {
      ThreeArgFunctor functor = functor_as<ThreeArgFunctor>();

      Handle a1 = env->get_handle(args.get_argument(0));
      Handle a2 = env->get_handle(args.get_argument(1));

      Handle ret_handle = functor(receiver, a1, a2);

      return env->get_object(ret_handle);
    }

    case 3: {
      FourArgFunctor functor = functor_as<FourArgFunctor>();
      Handle a1 = env->get_handle(args.get_argument(0));
      Handle a2 = env->get_handle(args.get_argument(1));
      Handle a3 = env->get_handle(args.get_argument(2));

      Handle ret_handle = functor(receiver, a1, a2, a3);

      return env->get_object(ret_handle);
    }

    case 4: {
      FiveArgFunctor functor = functor_as<FiveArgFunctor>();
      Handle a1 = env->get_handle(args.get_argument(0));
      Handle a2 = env->get_handle(args.get_argument(1));
      Handle a3 = env->get_handle(args.get_argument(2));
      Handle a4 = env->get_handle(args.get_argument(3));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4);

      return env->get_object(ret_handle);
    }

    case 5: {
      SixArgFunctor functor = functor_as<SixArgFunctor>();
      Handle a1 = env->get_handle(args.get_argument(0));
      Handle a2 = env->get_handle(args.get_argument(1));
      Handle a3 = env->get_handle(args.get_argument(2));
      Handle a4 = env->get_handle(args.get_argument(3));
      Handle a5 = env->get_handle(args.get_argument(4));

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
