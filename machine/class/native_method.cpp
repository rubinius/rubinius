#include "c_api.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "exception.hpp"
#include "exception_point.hpp"
#include "memory.hpp"
#include "on_stack.hpp"

#include "class/array.hpp"
#include "class/data.hpp"
#include "class/exception.hpp"
#include "class/ffi_pointer.hpp"
#include "class/float.hpp"
#include "class/location.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"
#include "class/tuple.hpp"

#include "capi/capi.hpp"

#include "diagnostics/machine.hpp"

#include "dtrace/dtrace.h"

namespace rubinius {
  static thread_local NativeMethodEnvironment* native_method_environment = nullptr;

/* Class methods */

  NativeMethodEnvironment::NativeMethodEnvironment(STATE)
    : state_(state)
    , current_call_frame_(0)
    , current_native_frame_(0)
    , current_ep_(0)
    , outgoing_block_(0)
    , metrics_(new diagnostics::MachineMetrics())
  {}

  NativeMethodEnvironment* NativeMethodEnvironment::get() {
    return native_method_environment;
  }

  NativeMethodFrame::NativeMethodFrame(NativeMethodEnvironment* env, NativeMethodFrame* prev, NativeMethod* method)
    : _previous_(prev)
    , _env_(env)
    , _capi_lock_index_(method ? method->capi_lock_index() : 0)
    , _block_(Qnil)
    , _receiver_(Qnil)
    , _module_(Qnil)
    , _method_(Qnil)
  {}

  Object* NativeMethodEnvironment::block() {
    if(!current_native_frame_) return cNil;
    return MemoryHandle::object(current_native_frame()->block());
  }

  StackVariables* NativeMethodEnvironment::scope() {
    if(CallFrame* frame = state()->get_scope_frame()) {
      return frame->scope;
    }

    return NULL;
  }

  void NativeMethod::bootstrap(STATE) {
    GO(nmethod).set(state->memory()->new_class<Class, NativeMethod>(
          state, G(executable), G(rubinius), "NativeMethod"));

    init_thread(state);
  }

  void NativeMethod::initialize(STATE, NativeMethod* obj) {
    Executable::initialize(state, obj);

    obj->arity(nil<Fixnum>());
    obj->file(nil<String>());
    obj->name(nil<Symbol>());
    obj->module(nil<Module>());
    obj->func_ = NULL;
    obj->capi_lock_index(0);
  }

  void NativeMethod::init_thread(STATE) {
    NativeMethodEnvironment* env = new NativeMethodEnvironment(state);
    native_method_environment = env;
    state->native_method_environment = env;
  }

  void NativeMethod::cleanup_thread(STATE) {
    delete state->native_method_environment;
    state->native_method_environment = nullptr;
    native_method_environment = nullptr;
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
      return MemoryHandle::object(nm->func()(receiver));
    }
  };

  class OneArgument {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      VALUE a1 = MemoryHandle::value(args.get_argument(0));

      return MemoryHandle::object(nm->func()(receiver, a1));
    }
  };

  class TwoArguments {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      VALUE a1 = MemoryHandle::value(args.get_argument(0));
      VALUE a2 = MemoryHandle::value(args.get_argument(1));

      return MemoryHandle::object(nm->func()(receiver, a1, a2));
    }
  };

  class ThreeArguments {
  public:
    static Object* invoke(STATE, NativeMethod* nm, NativeMethodEnvironment* env,
                          Arguments& args)
    {
      VALUE receiver = env->current_native_frame()->receiver();
      VALUE a1 = MemoryHandle::value(args.get_argument(0));
      VALUE a2 = MemoryHandle::value(args.get_argument(1));
      VALUE a3 = MemoryHandle::value(args.get_argument(2));

      return MemoryHandle::object(nm->func()(receiver, a1, a2, a3));
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
        VALUE ary = MemoryHandle::value(args.as_array(state));

        VALUE ret = nm->func()(ary);

        return MemoryHandle::object(ret);
      }

      case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
        VALUE ary = MemoryHandle::value(args.as_array(state));

        VALUE ret = nm->func()(receiver, ary);

        return MemoryHandle::object(ret);
      }

      case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
        VALUE ary[args.total()];

        for (std::size_t i = 0; i < args.total(); ++i) {
          ary[i] = MemoryHandle::value(args.get_argument(i));
        }

        VALUE ret = nm->func_as<ArgcFunction>()(args.total(), ary, receiver);

        return MemoryHandle::object(ret);
      }

        /*
         *  Normal arg counts
         *
         */

      case 0:
        return MemoryHandle::object(nm->func()(receiver));

      case 1: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));

        VALUE ret = nm->func()(receiver, a1);

        return MemoryHandle::object(ret);
      }

      case 2: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));

        VALUE ret = nm->func()(receiver, a1, a2);

        return MemoryHandle::object(ret);
      }

      case 3: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));

        VALUE ret = nm->func()(receiver, a1, a2, a3);

        return MemoryHandle::object(ret);
      }

      case 4: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4);

        return MemoryHandle::object(ret);
      }

      case 5: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5);

        return MemoryHandle::object(ret);
      }

      case 6: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6);

        return MemoryHandle::object(ret);
      }

      case 7: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7);

        return MemoryHandle::object(ret);
      }

      case 8: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7, a8);

        return MemoryHandle::object(ret);
      }

      case 9: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7, a8, a9);

        return MemoryHandle::object(ret);
      }

      case 10: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));
        VALUE a10 = MemoryHandle::value(args.get_argument(9));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10);

        return MemoryHandle::object(ret);
      }

      case 11: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));
        VALUE a10 = MemoryHandle::value(args.get_argument(9));
        VALUE a11 = MemoryHandle::value(args.get_argument(10));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11);

        return MemoryHandle::object(ret);
      }

      case 12: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));
        VALUE a10 = MemoryHandle::value(args.get_argument(9));
        VALUE a11 = MemoryHandle::value(args.get_argument(10));
        VALUE a12 = MemoryHandle::value(args.get_argument(11));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12);

        return MemoryHandle::object(ret);
      }

      case 13: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));
        VALUE a10 = MemoryHandle::value(args.get_argument(9));
        VALUE a11 = MemoryHandle::value(args.get_argument(10));
        VALUE a12 = MemoryHandle::value(args.get_argument(11));
        VALUE a13 = MemoryHandle::value(args.get_argument(12));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12, a13);

        return MemoryHandle::object(ret);
      }

      case 14: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));
        VALUE a10 = MemoryHandle::value(args.get_argument(9));
        VALUE a11 = MemoryHandle::value(args.get_argument(10));
        VALUE a12 = MemoryHandle::value(args.get_argument(11));
        VALUE a13 = MemoryHandle::value(args.get_argument(12));
        VALUE a14 = MemoryHandle::value(args.get_argument(13));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12, a13, a14);

        return MemoryHandle::object(ret);
      }

      case 15: {
        VALUE a1 = MemoryHandle::value(args.get_argument(0));
        VALUE a2 = MemoryHandle::value(args.get_argument(1));
        VALUE a3 = MemoryHandle::value(args.get_argument(2));
        VALUE a4 = MemoryHandle::value(args.get_argument(3));
        VALUE a5 = MemoryHandle::value(args.get_argument(4));
        VALUE a6 = MemoryHandle::value(args.get_argument(5));
        VALUE a7 = MemoryHandle::value(args.get_argument(6));
        VALUE a8 = MemoryHandle::value(args.get_argument(7));
        VALUE a9 = MemoryHandle::value(args.get_argument(8));
        VALUE a10 = MemoryHandle::value(args.get_argument(9));
        VALUE a11 = MemoryHandle::value(args.get_argument(10));
        VALUE a12 = MemoryHandle::value(args.get_argument(11));
        VALUE a13 = MemoryHandle::value(args.get_argument(12));
        VALUE a14 = MemoryHandle::value(args.get_argument(13));
        VALUE a15 = MemoryHandle::value(args.get_argument(14));

        VALUE ret = nm->func()(receiver, a1, a2, a3, a4, a5, a6, a7,
                               a8, a9, a10, a11, a12, a13, a14, a15);

        return MemoryHandle::object(ret);
      }
        /* Extension entry point, should never occur for user code. */
      case INIT_FUNCTION: {
        nm->func_as<InitFunction>()();

        return cNil;
      }

        /* A C function being used as a block */
      case ITERATE_BLOCK: {
        VALUE cb_data = MemoryHandle::value(nm->get_ivar(state, state->symbol("cb_data")));

        Object* ob = nm->get_ivar(state, state->symbol("original_block"));
        if(!ob->nil_p()) {
          env->current_native_frame()->block(MemoryHandle::value(ob));
        }

        VALUE val;

        switch(args.total()) {
        case 0:
          val = MemoryHandle::value(cNil);
          break;
        case 1:
          val = MemoryHandle::value(args.get_argument(0));
          break;
        default:
          val = MemoryHandle::value(args.as_array(state));
          break;
        }

        VALUE ret = nm->func()(val, cb_data, receiver);
        return MemoryHandle::object(ret);
      }

      case C_BLOCK_CALL: {
        VALUE val;
        VALUE ary[args.total()];
        VALUE cb_data = MemoryHandle::value(nm->get_ivar(state, state->symbol("cb_data")));

        if(args.total() > 0) {
          val = MemoryHandle::value(args.get_argument(0));
        } else {
          val = MemoryHandle::value(cNil);
        }

        for (std::size_t i = 0; i < args.total(); ++i) {
          ary[i] = MemoryHandle::value(args.get_argument(i));
        }

        VALUE ret = nm->func()(val, cb_data, args.total(), ary);

        return MemoryHandle::object(ret);
      }

      case C_LAMBDA: {
        VALUE cb_data = MemoryHandle::value(nm->get_ivar(state, state->symbol("cb_data")));
        VALUE val = MemoryHandle::value(args.as_array(state));
        VALUE ret = nm->func()(val, cb_data);
        return MemoryHandle::object(ret);
      }

      case C_CALLBACK: {
        VALUE cb_data = MemoryHandle::value(nm->get_ivar(state, state->symbol("cb_data")));

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
      Executable* exec, Module* mod, Arguments& args) {
    NativeMethod* nm = as<NativeMethod>(exec);

    int arity = nm->arity()->to_int();

    if(arity >= 0 && (size_t)arity != args.total()) {
      Exception* exc = Exception::make_argument_error(
          state, arity, args.total(), args.name()->cpp_str(state).c_str());
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);

      return NULL;
    }

    NativeMethodEnvironment* env = state->native_method_environment;

    NativeMethodFrame nmf(env, env->current_native_frame(), nm);

    uintptr_t* mem = ALLOCA_CALL_FRAME(0);
    CallFrame* call_frame = new(mem) CallFrame();

    call_frame->lexical_scope_ = nullptr;
    call_frame->dispatch_data = (void*)&nmf;
    call_frame->compiled_code = nullptr;
    call_frame->flags = CallFrame::cNativeMethod;
    call_frame->top_scope_ = nullptr;
    call_frame->scope = nullptr;
    call_frame->arguments = &args;
    call_frame->unwind = nullptr;

    CallFrame* saved_frame = env->current_call_frame();
    env->set_current_call_frame(call_frame);
    env->set_current_native_frame(&nmf);

    // Register the CallFrame, because we might GC below this.
    if(!state->push_call_frame(state, call_frame)) {
      return NULL;
    }

    // Be sure to do this after installing nmf as the current
    // native frame.
    nmf.setup(
        MemoryHandle::value(args.recv()),
        MemoryHandle::value(args.block()),
        MemoryHandle::value(exec),
        MemoryHandle::value(mod));

    // We've got things setup (they can be GC'd properly), so we need to
    // wait before entering the extension code.
    ENTER_CAPI(state);

    Object* value;
    ExceptionPoint ep(env);

    try {
      RUBINIUS_METHOD_NATIVE_ENTRY_HOOK(state, mod, args.name());

      PLACE_EXCEPTION_POINT(ep);

      if(unlikely(ep.jumped_to())) {
        value = NULL;
      } else {
        value = ArgumentHandler::invoke(state, nm, env, args);
      }
      RUBINIUS_METHOD_NATIVE_RETURN_HOOK(state, mod, args.name());
    } catch(const RubyException& exc) {
      LEAVE_CAPI(state);

      state->pop_call_frame(state, call_frame->previous);
      env->set_current_call_frame(saved_frame);
      env->set_current_native_frame(nmf.previous());
      ep.pop(env);

      throw(exc);
    }

    LEAVE_CAPI(state);

    if(!state->pop_call_frame(state, call_frame->previous)) {
      value = NULL;
    }

    env->set_current_call_frame(saved_frame);
    env->set_current_native_frame(nmf.previous());
    ep.pop(env);

    return value;
  }

  NativeMethod* NativeMethod::load_extension_entry_point(STATE,
      String* library, Symbol* name, Pointer* ptr) {
    void* func = ptr->pointer;

    int lock_index = state->c_api()->capi_lock_index(name->debug_str(state));

    return NativeMethod::create(state, library, G(rubinius),
                                name, func,
                                Fixnum::from(INIT_FUNCTION),
                                lock_index);
  }

  NativeMethod* NativeMethod::create(STATE, String* file_name, Module* module,
      Symbol* method_name, void* func, Fixnum* arity, int lock_index)
  {
    NativeMethod* nmethod =
      state->memory()->new_object<NativeMethod>(state, G(nmethod));

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
    nmethod->capi_lock_index(lock_index);

    return nmethod;
  }


}
