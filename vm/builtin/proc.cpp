#include "arguments.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/exception.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/proc.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "dispatch.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "ontology.hpp"
#include "on_stack.hpp"
#include "version.h"

namespace rubinius {

  void Proc::init(STATE) {
    GO(proc).set(ontology::new_class(state, "Proc", G(object)));
    G(proc)->set_object_type(state, ProcType);
  }

  Proc* Proc::create(STATE, Object* self) {
    return state->new_object<Proc>(as<Class>(self));
  }

  Proc* Proc::from_env(STATE, Object* self, Object* env) {
    if(Proc* p = try_as<Proc>(env)) {
      return p;
    }

    if(BlockEnvironment* be = try_as<BlockEnvironment>(env)) {
      Proc* proc = Proc::create(state, self);
      proc->block(state, be);
      be->lock_scope(state);
      return proc;
    }

    return NULL;
  }

  Proc* Proc::from_env_prim(STATE, Object* self, Object* env) {
    if(Proc* p = from_env(state, self, env)) {
      return p;
    } else {
      return static_cast<Proc*>(Primitives::failure());
    }
  }

  Object* Proc::call(STATE, CallFrame* call_frame, Arguments& args) {
    bool lambda_style = CBOOL(lambda_);
    int flags = 0;

    Proc* self = this;
    OnStack<1> os(state, self);
    // Check the arity in lambda mode
    if(lambda_style && !block_->nil_p()) {
      flags = CallFrame::cIsLambda;
      int total = self->block_->compiled_code()->total_args()->to_native();
      int required = self->block_->compiled_code()->required_args()->to_native();

      bool arity_ok = false;
      if(Fixnum* fix = try_as<Fixnum>(self->block_->compiled_code()->splat())) {
        switch(fix->to_native()) {
        case -2:
          arity_ok = true;
          break;
        case -4:
          // splat = -4 means { |(a, b)| }
          if(args.total() == 1) {
            Array* ary = 0;
            Object* obj = args.get_argument(0);

            if(!(ary = try_as<Array>(obj))) {
              if(CBOOL(obj->respond_to(state, G(sym_to_ary), cFalse))) {
                obj = obj->send(state, call_frame, G(sym_to_ary));
                if(!(ary = try_as<Array>(obj))) {
                  Exception::type_error(state, "to_ary must return an Array", call_frame);
                  return 0;
                }
              }
            }

            if(ary) args.use_argument(ary);
          }
          // fall through for arity check
        case -3:
          // splat = -3 is used to distinguish { |a, | } from { |a| }
          if(args.total() == (size_t)required) arity_ok = true;
          break;
        default:
          if(args.total() >= (size_t)required) {
            arity_ok = true;
          }
        }

      /* For blocks taking one argument { |a|  }, in 1.8, there is a warning
       * issued but no exception raised when less than or more than one
       * argument is passed. If more than one is passed, 'a' receives an Array
       * of all the arguments.
       */
      } else if(required == 1 && LANGUAGE_18_ENABLED) {
        arity_ok = true;
      } else {
        arity_ok = args.total() <= (size_t)total &&
                   args.total() >= (size_t)required;
      }

      if(!arity_ok) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(),
              block_->compiled_code()->name());
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);
        return NULL;
      }
    }

    if(self->bound_method_->nil_p()) {
      if(self->block_->nil_p()) {
        Dispatch dis(state->symbol("__yield__"));
        return dis.send(state, call_frame, args);
      } else {
        return self->block_->call(state, call_frame, args, flags);
      }
    } else if(NativeMethod* nm = try_as<NativeMethod>(self->bound_method_)) {
      return nm->execute(state, call_frame, nm, G(object), args);
    } else if(NativeFunction* nf = try_as<NativeFunction>(self->bound_method_)) {
      return nf->call(state, args, call_frame);
    } else {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, self->bound_method_, "NativeMethod nor NativeFunction bound to proc");
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }
  }

  Object* Proc::yield(STATE, CallFrame* call_frame, Arguments& args) {
    if(bound_method_->nil_p()) {
      if(block_->nil_p()) {
        return call(state, call_frame, args);
      } else {
        // NOTE! To match MRI semantics, this explicitely ignores lambda_.
        return block_->call(state, call_frame, args, 0);
      }
    } else if(NativeMethod* nm = try_as<NativeMethod>(bound_method_)) {
      return nm->execute(state, call_frame, nm, G(object), args);
    } else if(NativeFunction* nf = try_as<NativeFunction>(bound_method_)) {
      return nf->call(state, args, call_frame);
    } else {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, bound_method_, "NativeMethod nor NativeFunction bound to proc");
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }
  }

  Object* Proc::call_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                          Arguments& args) {
    return call(state, call_frame, args);
  }

  Object* Proc::call_on_object(STATE, CallFrame* call_frame,
                               Executable* exec, Module* mod, Arguments& args) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Since we allow Proc's to be created without setting block_, we need to check
    // it.
    if(block_->nil_p()) {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, block_, "Invalid proc style");
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->compiled_code()->required_args()->to_native();

      if(args.total() < 1 || (required >= 0 && (size_t)required != args.total() - 1)) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(),
              block_->compiled_code()->name());
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);
        return NULL;
      }
    }

    return block_->call_on_object(state, call_frame, args, flags);
  }
}
