#include "arguments.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"
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
#include "version.h"

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
      OnStack<2> os(state, env, args.argument_container_location());
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
      const bool has_splat = (mcode->splat_position >= 0);
      native_int total_args = args.total();

      // expecting 0, got 0.
      if(mcode->total_args == 0 && total_args == 0) {
        if(has_splat) {
          scope->set_local(mcode->splat_position, Array::create(state, 0));
        }

        return true;
      }

      // Only do destructuring in non-lambda mode
      if((flags & CallFrame::cIsLambda) == 0) {
        /* If only one argument was yielded and:
         *
         *  1. the block takes two or more arguments
         *  2. OR takes one argument and a splat
         *  3. OR has the form { |a, | }
         *  4. OR has the form { |(a, b)| }
         *  5. OR has the form { |(a, b), c| }
         *
         * then we check if the one argument is an Array. If it is not, call
         * #to_ary to convert it to an Array and raise if #to_ary does not
         * return an Array.
         *
         * Finally, in cases 1-3, and 5 above, we destructure the Array into
         * the block's arguments.
         */
        if(total_args == 1
            && (mcode->required_args > 1
              || (mcode->required_args == 1
                && (has_splat || mcode->splat_position < -2)))) {
          Object* obj = args.get_argument(0);
          Array* ary = 0;

          if(!(ary = try_as<Array>(obj))) {
            if(CBOOL(obj->respond_to(state, G(sym_to_ary), cFalse))) {
              obj = obj->send(state, call_frame, G(sym_to_ary));
              if(!obj) return false;
              if(!(ary = try_as<Array>(obj))) {
                Exception::type_error(state, "to_ary must return an Array", call_frame);
                return false;
              }
            }
          }

          if(ary) {
            if(mcode->splat_position == -4 && mcode->required_args == 1) {
              args.use_argument(ary);
            } else {
              args.use_array(ary);
            }
          }
        }
      }

      const native_int P = mcode->post_args;
      const native_int R = mcode->required_args;

      // M is for mandatory
      const native_int M = R - P;
      const native_int T = args.total();

      // DT is for declared total
      const native_int DT = mcode->total_args;
      const native_int O = DT - R;

      // HS is for has splat
      const native_int HS = mcode->splat_position >= 0 ? 1 : 0;

      // CT is for clamped total
      const native_int CT = HS ? T : MIN(T, DT);

      // Z is for the available # of post args
      const native_int Z = CT - M;

      // U is for the available # of optional args
      const native_int U = Z - P;

      // PAO is for the post-args offset
      // PLO is for the post-arg locals offset
      const native_int PAO = CT - MIN(Z, P);
      const native_int PLO = M + O + HS;

      /* There are 4 types of arguments, illustrated here:
       *    m(a, b=1, *c, d)
       *
       *  where:
       *    a is a (pre optional/splat) fixed position argument
       *    b is an optional argument
       *    c is a splat argument
       *    d is a post (optional/splat) argument
       *
       *  The arity checking above ensures that we have at least one argument
       *  on the stack for each fixed position argument (ie arguments a and d
       *  above).
       *
       *  The number of (pre) fixed arguments is 'required_args - post_args'.
       *
       *  The number of optional arguments is 'total_args - required_args'.
       *
       *  We fill in the required arguments, then the optional arguments, and
       *  the rest (if any) go into an array for the splat.
       */

      // Phase 1, mandatory args
      for(native_int i = 0, l = MIN(M,T);
          i < l;
          i++)
      {
        scope->set_local(i, args.get_argument(i));
      }

      // Phase 2, post args
      for(native_int i = 0; i < MIN(Z, P); i++)
      {
        scope->set_local(PLO + i, args.get_argument(PAO + i));
      }

      // Phase 3, optionals

      for(native_int i = M, limit = M + MIN(U, O);
          i < limit;
          i++)
      {
        scope->set_local(i, args.get_argument(i));
      }


      if(has_splat) {
        Array* ary;
        /* There is a splat. So if the passed in arguments are greater
         * than the total number of fixed arguments, put the rest of the
         * arguments into the Array.
         *
         * Otherwise, generate an empty Array.
         *
         * NOTE: remember that total includes the number of fixed arguments,
         * even if they're optional, so we can get args.total() == 0, and
         * total == 1 */
        int splat_size = T - DT;
        if(splat_size > 0) {
          ary = Array::create(state, splat_size);

          for(int i = 0, n = M + O;
              i < splat_size;
              i++, n++)
          {
            ary->set(state, i, args.get_argument(n));
          }
        } else {
          ary = Array::create(state, 0);
        }

        scope->set_local(mcode->splat_position, ary);
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
      if(mcode->call_count >= state->shared().config.jit_call_til_compile) {
        LLVMState* ls = LLVMState::get(state);

        GCTokenImpl gct;
        OnStack<1> os(state, env);
        ls->compile_soon(state, gct, env->compiled_code(), previous,
                         invocation.self->direct_class(state), env, true);

      } else {
        mcode->call_count++;
      }
    }
#endif

    StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);

    Module* mod = invocation.module;
    if(!mod) mod = env->module();
    scope->initialize(invocation.self, env->top_scope_->block(),
                      mod, mcode->number_of_locals);
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

    // TODO: this is a quick hack to process block arguments in 1.9.
    if(!LANGUAGE_18_ENABLED) {
      if(!GenericArguments::call(state, frame, mcode, scope, args, invocation.flags)) {
        return NULL;
      }
    }

#ifdef RBX_PROFILER
    if(unlikely(state->vm()->tooling())) {
      Module* mod = scope->module();
      if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
        if(Module* ma = try_as<Module>(sc->attached_instance())) {
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
