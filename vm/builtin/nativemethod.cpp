#include <iostream>

#include "vm.hpp"

#include "native_libraries.hpp"
#include "primitives.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"

#include "message.hpp"

namespace rubinius {

  typedef TypedRoot<Object*> RootHandle;

  /** Thread-local framing instance. */
  thread::ThreadData<NativeMethodFraming*> native_method_framing;


/* Class methods */

  NativeMethodFraming* NativeMethodFraming::get() {
    return native_method_framing.get();
  }

  Handle NativeMethodFrame::get_handle(STATE, Object* obj) {
    Handle hndl = handles_.size();
    handles_.push_back(new RootHandle(state, obj));
    return hndl;
  }

  Object* NativeMethodFrame::get_object(Handle hndl) {
    return handles_[hndl]->get();
  }

  Handle NativeMethodFraming::get_handle(Object* obj) {
    return current_native_frame_->get_handle(state_, obj);
  }

  Handle NativeMethodFraming::get_handle_global(Object* obj) {
    Handle hndl = -1 - global_handles_.size();
    global_handles_.push_back(new RootHandle(state_, obj));
    return hndl;
  }

  Object* NativeMethodFraming::get_object(Handle hndl) {
    if(hndl < 0) {
      return global_handles_[-1 - hndl]->get();
    } else {
      return current_native_frame_->get_object(hndl);
    }
  }

  void NativeMethodFraming::delete_global(Handle hndl) {
    RootHandle* root = global_handles_[-1 - hndl];
    delete root;
    global_handles_[-1 - hndl] = 0;
  }

  Object* NativeMethodFraming::block() {
    return current_call_frame_->top_scope->block();
  }

  Handles& NativeMethodFraming::handles() {
    return current_native_frame_->handles();
  }

  void NativeMethod::register_class_with(STATE) {
    state->globals.nmethod.set(state->new_class("NativeMethod", G(executable)));
    state->globals.nmethod.get()->set_object_type(state, NativeMethodType);

    native_method_framing.set(new NativeMethodFraming);
  }

  NativeMethod* NativeMethod::allocate(STATE) {
    return create<GenericFunctor>(state);
  }


  Object* NativeMethod::executor_implementation(STATE, CallFrame* call_frame, Message& msg) {
    NativeMethodFraming* framing = native_method_framing.get();
    NativeMethodFrame nmf(framing->current_native_frame());

    framing->set_current_call_frame(call_frame);
    framing->set_current_native_frame(&nmf);
    framing->set_state(state);

    NativeMethod* nm = as<NativeMethod>(msg.method);
    Object* ret = nm->call(state, &nmf, msg);

    framing->set_current_native_frame(nmf.previous());

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
  Object* NativeMethod::call(STATE, NativeMethodFrame* frame, Message& msg) {
    Handle receiver = frame->get_handle(state, msg.recv);

    switch(arity()->to_int()) {
    case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
      Handle args = frame->get_handle(state, msg.as_array(state));

      Handle ret_handle = functor_as<OneArgFunctor>()(args);

      return frame->get_object(ret_handle);
    }

    case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
      Handle args = frame->get_handle(state, msg.as_array(state));

      Handle ret_handle = functor_as<TwoArgFunctor>()(receiver, args);

      return frame->get_object(ret_handle);
    }

    case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
      Handle* args = new Handle[msg.args()];

      for (std::size_t i = 0; i < msg.args(); ++i) {
        args[i] = frame->get_handle(state, msg.get_argument(i));
      }

      Handle ret_handle = functor_as<ArgcFunctor>()(msg.args(), args, receiver);

      delete[] args;

      return frame->get_object(ret_handle);
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

      return frame->get_object(ret_handle);
    }

    case 1: {
      TwoArgFunctor functor = functor_as<TwoArgFunctor>();

      Handle a1 = frame->get_handle(state, msg.get_argument(0));

      Handle ret_handle = functor(receiver, a1);

      return frame->get_object(ret_handle);
    }

    case 2: {
      ThreeArgFunctor functor = functor_as<ThreeArgFunctor>();

      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));

      Handle ret_handle = functor(receiver, a1, a2);

      return frame->get_object(ret_handle);
    }

    case 3: {
      FourArgFunctor functor = functor_as<FourArgFunctor>();
      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));
      Handle a3 = frame->get_handle(state, msg.get_argument(2));

      Handle ret_handle = functor(receiver, a1, a2, a3);

      return frame->get_object(ret_handle);
    }

    case 4: {
      FiveArgFunctor functor = functor_as<FiveArgFunctor>();
      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));
      Handle a3 = frame->get_handle(state, msg.get_argument(2));
      Handle a4 = frame->get_handle(state, msg.get_argument(3));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4);

      return frame->get_object(ret_handle);
    }

    case 5: {
      SixArgFunctor functor = functor_as<SixArgFunctor>();
      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));
      Handle a3 = frame->get_handle(state, msg.get_argument(2));
      Handle a4 = frame->get_handle(state, msg.get_argument(3));
      Handle a5 = frame->get_handle(state, msg.get_argument(4));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4, a5);

      return frame->get_object(ret_handle);
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
