#include "arguments.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "dispatch.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "primitives.hpp"

#include "class/block_environment.hpp"
#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/native_method.hpp"
#include "class/native_function.hpp"
#include "class/proc.hpp"

namespace rubinius {

  void Proc::bootstrap(STATE) {
    GO(proc).set(state->memory()->new_class<Class, Proc>(state, "Proc"));
  }

  Proc* Proc::create(STATE, Object* self) {
    return state->memory()->new_object<Proc>(state, as<Class>(self));
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

  Object* Proc::call(STATE, Arguments& args) {
    bool lambda_style = CBOOL(lambda());
    int flags = 0;

    Proc* self = this;
    OnStack<1> os(state, self);
    // Check the arity in lambda mode
    if(lambda_style && !block()->nil_p()) {
      flags = CallFrame::cIsLambda;
      int total = self->block()->compiled_code()->total_args()->to_native();
      int required = self->block()->compiled_code()->required_args()->to_native();

      bool arity_ok = false;
      if(Fixnum* fix = try_as<Fixnum>(self->block()->compiled_code()->splat())) {
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
                if(!(ary = try_as<Array>(obj->send(state, G(sym_to_ary))))) {
                  Exception::type_error(state, "to_ary must return an Array");
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

      } else {
        arity_ok = args.total() <= (size_t)total &&
                   args.total() >= (size_t)required;
      }

      if(!arity_ok) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(),
              block()->compiled_code()->name()->cpp_str(state).c_str());
        exc->locations(state, Location::from_call_stack(state));
        state->raise_exception(exc);
        return NULL;
      }
    }

    if(self->bound_method()->nil_p()) {
      if(self->block()->nil_p()) {
        Dispatch dispatch(state->symbol("__yield__"));
        return dispatch.send(state, args);
      } else {
        return self->block()->call(state, args, flags);
      }
    } else if(NativeMethod* nm = try_as<NativeMethod>(self->bound_method())) {
      return nm->execute(state, nm, G(object), args);
    } else if(NativeFunction* nf = try_as<NativeFunction>(self->bound_method())) {
      return nf->call(state, args);
    } else {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, self->bound_method(), "NativeMethod nor NativeFunction bound to proc");
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);
      return NULL;
    }
  }

  Object* Proc::yield(STATE, Arguments& args) {
    if(bound_method()->nil_p()) {
      if(block()->nil_p()) {
        return call(state, args);
      } else {
        int flags = CBOOL(lambda()) ? CallFrame::cIsLambda : 0;
        return block()->call(state, args, flags);
      }
    } else if(NativeMethod* nm = try_as<NativeMethod>(bound_method())) {
      return nm->execute(state, nm, G(object), args);
    } else if(NativeFunction* nf = try_as<NativeFunction>(bound_method())) {
      return nf->call(state, args);
    } else {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, bound_method(), "NativeMethod nor NativeFunction bound to proc");
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);
      return NULL;
    }
  }

  Object* Proc::call_prim(STATE, Executable* exec, Module* mod,
                          Arguments& args) {
    return call(state, args);
  }

  Object* Proc::call_on_object(STATE,
                               Executable* exec, Module* mod, Arguments& args) {
    bool lambda_style = !lambda()->nil_p();
    int flags = 0;

    // Since we allow Proc's to be created without setting block_, we need to check
    // it.
    if(block()->nil_p()) {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, block(), "Invalid proc style");
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);
      return NULL;
    }

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block()->compiled_code()->required_args()->to_native();

      if(args.total() < 1 || (required >= 0 && (size_t)required != args.total() - 1)) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(),
              block()->compiled_code()->name()->cpp_str(state).c_str());
        exc->locations(state, Location::from_call_stack(state));
        state->raise_exception(exc);
        return NULL;
      }
    }

    return block()->call_on_object(state, args, flags);
  }
}
