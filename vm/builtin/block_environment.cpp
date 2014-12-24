#include "arguments.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/jit.hpp"
#include "builtin/location.hpp"
#include "builtin/native_method.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/tuple.hpp"
#include "builtin/variable_scope.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "instruments/tooling.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

#include <iostream>

namespace rubinius {

  void BlockEnvironment::init(STATE) {
    GO(blokenv).set(ontology::new_class(state, "BlockEnvironment", G(object),
                                     G(rubinius)));
    G(blokenv)->set_object_type(state, BlockEnvironmentType);


  }

  void BlockEnvironment::bootstrap_methods(STATE) {
    GCTokenImpl gct;
    System::attach_primitive(state, gct,
                             G(blokenv), false,
                             state->symbol("call_under"),
                             state->symbol("block_call_under"));
  }

  void BlockEnvironment::lock_scope(STATE) {
    if(scope_ && !scope_->nil_p()) {
      scope_->set_locked(state);
    }
    if(top_scope_ && !top_scope_->nil_p()) {
      top_scope_->set_locked(state);
    }
  }

  BlockEnvironment* BlockEnvironment::allocate(STATE) {
    BlockEnvironment* env = state->new_object<BlockEnvironment>(G(blokenv));
    return env;
  }

  MachineCode* BlockEnvironment::machine_code(STATE, GCToken gct, CallFrame* call_frame) {
    return compiled_code_->internalize(state, gct, call_frame);
  }

  Object* BlockEnvironment::invoke(STATE, CallFrame* previous,
                            BlockEnvironment* env, Arguments& args,
                            BlockInvocation& invocation)
  {
    MachineCode* mcode = env->compiled_code_->machine_code();

    if(!mcode) {
      OnStack<3> os(state, env, args.recv_location(), args.block_location());
      OnStack<3> iv(state, invocation.self, invocation.constant_scope, invocation.module);
      VariableRootBuffer vrb(state->vm()->current_root_buffers(),
                             &args.arguments_location(), args.total());
      GCTokenImpl gct;

      mcode = env->machine_code(state, gct, previous);

      if(!mcode) {
        Exception::internal_error(state, previous, "invalid bytecode method");
        return 0;
      }
    }

#ifdef ENABLE_LLVM
    if(executor ptr = mcode->unspecialized) {
      return (*((BlockExecutor)ptr))(state, previous, env, args, invocation);
    }
#endif

    return execute_interpreter(state, previous, env, args, invocation);
  }

  // TODO: this is a quick hack to process block arguments in 1.9.
  class GenericArguments {
  public:
    static bool call(STATE, CallFrame* call_frame,
                     MachineCode* mcode, StackVariables* scope,
                     Arguments& args, int flags)
    {
      /* There are 5 types of arguments, illustrated here:
       *    m(a, b=1, *c, d, e: 2)
       *
       *  where:
       *    a is a head (pre optional/splat) fixed position argument
       *    b is an optional argument
       *    c is a rest argument
       *    d is a post (optional/splat) argument
       *    e is a keyword argument, which may be required (having no default
       *      value), optional, or keyword rest argument (**kw).
       *
       * The arity checking above ensures that we have at least one argument
       * on the stack for each fixed position argument (ie arguments a and d
       * above).
       *
       * We assign the arguments in the following order: first the fixed
       * arguments (head and post) and possibly the keyword argument, then the
       * optional arguments, and the remainder (if any) are combined in an
       * array for the rest argument.
       *
       * We assign values from the sender's side to local variables on the
       * receiver's side. Which values to assign are computed as follows:
       *
       *  sender indexes (arguments)
       *  -v-v-v-v-v-v-v-v-v-v-v-v--
       *
       *   0...H  H...H+ON  H+ON...N-P-K  N-P-K...N-K  N-K
       *   |      |         |             |            |
       *   H      O         R             P            K
       *   |      |         |             |            |
       *   0...H  H...H+O   RI            PI...PI+P    KI
       *
       *  -^-^-^-^-^-^-^-^-^-^-^-^-
       *  receiver indexes (locals)
       *
       * where:
       *
       *  arguments passed by sender
       *  --------------------------
       *    N  : total number of arguments passed
       *    H* : number of head arguments
       *    E  : number of extra arguments
       *    ON : number or arguments assigned to optional parameters
       *    RN : number of arguments assigned to the rest argument
       *    P* : number of post arguments
       *    K  : number of keyword arguments passed, 1 if the last argument is
       *         a Hash or if #to_hash returns a Hash, 0 otherwise
       *    HL : maximum number of head arguments passed
       *    PM : post arguments missing when N < M
       *
       *  parameters defined by receiver
       *  ------------------------------
       *    T  : total number of parameters
       *    M  : number of head + post parameters
       *    H* : number of head parameters
       *    O  : number of optional parameters
       *    RP : true if a rest parameter is defined, false otherwise
       *    RI : index of rest parameter if RP is true, else -1
       *    P* : number of post parameters
       *    PI : index of the first post parameter
       *    KP : true if a keyword parameter is defined, false otherwise
       *    KA : true if the keyword argument was extracted from the passed
       *         argument leaving a remaining value
       *    KI : index of keyword rest parameter
       *
       *  (*) The values of H and P are fixed and they represent the same
       *  values at both the sender and receiver, so they are named the same.
       *
       *  formulas
       *  --------
       *    K  = KP && !KA && N > M ? 1 : 0
       *    E  = N - M - K
       *    O  = T - M - (keywords ? 1 : 0)
       *    ON = (X = MIN(O, E)) > 0 ? X : 0
       *    RN = RP && (X = E - ON) > 0 ? X : 0
       *    PI = H + O + (RP ? 1 : 0)
       *    KI = RP ? T : T - 1
       *    HL = (H - N) > 0 ? MIN(N, H - N) : H
       *    PM = N - H > 0 ? P - (N - H) : P
       *
       */

      native_int N = args.total();
      const native_int T = mcode->total_args;
      const native_int M = mcode->required_args;
      const native_int O = T - M - (mcode->keywords ? 1 : 0);


      /* TODO: Clean up usage to uniformly refer to 'splat' as N arguments
       * passed from sender at a single position and 'rest' as N arguments
       * collected into a single argument at the receiver.
       */
      const native_int RI = mcode->splat_position;
      const bool RP = (RI >= 0);

      // expecting 0, got 0.
      if(T == 0 && N == 0) {
        if(RP) {
          scope->set_local(mcode->splat_position, Array::create(state, 0));
        }

        return true;
      }

      const bool lambda = ((flags & CallFrame::cIsLambda) == CallFrame::cIsLambda);

      // Only do destructuring in non-lambda mode
      if(!lambda) {
        /* If only one argument was yielded and the block takes more than one
         * argument or has form { |a, | }:
         *
         *  1. If the object is an Array, assign elements to arguments.
         *  2. If the object returns 'nil' from #to_ary, assign the object
         *     to the first argument.
         *  3. If the object returns an Array from #to_ary, assign the
         *     elements to the arguments.
         *  4. If the object returns non-Array from #to_ary, raise a TypeError.
         */
        if(N == 1 && (T > 1 || (RP && T > 0) || RI < -2)) {
          Object* obj = args.get_argument(0);
          Array* ary = 0;

          if(!(ary = try_as<Array>(obj))) {
            if(CBOOL(obj->respond_to(state, G(sym_to_ary), cFalse))) {
              if(!(obj = obj->send(state, call_frame, G(sym_to_ary)))) {
                return false;
              }

              if(!(ary = try_as<Array>(obj)) && !obj->nil_p()) {
                Exception::type_error(state, "to_ary must return an Array", call_frame);
                return false;
              }
            }
          }

          if(ary) {
            if(RI == -4 && M == 1) {
              args.use_argument(ary);
            } else {
              args.use_array(ary);
            }

            N = args.total();
          }
        }
      }

      const native_int P = mcode->post_args;
      const native_int H = M - P;

      // Too many args (no rest argument!)
      if(!RP && N > T) {
        if(lambda) return false;

        N = T;
      }

      // Too few args!
      if(lambda && N < M) return false;

      Object* kw = 0;
      Object* kw_remainder = 0;
      bool KP = false;
      bool KA = false;

      if(mcode->keywords && N > M) {
        Object* obj = args.get_argument(args.total() - 1);

        OnStack<1> os(state, obj);
        Object* arguments[2];

        arguments[0] = obj;
        arguments[1] = RBOOL(O > 0 || RP);
        Arguments args(G(sym_keyword_object), G(runtime), 2, arguments);
        Dispatch dis(G(sym_keyword_object));

        Object* kw_result = dis.send(state, call_frame, args);

        if(kw_result) {
          if(Array* ary = try_as<Array>(kw_result)) {
            Object* o = 0;

            if(!(o = ary->get(state, 0))->nil_p()) {
              kw_remainder = o;
              KA = true;
            }

            kw = ary->get(state, 1);
            KP = true;
          }
        } else {
          return false;
        }
      }

      // A single kwrest argument
      if(mcode->keywords && !RP && !KP && N >= T) {
        if(lambda) return false;

        N = T - 1;
      }

      const native_int K = (KP && !KA && N > M) ? 1 : 0;
      const native_int N_M_K = N - M - K;
      const native_int E = N_M_K > 0 ? N_M_K : 0;

      native_int X;

      const native_int ON = (X = MIN(O, E)) > 0 ? X : 0;
      const native_int RN = (RP && (X = E - ON) > 0) ? X : 0;
      const native_int PI = H + O + (RP ? 1 : 0);
      const native_int KI = RP ? T : T - 1;

      native_int a = 0;   // argument index
      native_int l = 0;   // local index

      // head arguments
      if(H > 0) {
        for(; l < H && a < N; l++, a++) {
          scope->set_local(l, args.get_argument(a));
        }

        for(; l < H; l++) {
          scope->set_local(l, cNil);
        }
      }

      // optional arguments
      if(O > 0) {
        for(; l < H + O && a < MIN(N, H + ON); l++, a++) {
          if(unlikely(kw_remainder && !RP && (a == N - 1))) {
            scope->set_local(l, kw_remainder);
          } else {
            scope->set_local(l, args.get_argument(a));
          }
        }

        for(; l < H + O; l++) {
          scope->set_local(l, G(undefined));
        }
      }

      // rest arguments
      if(RP) {
        Array* ary;

        if(RN > 0) {
          ary = Array::create(state, RN);

          for(int i = 0; i < RN && a < N - P - K; i++, a++) {
            if(unlikely(kw_remainder && (a == N - 1))) {
              ary->set(state, i, kw_remainder);
            } else {
              ary->set(state, i, args.get_argument(a));
            }
          }
        } else {
          ary = Array::create(state, 0);
        }

        scope->set_local(RI, ary);
      }

      // post arguments
      if(P > 0) {
        const native_int N_K = (X = MIN(N, N - K)) > 0 ? X : N;

        for(l = PI; l < PI + P && a < N_K; l++, a++) {
          scope->set_local(l, args.get_argument(a));
        }

        for(; l < PI + P; l++) {
          scope->set_local(l, cNil);
        }
      }

      // keywords
      if(kw) {
        scope->set_local(KI, kw);
      }

      return true;
    }
  };

  // Installed by default in BlockEnvironment::execute, it runs the bytecodes
  // for the block in the interpreter.
  //
  // Future code will detect hot blocks and queue them in the JIT, whereby the
  // JIT will install a newly minted machine function into ::execute.
  Object* BlockEnvironment::execute_interpreter(STATE, CallFrame* previous,
                            BlockEnvironment* env, Arguments& args,
                            BlockInvocation& invocation)
  {
    // Don't use env->machine_code() because it might lock and the work should
    // already be done.
    MachineCode* const mcode = env->compiled_code_->machine_code();

    if(!mcode) {
      Exception::internal_error(state, previous, "invalid bytecode method");
      return 0;
    }

#ifdef ENABLE_LLVM
    if(mcode->call_count >= 0) {
      if(mcode->call_count >= state->shared().config.jit_threshold_compile) {
        OnStack<1> os(state, env);

        G(jit)->compile_soon(state, env->compiled_code(), previous,
            invocation.self->direct_class(state), env, true);
      } else {
        mcode->call_count++;
      }
    }
#endif

    StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);

    Module* mod = invocation.module;
    if(!mod) mod = env->module();

    Object* block = cNil;
    if(VariableScope* scope = env->top_scope_) {
      if(!scope->nil_p()) block = scope->block();
    }

    scope->initialize(invocation.self, block, mod, mcode->number_of_locals);
    scope->set_parent(env->scope_);

    InterpreterCallFrame* frame = ALLOCA_CALLFRAME(mcode->stack_size);

    frame->prepare(mcode->stack_size);

    frame->previous = previous;
    frame->constant_scope_ = invocation.constant_scope;

    frame->arguments = &args;
    frame->dispatch_data = env;
    frame->compiled_code = env->compiled_code_;
    frame->scope = scope;
    frame->top_scope_ = env->top_scope_;
    frame->flags = invocation.flags | CallFrame::cMultipleScopes
                                    | CallFrame::cBlock;

    if(!GenericArguments::call(state, frame, mcode, scope, args, invocation.flags)) {
      if(state->vm()->thread_state()->raise_reason() == cNone) {
        Exception* exc =
          Exception::make_argument_error(state, mcode->required_args, args.total(),
                                         mcode->name());
        exc->locations(state, Location::from_call_stack(state, previous));
        state->raise_exception(exc);
      }
      return NULL;
    }

#ifdef RBX_PROFILER
    if(unlikely(state->vm()->tooling())) {
      Module* mod = scope->module();
      if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
        if(Module* ma = try_as<Module>(sc->singleton())) {
          mod = ma;
        }
      }

      OnStack<2> os(state, env, mod);

      // Check the stack and interrupts here rather than in the interpreter
      // loop itself.

      GCTokenImpl gct;

      if(!state->check_interrupts(gct, frame, frame)) return NULL;

      state->checkpoint(gct, frame);

      tooling::BlockEntry method(state, env, mod);
      return (*mcode->run)(state, mcode, frame);
    } else {
      // Check the stack and interrupts here rather than in the interpreter
      // loop itself.

      GCTokenImpl gct;

      if(!state->check_interrupts(gct, frame, frame)) return NULL;

      state->checkpoint(gct, frame);
      return (*mcode->run)(state, mcode, frame);
    }
#else
    // Check the stack and interrupts here rather than in the interpreter
    // loop itself.

    GCTokenImpl gct;

    if(!state->check_interrupts(gct, frame, frame)) return NULL;

    state->checkpoint(gct, frame);
    return (*mcode->run)(state, mcode, frame);
#endif
  }

  Object* BlockEnvironment::call(STATE, CallFrame* call_frame,
                                 Arguments& args, int flags)
  {
    BlockInvocation invocation(scope_->self(), constant_scope_, flags);
    return invoke(state, call_frame, this, args, invocation);
  }

  Object* BlockEnvironment::call_prim(STATE, CallFrame* call_frame,
                                      Executable* exec, Module* mod,
                                      Arguments& args)
  {
    return call(state, call_frame, args);
  }

  Object* BlockEnvironment::call_on_object(STATE, CallFrame* call_frame,
                                           Arguments& args, int flags)
  {
    if(args.total() < 1) {
      Exception* exc =
        Exception::make_argument_error(state, 1, args.total(),
                                       compiled_code_->name());
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.shift(state);

    BlockInvocation invocation(recv, constant_scope_, flags);
    return invoke(state, call_frame, this, args, invocation);
  }

  Object* BlockEnvironment::call_under(STATE, CallFrame* call_frame,
                                       Executable* exec, Module* mod,
                                       Arguments& args)
  {
    if(args.total() < 3) {
      Exception* exc =
        Exception::make_argument_error(state, 3, args.total(),
                                       compiled_code_->name());
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.shift(state);
    ConstantScope* constant_scope = as<ConstantScope>(args.shift(state));
    Object* visibility_scope = args.shift(state);

    int flags = CBOOL(visibility_scope) ? CallFrame::cTopLevelVisibility : 0;
    BlockInvocation invocation(recv, constant_scope, flags);
    return invoke(state, call_frame, this, args, invocation);
  }


  BlockEnvironment* BlockEnvironment::under_call_frame(STATE, GCToken gct,
      CompiledCode* ccode, MachineCode* caller,
      CallFrame* call_frame)
  {

    MachineCode* mcode = ccode->machine_code();
    if(!mcode) {
      OnStack<1> os(state, ccode);
      mcode = ccode->internalize(state, gct, call_frame);
      if(!mcode) {
        Exception::internal_error(state, call_frame, "invalid bytecode method");
        return 0;
      }
    }

    BlockEnvironment* be = state->new_object_dirty<BlockEnvironment>(G(blokenv));
    be->scope(state, call_frame->promote_scope(state));
    be->top_scope(state, call_frame->top_scope(state));
    be->compiled_code(state, ccode);
    be->constant_scope(state, call_frame->constant_scope());
    be->module(state, call_frame->module());
    return be;
  }

  BlockEnvironment* BlockEnvironment::dup(STATE) {
    BlockEnvironment* be = state->new_object_dirty<BlockEnvironment>(G(blokenv));

    be->scope(state, scope_);
    be->top_scope(state, top_scope_);
    be->compiled_code(state, compiled_code_);
    be->constant_scope(state, constant_scope_);
    be->module(state, nil<Module>());
    return be;
  }

  Object* BlockEnvironment::of_sender(STATE, CallFrame* call_frame) {
    if(NativeMethodFrame* nmf = call_frame->previous->native_method_frame()) {
      return state->vm()->native_method_environment->get_object(nmf->block());
    }

    CallFrame* target = call_frame->previous->top_ruby_frame();

    // We assume that code using this is going to use it over and
    // over again (ie Proc.new) so we mark the method as not
    // inlinable so that this works even with the JIT on.

    if(!target) {
      return cNil;
    }

    target->compiled_code->machine_code()->set_no_inline();

    if(target->scope) {
      return target->scope->block();
    }

    return cNil;
  }

  void BlockEnvironment::Info::show(STATE, Object* self, int level) {
    BlockEnvironment* be = as<BlockEnvironment>(self);

    class_header(state, self);
    //indent_attribute(++level, "scope"); be->scope()->show(state, level);
    // indent_attribute(level, "top_scope"); be->top_scope()->show(state, level);
    indent_attribute(level, "compiled_code"); be->compiled_code()->show(state, level);
    close_body(level);
  }
}
