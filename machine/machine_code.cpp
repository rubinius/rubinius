#include "config.h"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "memory.hpp"
#include "defines.hpp"
#include "machine_code.hpp"
#include "interpreter.hpp"
#include "logger.hpp"

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/compiled_code.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/iseq.hpp"
#include "class/jit.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"
#include "class/class.hpp"
#include "class/location.hpp"
#include "class/constant_cache.hpp"
#include "class/call_site.hpp"
#include "class/unwind_site.hpp"
#include "class/unwind_state.hpp"

#include "diagnostics/measurement.hpp"
#include "diagnostics/timing.hpp"

#include "instructions.hpp"

#include "raise_reason.hpp"
#include "on_stack.hpp"

#include "configuration.hpp"
#include "dtrace/dtrace.h"

#include <sstream>

#ifdef RBX_WINDOWS
#include <malloc.h>
#endif

/*
 * An internalization of a CompiledCode which holds the instructions for the
 * method.
 */
namespace rubinius {
  std::atomic<uint64_t> MachineCode::code_serial;

  void MachineCode::bootstrap(STATE) {
    code_serial = 0;
  }

  MachineCode* MachineCode::create(STATE, CompiledCode* code) {
    return new MachineCode(state, code);
  }

  /*
   * Turns a CompiledCode's InstructionSequence into a C array of opcodes.
   */
  MachineCode::MachineCode(STATE, CompiledCode* code)
    : run((InterpreterRunner)Interpreter::execute)
    , total(code->iseq()->opcodes()->num_fields())
    , type(NULL)
    , keywords(!code->keywords()->nil_p())
    , total_args(code->total_args()->to_native())
    , required_args(code->required_args()->to_native())
    , post_args(code->post_args()->to_native())
    , splat_position(-1)
    , keywords_count(0)
    , stack_size(code->stack_size()->to_native())
    , number_of_locals(code->number_of_locals())
    , registers(code->registers()->to_native())
    , sample_count(0)
    , call_count(0)
    , uncommon_count(0)
    , _call_site_count_(0)
    , _constant_cache_count_(0)
    , _references_count_(0)
    , _references_(nullptr)
    , _serial_(MachineCode::get_serial())
    , _nil_id_(code->nil_id(state))
    , _name_(code->name()->cpp_str(state))
    , _location_()
    , unspecialized(nullptr)
    , fallback(nullptr)
    , execute_status_(eInterpret)
    , debugging(false)
    , flags(0)
  {
    if(keywords) {
      keywords_count = code->keywords()->num_fields() / 2;
    }

    std::ostringstream loc;
    loc << code->file()->cpp_str(state) << ":" << code->start_line();

    _location_.assign(loc.str());

    opcodes = new opcode[total];

    Interpreter::prepare(state, code, this);

    if(Fixnum* pos = try_as<Fixnum>(code->splat())) {
      splat_position = pos->to_native();
    }

    state->memory()->add_code_resource(state, this);
  }

  MachineCode::~MachineCode() {
#ifdef RBX_GC_DEBUG
    memset(opcodes, 0xFF, total * sizeof(opcode));
#endif
    delete[] opcodes;

    if(references()) {
#ifdef RBX_GC_DEBUG
      memset(references(), 0xFF, references_count() * sizeof(size_t));
#endif
      delete[] references();
    }
  }

  void MachineCode::finalize(STATE) {
    for(size_t i = 0; i < references_count(); i++) {
      if(size_t ip = references()[i]) {
        if(CallSite* call_site = try_as<CallSite>(
              reinterpret_cast<Object*>(opcodes[ip])))
        {
          call_site->finalize(state);
        } else if(UnwindSite* unwind_site = try_as<UnwindSite>(
              reinterpret_cast<Object*>(opcodes[ip])))
        {
          unwind_site->finalize(state);
        }
      }
    }
  }

  int MachineCode::size() {
    return sizeof(MachineCode) +
      (total * sizeof(opcode)) + // opcodes
      (total * sizeof(void*));
  }

  CallSite* MachineCode::call_site(STATE, int ip) {
    Object* obj = reinterpret_cast<Object*>(opcodes[ip + 1]);
    return as<CallSite>(obj);
  }

  ConstantCache* MachineCode::constant_cache(STATE, int ip) {
    Object* obj = reinterpret_cast<Object*>(opcodes[ip + 1]);
    return as<ConstantCache>(obj);
  }

  Tuple* MachineCode::call_sites(STATE) {
    size_t count = call_site_count();
    Tuple* sites = Tuple::create(state, count);

    for(size_t i = 0, j = 0;
        i < count && j < references_count();
        j++)
    {
      if(CallSite* site =
          try_as<CallSite>(reinterpret_cast<Object*>(opcodes[references()[j]])))
      {
        sites->put(state, i++, site);
      }
    }

    return sites;
  }

  Tuple* MachineCode::constant_caches(STATE) {
    size_t count = constant_cache_count();
    Tuple* caches = Tuple::create(state, count);

    for(size_t i = 0, j = 0;
        i < count && j < references_count();
        j++)
    {
      if(ConstantCache* cache =
          try_as<ConstantCache>(reinterpret_cast<Object*>(opcodes[references()[j]])))
      {
        caches->put(state, i++, cache);
      }
    }

    return caches;
  }

  void MachineCode::store_call_site(STATE, CompiledCode* code, int ip, CallSite* call_site) {
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(call_site);
    code->write_barrier(state, call_site);
    std::atomic_thread_fence(std::memory_order_acq_rel);
  }

  void MachineCode::store_constant_cache(STATE, CompiledCode* code, int ip, ConstantCache* constant_cache) {
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(constant_cache);
    code->write_barrier(state, constant_cache);
    std::atomic_thread_fence(std::memory_order_acq_rel);
  }

  void MachineCode::store_unwind_site(STATE, CompiledCode* code, int ip, UnwindSite* unwind_site) {
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(unwind_site);
    code->write_barrier(state, unwind_site);
    std::atomic_thread_fence(std::memory_order_acq_rel);
  }

  void MachineCode::store_measurement(STATE,
      CompiledCode* code, int ip, diagnostics::Measurement* m)
  {
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(m);
    std::atomic_thread_fence(std::memory_order_acq_rel);
  }

  // Argument handler implementations

  // For when the method expects no arguments at all (no splat, nothing)
  class NoArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      return args.total() == 0;
    }
  };

  // For when the method expects 1 and only 1 argument
  class OneArgument {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if(args.total() != 1) return false;
      scope->set_local(0, args.get_argument(0));
      return true;
    }
  };

  // For when the method expects 2 and only 2 arguments
  class TwoArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if(args.total() != 2) return false;
      scope->set_local(0, args.get_argument(0));
      scope->set_local(1, args.get_argument(1));
      return true;
    }
  };

  // For when the method expects 3 and only 3 arguments
  class ThreeArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if(args.total() != 3) return false;
      scope->set_local(0, args.get_argument(0));
      scope->set_local(1, args.get_argument(1));
      scope->set_local(2, args.get_argument(2));
      return true;
    }
  };

  // For when the method expects a fixed number of arguments (no splat)
  class FixedArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if((intptr_t)args.total() != mcode->total_args) return false;

      for(intptr_t i = 0; i < mcode->total_args; i++) {
        scope->set_local(i, args.get_argument(i));
      }

      return true;
    }
  };

  // For when a method takes all arguments as a splat
  class SplatOnlyArgument {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      const size_t total = args.total();
      Array* ary = Array::create(state, total);

      for(size_t i = 0; i < total; i++) {
        ary->set(state, i, args.get_argument(i));
      }

      scope->set_local(mcode->splat_position, ary);
      return true;
    }
  };

  // The fallback, can handle all cases
  class GenericArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
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
       *
       */

      const intptr_t N = args.total();
      const intptr_t T = mcode->total_args;
      const intptr_t M = mcode->required_args;
      const intptr_t O = T - M - (mcode->keywords ? 1 : 0);

      /* TODO: Clean up usage to uniformly refer to 'splat' as N arguments
       * passed from sender at a single position and 'rest' as N arguments
       * collected into a single argument at the receiver.
       */
      const intptr_t RI = mcode->splat_position;
      const bool RP = (RI >= 0);

      // expecting 0, got 0.
      if(T == 0 && N == 0) {
        if(RP) {
          scope->set_local(mcode->splat_position, Array::create(state, 0));
        }

        return true;
      }

      // Too few args!
      if(N < M) return false;

      // Too many args (no rest argument!)
      if(!RP && N > T) return false;

      const intptr_t P  = mcode->post_args;
      const intptr_t H  = M - P;

      Object* kw = 0;
      Object* kw_remainder = 0;
      bool KP = false;
      bool KA = false;

      if(mcode->keywords && N > M) {
        Object* obj = args.get_argument(N - 1);
        Object* arguments[2] = { obj, RBOOL(O > 0 || RP) };

        Arguments args(G(sym_keyword_object), G(runtime), 2, arguments);
        Dispatch dispatch(G(sym_keyword_object));

        if(Object* kw_result = dispatch.send(state, args)) {
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

      const intptr_t K = (KP && !KA && N > M) ? 1 : 0;
      const intptr_t E = N - M - K;

      // Too many arguments
      if(mcode->keywords && !RP && !KP && E > O) return false;

      intptr_t X;

      const intptr_t ON = (X = MIN(O, E)) > 0 ? X : 0;
      const intptr_t RN = (RP && (X = E - ON) > 0) ? X : 0;
      const intptr_t PI = H + O + (RP ? 1 : 0);
      const intptr_t KI = RP ? T : T - 1;

      intptr_t a = 0;   // argument index
      intptr_t l = 0;   // local index

      // head arguments
      if(H > 0) {
        for(; a < H; l++, a++) {
          scope->set_local(l, args.get_argument(a));
        }
      }

      // optional arguments
      if(O > 0) {
        for(; l < H + O && a < H + ON; l++, a++) {
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
        for(l = PI; l < PI + P && a < N - K; l++, a++) {
          scope->set_local(l, args.get_argument(a));
        }
      }

      // keywords
      if(kw) {
        scope->set_local(KI, kw);
      }

      return true;
    }
  };

  /*
   * Looks at the opcodes for this method and optimizes instance variable
   * access by using special byte codes.
   *
   * For push_ivar, uses push_my_field when the instance variable has an
   * index assigned.  Same for set_ivar/store_my_field.
   */

  void MachineCode::specialize(STATE, CompiledCode* original, TypeInfo* ti) {
    type = ti;
    for(size_t width, i = 0; i < total; i += width) {
      Tuple* ops = original->iseq()->opcodes();

      opcode op = as<Fixnum>(ops->at(i))->to_native();
      width = Instructions::instruction_data(op).width;

      if(op == instructions::data_push_ivar.id) {
        intptr_t sym = as<Symbol>(reinterpret_cast<Object*>(opcodes[i + 1]))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = reinterpret_cast<intptr_t>(
              instructions::data_push_my_offset.interpreter_address);
          opcodes[i + 1] = ti->slot_locations[it->second];
        }
      } else if(op == instructions::data_set_ivar.id) {
        intptr_t sym = as<Symbol>(reinterpret_cast<Object*>(opcodes[i + 1]))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = reinterpret_cast<intptr_t>(
              instructions::data_store_my_field.interpreter_address);
          opcodes[i + 1] = it->second;
        }
      }
    }
  }

  void MachineCode::setup_argument_handler() {
    // Firstly, use the generic case that handles all cases
    fallback = &MachineCode::execute_specialized<GenericArguments>;

    // If there are no optionals, only a fixed number of positional arguments.
    if(total_args == required_args) {
      // if no arguments are expected
      if(total_args == 0) {
        // and there is no splat, use the fastest case.
        if(splat_position == -1) {
          fallback = &MachineCode::execute_specialized<NoArguments>;

        // otherwise use the splat only case.
        } else if(!keywords) {
          fallback = &MachineCode::execute_specialized<SplatOnlyArgument>;
        }
      // Otherwise use the few specialized cases iff there is no splat
      } else if(splat_position == -1) {
        switch(total_args) {
        case 1:
          fallback = &MachineCode::execute_specialized<OneArgument>;
          break;
        case 2:
          fallback = &MachineCode::execute_specialized<TwoArguments>;
          break;
        case 3:
          fallback = &MachineCode::execute_specialized<ThreeArguments>;
          break;
        default:
          fallback = &MachineCode::execute_specialized<FixedArguments>;
          break;
        }
      }
    }
  }

  /* This is the execute implementation used by normal Ruby code,
   * as opposed to Primitives or FFI functions.
   * It prepares a Ruby method for execution.
   * Here, +exec+ is a MachineCode instance accessed via the +machine_code+ slot on
   * CompiledCode.
   *
   * This method works as a template even though it's here because it's never
   * called from outside of this file. Thus all the template expansions are done.
   */
  template <typename ArgumentHandler>
    Object* MachineCode::execute_specialized(STATE,
        Executable* exec, Module* mod, Arguments& args)
    {
      CompiledCode* code = as<CompiledCode>(exec);
      MachineCode* mcode = code->machine_code();

      StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);
      // Originally, I tried using msg.module directly, but what happens is if
      // super is used, that field is read. If you combine that with the method
      // being called recursively, msg.module can change, causing super() to
      // look in the wrong place.
      //
      // Thus, we have to cache the value in the StackVariables.
      scope->initialize(args.recv(), args.name(), args.block(), mod, mcode->number_of_locals);

      // If argument handling fails..
      if(ArgumentHandler::call(state, mcode, scope, args) == false) {
        if(state->unwind_state()->raise_reason() == cNone) {
          Exception* exc =
            Exception::make_argument_error(state, mcode->total_args,
                args.total(), args.name()->cpp_str(state).c_str());
          exc->locations(state, Location::from_call_stack(state));
          state->raise_exception(exc);
        }

        return NULL;
      }

      uintptr_t* mem = ALLOCA_CALL_FRAME(mcode->stack_size + mcode->registers);
      CallFrame* call_frame = new(mem) CallFrame();

      call_frame->prepare(mcode->stack_size);

      call_frame->lexical_scope_ = code->scope();
      call_frame->dispatch_data = nullptr;
      call_frame->compiled_code = code;
      call_frame->flags = 0;
      call_frame->top_scope_ = nullptr;
      call_frame->scope = scope;
      call_frame->arguments = &args;
      call_frame->unwind = nullptr;

      if(!state->push_call_frame(state, call_frame)) {
        return NULL;
      }

      mcode->call_count++;

      Object* value = 0;

      RUBINIUS_METHOD_ENTRY_HOOK(state, scope->module(), args.name());
      value = (*mcode->run)(state, mcode);
      RUBINIUS_METHOD_RETURN_HOOK(state, scope->module(), args.name());

      if(!state->pop_call_frame(state, call_frame->previous)) {
        return NULL;
      }

      return value;
    }

  /** This is used as a fallback way of entering the interpreter */
  Object* MachineCode::execute(STATE, Executable* exec, Module* mod, Arguments& args) {
    return execute_specialized<GenericArguments>(state, exec, mod, args);
  }

  Object* MachineCode::execute_as_script(STATE, CompiledCode* code) {
    MachineCode* mcode = code->machine_code();

    Symbol* name = state->symbol("__script__");

    StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);
    // Originally, I tried using msg.module directly, but what happens is if
    // super is used, that field is read. If you combine that with the method
    // being called recursively, msg.module can change, causing super() to
    // look in the wrong place.
    //
    // Thus, we have to cache the value in the StackVariables.
    scope->initialize(G(main), name, cNil, G(object), mcode->number_of_locals);

    uintptr_t* mem = ALLOCA_CALL_FRAME(mcode->stack_size + mcode->registers);
    CallFrame* call_frame = new(mem) CallFrame();

    call_frame->prepare(mcode->stack_size);

    Arguments args(name, G(main), cNil, 0, 0);

    call_frame->lexical_scope_ = code->scope();
    call_frame->dispatch_data = nullptr;
    call_frame->compiled_code = code;
    call_frame->flags = CallFrame::cScript | CallFrame::cTopLevelVisibility;
    call_frame->top_scope_ = nullptr;
    call_frame->scope = scope;
    call_frame->arguments = &args;
    call_frame->unwind = nullptr;

    if(!state->push_call_frame(state, call_frame)) {
      return NULL;
    }

    state->checkpoint(state);

    // Don't generate profiling info here, it's expected
    // to be done by the caller.

    Object* value = (*mcode->run)(state, mcode);

    if(!state->pop_call_frame(state, call_frame->previous)) {
      return NULL;
    }

    return value;
  }
}
