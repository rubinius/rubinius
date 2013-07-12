#include "config.h"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "objectmemory.hpp"
#include "prelude.hpp"
#include "machine_code.hpp"

#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "builtin/location.hpp"
#include "builtin/constant_cache.hpp"
#include "builtin/call_site.hpp"

#include "instructions.hpp"

#include "instruments/tooling.hpp"
#include "instruments/timing.hpp"

#include "raise_reason.hpp"
#include "on_stack.hpp"

#include "configuration.hpp"
#include "dtrace/dtrace.h"

#ifdef RBX_WINDOWS
#include <malloc.h>
#endif

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

/*
 * An internalization of a CompiledCode which holds the instructions for the
 * method.
 */
namespace rubinius {

  void MachineCode::init(STATE) {
  }

  /*
   * Turns a CompiledCode's InstructionSequence into a C array of opcodes.
   */
  MachineCode::MachineCode(STATE, CompiledCode* meth)
    : type(NULL)
    , uncommon_count(0)
    , number_of_call_sites_(0)
    , call_site_offsets_(0)
    , number_of_constant_caches_(0)
    , constant_cache_offsets_(0)
    , execute_status_(eInterpret)
    , name_(meth->name())
    , method_id_(state->shared().inc_method_count(state))
    , debugging(false)
    , flags(0)
  {
    if(state->shared().tool_broker()->tooling_interpreter_p()) {
      run = MachineCode::tooling_interpreter;
    } else {
      run = MachineCode::interpreter;
    }

    total = meth->iseq()->opcodes()->num_fields();

    opcodes = new opcode[total];

    fill_opcodes(state, meth);
    stack_size = meth->stack_size()->to_native();
    number_of_locals = meth->number_of_locals();

    total_args = meth->total_args()->to_native();
    required_args = meth->required_args()->to_native();
    post_args = meth->post_args()->to_native();

    if(Fixnum* pos = try_as<Fixnum>(meth->splat())) {
      splat_position = pos->to_native();
    } else {
      splat_position = -1;
    }

    // Disable JIT for large methods
    if(!state->shared().config.jit_disabled &&
        total < (size_t)state->shared().config.jit_max_method_size) {
      call_count = 0;
    } else {
      call_count = -1;
    }

    unspecialized = 0;
    fallback = 0;

    for(int i = 0; i < cMaxSpecializations; i++) {
      specializations[i].class_data.raw = 0;
      specializations[i].execute = 0;
      specializations[i].jit_data = 0;
    }

    state->shared().om->add_code_resource(this);
  }

  MachineCode::~MachineCode() {
#ifdef RBX_GC_DEBUG
    memset(opcodes, 0xFF, total * sizeof(opcode));
#endif
    delete[] opcodes;

    if(call_site_offsets_) {
#ifdef RBX_GC_DEBUG
      memset(call_site_offsets_, 0xFF, number_of_call_sites_ * sizeof(size_t));
#endif
      delete[] call_site_offsets_;
    }
    if(constant_cache_offsets_) {
#ifdef RBX_GC_DEBUG
      memset(constant_cache_offsets_, 0xFF, number_of_constant_caches_ * sizeof(size_t));
#endif
      delete[] constant_cache_offsets_;
    }
  }

  int MachineCode::size() {
    return sizeof(MachineCode) +
      (total * sizeof(opcode)) + // opcodes
      (total * sizeof(void*));
  }

  void MachineCode::fill_opcodes(STATE, CompiledCode* original) {
    Tuple* ops = original->iseq()->opcodes();

    int sends = 0;
    int constants = 0;

    for(size_t index = 0; index < total;) {
      Object* val = ops->at(state, index);
      if(val->nil_p()) {
        opcodes[index++] = 0;
      } else {
        opcodes[index] = as<Fixnum>(val)->to_native();

        size_t width = InstructionSequence::instruction_width(opcodes[index]);

        switch(width) {
        case 2:
          opcodes[index + 1] = as<Fixnum>(ops->at(state, index + 1))->to_native();
          break;
        case 3:
          opcodes[index + 1] = as<Fixnum>(ops->at(state, index + 1))->to_native();
          opcodes[index + 2] = as<Fixnum>(ops->at(state, index + 2))->to_native();
          break;
        }

        switch(opcodes[index]) {
        case InstructionSequence::insn_send_method:
        case InstructionSequence::insn_send_stack:
        case InstructionSequence::insn_send_stack_with_block:
        case InstructionSequence::insn_send_stack_with_splat:
        case InstructionSequence::insn_send_super_stack_with_block:
        case InstructionSequence::insn_send_super_stack_with_splat:
        case InstructionSequence::insn_zsuper:
        case InstructionSequence::insn_meta_send_call:
        case InstructionSequence::insn_meta_send_op_plus:
        case InstructionSequence::insn_meta_send_op_minus:
        case InstructionSequence::insn_meta_send_op_equal:
        case InstructionSequence::insn_meta_send_op_tequal:
        case InstructionSequence::insn_meta_send_op_lt:
        case InstructionSequence::insn_meta_send_op_gt:
        case InstructionSequence::insn_meta_to_s:
        case InstructionSequence::insn_check_serial:
        case InstructionSequence::insn_check_serial_private:
        case InstructionSequence::insn_call_custom:
          sends++;
          break;
        case InstructionSequence::insn_push_const_fast:
        case InstructionSequence::insn_find_const_fast:
          constants++;
          break;
        }

        index += width;
      }
    }

    initialize_call_sites(state, original, sends);
    initialize_constant_caches(state, original, constants);
  }

  void MachineCode::initialize_call_sites(STATE, CompiledCode* original, int sends) {
    number_of_call_sites_ = sends;
    call_site_offsets_ = new size_t[sends];

    int which = 0;
    bool allow_private = false;
    bool is_super = false;
    int inline_index = 0;

    for(size_t ip = 0; ip < total;) {
      opcode op = opcodes[ip];
      switch(op) {
      case InstructionSequence::insn_invoke_primitive: {
        Symbol* name = as<Symbol>(original->literals()->at(opcodes[ip + 1]));

        InvokePrimitive invoker = Primitives::get_invoke_stub(state, name);
        opcodes[ip + 1] = reinterpret_cast<intptr_t>(invoker);
        break;
      }
      case InstructionSequence::insn_allow_private:
        allow_private = true;
        break;
      case InstructionSequence::insn_send_super_stack_with_block:
      case InstructionSequence::insn_send_super_stack_with_splat:
      case InstructionSequence::insn_zsuper:
        is_super = true;
        // fall through
      case InstructionSequence::insn_check_serial:
      case InstructionSequence::insn_check_serial_private:
      case InstructionSequence::insn_call_custom:
      case InstructionSequence::insn_send_method:
      case InstructionSequence::insn_send_stack:
      case InstructionSequence::insn_send_stack_with_block:
      case InstructionSequence::insn_send_stack_with_splat:
      case InstructionSequence::insn_meta_send_call:
      case InstructionSequence::insn_meta_send_op_plus:
      case InstructionSequence::insn_meta_send_op_minus:
      case InstructionSequence::insn_meta_send_op_equal:
      case InstructionSequence::insn_meta_send_op_tequal:
      case InstructionSequence::insn_meta_send_op_lt:
      case InstructionSequence::insn_meta_send_op_gt:
      case InstructionSequence::insn_meta_to_s:
        {
        assert(which < sends);

        Symbol* name = try_as<Symbol>(original->literals()->at(opcodes[ip + 1]));
        if(!name) name = nil<Symbol>();

        CallSite* call_site = CallSite::empty(state, name, original, ip);

        call_site_offsets_[inline_index] = ip;
        inline_index++;

        if(op == InstructionSequence::insn_call_custom) {
          call_site->set_call_custom();
        } else {
          if(allow_private) call_site->set_is_private();
          if(is_super) call_site->set_is_super();

          if(op == InstructionSequence::insn_send_method) {
            call_site->set_is_vcall();
          }
        }

        store_call_site(state, original, ip, call_site);
        is_super = false;
        allow_private = false;
      }
      }

      ip += InstructionSequence::instruction_width(op);
    }
  }

  void MachineCode::initialize_constant_caches(STATE, CompiledCode* original, int constants) {
    number_of_constant_caches_ = constants;
    constant_cache_offsets_ = new size_t[constants];

    int constant_index = 0;
    for(size_t ip = 0; ip < total;) {
      opcode op = opcodes[ip];
      switch(op) {
      case InstructionSequence::insn_push_const_fast:
      case InstructionSequence::insn_find_const_fast: {
        Symbol* name = as<Symbol>(original->literals()->at(opcodes[ip + 1]));

        ConstantCache* cache = ConstantCache::empty(state, name, original, ip);
        constant_cache_offsets_[constant_index] = ip;
        constant_index++;
        store_constant_cache(state, original, ip, cache);
        break;
      }
      }

      ip += InstructionSequence::instruction_width(op);
    }

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
    Tuple* sites = Tuple::create(state, number_of_call_sites_);
    for(size_t i = 0; i < number_of_call_sites_; ++i) {
      sites->put(state, i, call_site(state, call_site_offsets_[i]));
    }
    return sites;
  }

  Tuple* MachineCode::constant_caches(STATE) {
    Tuple* caches = Tuple::create(state, number_of_constant_caches_);
    for(size_t i = 0; i < number_of_constant_caches_; ++i) {
      caches->put(state, i, constant_cache(state, constant_cache_offsets_[i]));
    }
    return caches;
  }

  void MachineCode::store_call_site(STATE, CompiledCode* code, int ip, CallSite* call_site) {
    atomic::memory_barrier();
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(call_site);
    code->write_barrier(state, call_site);
  }

  void MachineCode::store_constant_cache(STATE, CompiledCode* code, int ip, ConstantCache* constant_cache) {
    atomic::memory_barrier();
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(constant_cache);
    code->write_barrier(state, constant_cache);
  }

  // Argument handler implementations

  // For when the method expects no arguments at all (no splat, nothing)
  class NoArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
      return args.total() == 0;
    }
  };

  // For when the method expects 1 and only 1 argument
  class OneArgument {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
      if(args.total() != 1) return false;
      scope->set_local(0, args.get_argument(0));
      return true;
    }
  };

  // For when the method expects 2 and only 2 arguments
  class TwoArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
      if(args.total() != 2) return false;
      scope->set_local(0, args.get_argument(0));
      scope->set_local(1, args.get_argument(1));
      return true;
    }
  };

  // For when the method expects 3 and only 3 arguments
  class ThreeArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
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
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
      if((native_int)args.total() != mcode->total_args) return false;

      for(native_int i = 0; i < mcode->total_args; i++) {
        scope->set_local(i, args.get_argument(i));
      }

      return true;
    }
  };

  // For when a method takes all arguments as a splat
  class SplatOnlyArgument {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
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
      const bool has_splat = (mcode->splat_position >= 0);
      native_int total_args = args.total();

      // expecting 0, got 0.
      if(mcode->total_args == 0 && total_args == 0) {
        if(has_splat) {
          scope->set_local(mcode->splat_position, Array::create(state, 0));
        }

        return true;
      }

      // Too few args!
      if(total_args < mcode->required_args) return false;

      // Too many args (no splat!)
      if(!has_splat && total_args > mcode->total_args) return false;

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

      const native_int P = mcode->post_args;
      const native_int R = mcode->required_args;

      // M is for mandatory
      const native_int M = R - P;
      const native_int T = total_args;

      // DT is for declared total
      const native_int DT = mcode->total_args;
      const native_int O = DT - R;

      // HS is for has splat
      const native_int HS = has_splat ? 1 : 0;

      // Phase 1, mandatory args
      for(native_int i = 0; i < M; i++) {
        scope->set_local(i, args.get_argument(i));
      }

      // Phase 2, post args
      for(native_int i = T - P, l = M + O + HS;
          i < T;
          i++, l++)
      {
        scope->set_local(l, args.get_argument(i));
      }

      // Phase 3, optionals
      for(native_int i = M, limit = M + MIN(O, T-R);
          i < limit;
          i++)
      {
        scope->set_local(i, args.get_argument(i));
      }

      // Phase 4, splat
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

  /*
   * Looks at the opcodes for this method and optimizes instance variable
   * access by using special byte codes.
   *
   * For push_ivar, uses push_my_field when the instance variable has an
   * index assigned.  Same for set_ivar/store_my_field.
   */

  void MachineCode::specialize(STATE, CompiledCode* original, TypeInfo* ti) {
    type = ti;
    for(size_t i = 0; i < total;) {
      opcode op = opcodes[i];

      if(op == InstructionSequence::insn_push_ivar) {
        native_int idx = opcodes[i + 1];
        native_int sym = as<Symbol>(original->literals()->at(state, idx))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = InstructionSequence::insn_push_my_offset;
          opcodes[i + 1] = ti->slot_locations[it->second];
        }
      } else if(op == InstructionSequence::insn_set_ivar) {
        native_int idx = opcodes[i + 1];
        native_int sym = as<Symbol>(original->literals()->at(state, idx))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = InstructionSequence::insn_store_my_field;
          opcodes[i + 1] = it->second;
        }
      }

      i += InstructionSequence::instruction_width(op);
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
        } else {
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
    Object* MachineCode::execute_specialized(STATE, CallFrame* previous,
        Executable* exec, Module* mod, Arguments& args) {

      CompiledCode* code = as<CompiledCode>(exec);
      MachineCode* mcode = code->machine_code();

      StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);
      // Originally, I tried using msg.module directly, but what happens is if
      // super is used, that field is read. If you combine that with the method
      // being called recursively, msg.module can change, causing super() to
      // look in the wrong place.
      //
      // Thus, we have to cache the value in the StackVariables.
      scope->initialize(args.recv(), args.block(), mod, mcode->number_of_locals);

      InterpreterCallFrame* frame = ALLOCA_CALLFRAME(mcode->stack_size);

      // If argument handling fails..
      if(ArgumentHandler::call(state, mcode, scope, args) == false) {
        Exception* exc =
          Exception::make_argument_error(state, mcode->total_args, args.total(), args.name());
        exc->locations(state, Location::from_call_stack(state, previous));
        state->raise_exception(exc);

        return NULL;
      }

      frame->prepare(mcode->stack_size);

      frame->previous = previous;
      frame->constant_scope_ = code->scope();
      frame->dispatch_data = 0;
      frame->compiled_code = code;
      frame->flags = 0;
      frame->optional_jit_data = 0;
      frame->top_scope_ = 0;
      frame->scope = scope;
      frame->arguments = &args;

      GCTokenImpl gct;

#ifdef ENABLE_LLVM
      // A negative call_count means we've disabled usage based JIT
      // for this method.
      if(mcode->call_count >= 0) {
        if(mcode->call_count >= state->shared().config.jit_call_til_compile) {
          LLVMState* ls = LLVMState::get(state);
          OnStack<3> os(state, exec, mod, code);
          ls->compile_callframe(state, gct, code, frame);
        } else {
          mcode->call_count++;
        }
      }
#endif

#ifdef RBX_PROFILER
      if(unlikely(state->vm()->tooling())) {
        // Check the stack and interrupts here rather than in the interpreter
        // loop itself.
        OnStack<2> os(state, exec, code);
        if(!state->check_interrupts(gct, frame, frame)) return NULL;

        state->checkpoint(gct, frame);

        tooling::MethodEntry method(state, exec, scope->module(), args, code);

        RUBINIUS_METHOD_ENTRY_HOOK(state, scope->module(), args.name(), previous);
        Object* result = (*mcode->run)(state, mcode, frame);
        RUBINIUS_METHOD_RETURN_HOOK(state, scope->module(), args.name(), previous);
        return result;
      } else {
        if(!state->check_interrupts(gct, frame, frame)) return NULL;

        state->checkpoint(gct, frame);
        RUBINIUS_METHOD_ENTRY_HOOK(state, scope->module(), args.name(), previous);
        Object* result = (*mcode->run)(state, mcode, frame);
        RUBINIUS_METHOD_RETURN_HOOK(state, scope->module(), args.name(), previous);
        return result;
      }
#else
      if(!state->check_interrupts(gct, frame, frame)) return NULL;

      state->checkpoint(gct, frame);

      RUBINIUS_METHOD_ENTRY_HOOK(state, scope->module(), args.name(), previous);
      Object* result = (*mcode->run)(state, mcode, frame);
      RUBINIUS_METHOD_RETURN_HOOK(state, scope->module(), args.name(), previous);
      return result;
#endif
    }

  /** This is used as a fallback way of entering the interpreter */
  Object* MachineCode::execute(STATE, CallFrame* previous, Executable* exec, Module* mod, Arguments& args) {
    return execute_specialized<GenericArguments>(state, previous, exec, mod, args);
  }

  Object* MachineCode::execute_as_script(STATE, CompiledCode* code, CallFrame* previous) {
    MachineCode* mcode = code->machine_code();

    StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);
    // Originally, I tried using msg.module directly, but what happens is if
    // super is used, that field is read. If you combine that with the method
    // being called recursively, msg.module can change, causing super() to
    // look in the wrong place.
    //
    // Thus, we have to cache the value in the StackVariables.
    scope->initialize(G(main), cNil, G(object), mcode->number_of_locals);

    InterpreterCallFrame* frame = ALLOCA_CALLFRAME(mcode->stack_size);

    frame->prepare(mcode->stack_size);

    Arguments args(state->symbol("__script__"), G(main), cNil, 0, 0);

    frame->previous = previous;
    frame->constant_scope_ = code->scope();
    frame->dispatch_data = 0;
    frame->compiled_code = code;
    frame->flags = CallFrame::cScript | CallFrame::cTopLevelVisibility;
    frame->optional_jit_data = 0;
    frame->top_scope_ = 0;
    frame->scope = scope;
    frame->arguments = &args;

    // Do NOT check if we should JIT this. We NEVER want to jit a script.

    // Check the stack and interrupts here rather than in the interpreter
    // loop itself.

    GCTokenImpl gct;

    if(!state->check_interrupts(gct, frame, frame)) return NULL;

    state->checkpoint(gct, frame);

    // Don't generate profiling info here, it's expected
    // to be done by the caller.

    return (*mcode->run)(state, mcode, frame);
  }

  // If +disable+ is set, then the method is tagged as not being
  // available for JIT.
  void MachineCode::deoptimize(STATE, CompiledCode* original,
                            jit::RuntimeDataHolder* rd,
                            bool disable)
  {
#ifdef ENABLE_LLVM
    LLVMState* ls = LLVMState::get(state);
    ls->start_method_update();

    bool still_others = false;

    for(int i = 0; i < cMaxSpecializations; i++) {
      if(!rd) {
        specializations[i].class_data.raw = 0;
        specializations[i].execute = 0;
        specializations[i].jit_data = 0;
      } else if(specializations[i].jit_data == rd) {
        specializations[i].class_data.raw = 0;
        specializations[i].execute = 0;
        specializations[i].jit_data = 0;
      } else if(specializations[i].jit_data) {
        still_others = true;
      }
    }

    if(!rd || original->jit_data() == rd) {
      unspecialized = 0;
      original->set_jit_data(0);
    }

    if(original->jit_data()) still_others = true;

    if(!still_others) {
      execute_status_ = eInterpret;

      // This resets execute to use the interpreter
      original->set_executor(fallback);
    }

    if(disable) {
      execute_status_ = eJITDisable;
      original->set_executor(fallback);
    } else if(execute_status_ == eJITDisable && still_others) {
      execute_status_ = eJIT;
    }

    if(original->execute == CompiledCode::specialized_executor) {
      bool found = false;

      for(int i = 0; i < cMaxSpecializations; i++) {
        if(specializations[i].execute) found = true;
      }

      if(unspecialized) found = true;

      if(!found) rubinius::bug("no specializations!");
    }

    ls->end_method_update();
#endif
  }

  /*
   * Ensures the specified IP value is a valid address.
   */
  bool MachineCode::validate_ip(STATE, size_t ip) {
    /* Ensure ip is valid */
    MachineCode::Iterator iter(this);
    for(; !iter.end(); iter.inc()) {
      if(iter.position() >= ip) break;
    }
    return ip == iter.position();
  }
}
