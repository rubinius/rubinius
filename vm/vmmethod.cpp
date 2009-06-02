#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "objectmemory.hpp"
#include "prelude.hpp"
#include "vmmethod.hpp"

#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/system.hpp"
#include "builtin/machine_method.hpp"
#include "instructions.hpp"

#include "instruments/profiler.hpp"
#include "instruments/timing.hpp"
#include "config.h"

#include "raise_reason.hpp"

#include "configuration.hpp"

#ifdef ENABLE_LLVM
#include "llvm/jit.hpp"
#endif

#define USE_SPECIALIZED_EXECUTE

/*
 * An internalization of a CompiledMethod which holds the instructions for the
 * method.
 */
namespace rubinius {

  /** System-wide standard interpreter method pointer. */
  static InterpreterRunner standard_interpreter = 0;

  /** System-wide dynamic interpreter method pointer. */
  static InterpreterRunner dynamic_interpreter = 0;


  void VMMethod::init(STATE) {
#ifdef USE_DYNAMIC_INTERPRETER
    if(!state->shared.config.dynamic_interpreter_enabled) {
      dynamic_interpreter = NULL;
      standard_interpreter = &VMMethod::interpreter;
      return;
    }

    /*
    if(dynamic_interpreter == NULL) {
      uint8_t* buffer = new uint8_t[1024 * 1024 * 1024];
      JITCompiler jit(buffer);
      jit.create_interpreter(state);
      if(getenv("DUMP_DYN")) {
        jit.assembler().show();
      }

      dynamic_interpreter = reinterpret_cast<Runner>(buffer);
    }
    */

    standard_interpreter = dynamic_interpreter;
#else
    dynamic_interpreter = NULL;
    standard_interpreter = &VMMethod::interpreter;
#endif
  }

  /*
   * Turns a CompiledMethod's InstructionSequence into a C array of opcodes.
   */
  VMMethod::VMMethod(STATE, CompiledMethod* meth)
    : machine_method_(state)
    , run(standard_interpreter)
    , original(state, meth)
    , type(NULL)
    , jitted_(false)
  {
    meth->set_executor(&VMMethod::execute);

    total = meth->iseq()->opcodes()->num_fields();
    if(Tuple* tup = try_as<Tuple>(meth->literals())) {
      blocks.resize(tup->num_fields(), NULL);
    }

    opcodes = new opcode[total];

    fill_opcodes(state);
    stack_size =    meth->stack_size()->to_native();
    number_of_locals = meth->number_of_locals();

    total_args =    meth->total_args()->to_native();
    required_args = meth->required_args()->to_native();
    if(meth->splat()->nil_p()) {
      splat_position = -1;
    } else {
      splat_position = as<Integer>(meth->splat())->to_native();
    }

    setup_argument_handler(meth);

#ifdef USE_USAGE_JIT
    // Disable JIT for large methods
    if(meth->primitive()->nil_p() &&
        state->shared.config.jit_enabled &&
        total < (size_t)state->shared.config.jit_max_method_size) {
      call_count = 0;
    } else {
      call_count = -1;
    }
#else
    call_count = 0;
#endif

    machine_method_.set(Qnil);
  }

  VMMethod::~VMMethod() {
    delete[] opcodes;
  }

  void VMMethod::fill_opcodes(STATE) {
    Tuple* ops = original->iseq()->opcodes();
    Object* val;
    for(size_t index = 0; index < total;) {
      val = ops->at(state, index);
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
        case InstructionSequence::insn_send_super_stack_with_splat: {
          native_int which = opcodes[index + 1];
          SendSite::Internal* cache = new SendSite::Internal(which);
          SendSite* ss = as<SendSite>(original->literals()->at(state, which));
          ss->inner_cache_ = cache;
          cache->name = ss->name();
          opcodes[index + 1] = reinterpret_cast<intptr_t>(cache);
          break;
        }
        }

        index += width;
      }
    }
  }

  void VMMethod::set_machine_method(MachineMethod* mm) {
    machine_method_.set(mm);
  }

  // Argument handler implementations

#ifdef USE_SPECIALIZED_EXECUTE

  // For when the method expects no arguments at all (no splat, nothing)
  class NoArguments {
  public:
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
      return args.total() == 0;
    }
  };

  // For when the method expects 1 and only 1 argument
  class OneArgument {
  public:
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
      if(args.total() != 1) return false;
      scope->set_local(0, args.get_argument(0));
      return true;
    }
  };

  // For when the method expects 2 and only 2 arguments
  class TwoArguments {
  public:
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
      if(args.total() != 2) return false;
      scope->set_local(0, args.get_argument(0));
      scope->set_local(1, args.get_argument(1));
      return true;
    }
  };

  // For when the method expects 3 and only 3 arguments
  class ThreeArguments {
  public:
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
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
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
      if((native_int)args.total() != vmm->total_args) return false;

      for(native_int i = 0; i < vmm->total_args; i++) {
        scope->set_local(i, args.get_argument(i));
      }

      return true;
    }
  };

  // For when a method takes all arguments as a splat
  class SplatOnlyArgument {
  public:
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
      const size_t total = args.total();
      Array* ary = Array::create(state, total);

      for(size_t i = 0; i < total; i++) {
        ary->set(state, i, args.get_argument(i));
      }

      scope->set_local(vmm->splat_position, ary);
      return true;
    }
  };
#endif

  // The fallback, can handle all cases
  class GenericArguments {
  public:
    bool call(STATE, VMMethod* vmm, VariableScope* scope, Arguments& args) {
      const bool has_splat = (vmm->splat_position >= 0);

      // expecting 0, got 0.
      if(vmm->total_args == 0 and args.total() == 0) {
        if(has_splat) {
          scope->set_local(vmm->splat_position, Array::create(state, 0));
        }

        return true;
      }

      // Too few args!
      if((native_int)args.total() < vmm->required_args) return false;

      // Too many args (no splat!)
      if(!has_splat && (native_int)args.total() > vmm->total_args) return false;

      // Umm... something too do with figuring out how to handle
      // splat and optionals.
      native_int fixed_args = vmm->total_args;
      if((native_int)args.total() < vmm->total_args) {
        fixed_args = (native_int)args.total();
      }

      // Copy in the normal, fixed position arguments
      for(native_int i = 0; i < fixed_args; i++) {
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
        if((native_int)args.total() > vmm->total_args) {
          size_t splat_size = args.total() - vmm->total_args;
          ary = Array::create(state, splat_size);

          for(size_t i = 0, n = vmm->total_args; i < splat_size; i++, n++) {
            ary->set(state, i, args.get_argument(n));
          }
        } else {
          ary = Array::create(state, 0);
        }

        scope->set_local(vmm->splat_position, ary);
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

  void VMMethod::specialize(STATE, TypeInfo* ti) {
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

    find_super_instructions();
  }

  void VMMethod::find_super_instructions() {
    return;
    for(size_t index = 0; index < total;) {
      size_t width = InstructionSequence::instruction_width(opcodes[index]);
      int super = instructions::find_superop(&opcodes[index]);
      if(super > 0) {
        opcodes[index] = (opcode)super;
      }
      index += width;
    }
  }

  void VMMethod::setup_argument_handler(CompiledMethod* meth) {
#ifdef USE_SPECIALIZED_EXECUTE
    // If there are no optionals, only a fixed number of positional arguments.
    if(total_args == required_args) {
      // if no arguments are expected
      if(total_args == 0) {
        // and there is no splat, use the fastest case.
        if(splat_position == -1) {
          meth->set_executor(&VMMethod::execute_specialized<NoArguments>);

        // otherwise use the splat only case.
        } else {
          meth->set_executor(&VMMethod::execute_specialized<SplatOnlyArgument>);
        }
        return;

      // Otherwise use the few specialized cases iff there is no splat
      } else if(splat_position == -1) {
        switch(total_args) {
        case 1:
          meth->set_executor(&VMMethod::execute_specialized<OneArgument>);
          return;
        case 2:
          meth->set_executor(&VMMethod::execute_specialized<TwoArguments>);
          return;
        case 3:
          meth->set_executor(&VMMethod::execute_specialized<ThreeArguments>);
          return;
        default:
          meth->set_executor(&VMMethod::execute_specialized<FixedArguments>);
          return;
        }
      }
    }

    // Lastly, use the generic case that handles all cases
    meth->set_executor(&VMMethod::execute_specialized<GenericArguments>);
#endif
  }

  /* This is the execute implementation used by normal Ruby code,
   * as opposed to Primitives or FFI functions.
   * It prepares a Ruby method for execution.
   * Here, +exec+ is a VMMethod instance accessed via the +vmm+ slot on
   * CompiledMethod.
   *
   * This method works as a template even though it's here because it's never
   * called from outside of this file. Thus all the template expansions are done.
   */
  template <typename ArgumentHandler>
    Object* VMMethod::execute_specialized(STATE, CallFrame* previous,
                                          Dispatch& msg, Arguments& args) {

      CompiledMethod* cm = as<CompiledMethod>(msg.method);
      VMMethod* vmm = cm->backend_method_;

#ifdef USE_USAGE_JIT
#ifdef ENABLE_LLVM
    // A negative call_count means we've disabled usage based JIT
    // for this method.
    if(vmm->call_count >= 0) {
      if(vmm->call_count >= state->shared.config.jit_call_til_compile) {
        vmm->call_count = -1; // So we don't try and jit twice at the same time
        state->stats.jitted_methods++;

        LLVMState* ls = LLVMState::get(state);

        ls->compile_soon(state, vmm);

        if(state->shared.config.jit_show_compiling) {
          std::cout << "[[[ JIT Queued method "
                    << ls->queued_methods() << "/"
                    << ls->jitted_methods() << " ]]]\n";
        }
      } else {
        vmm->call_count++;
      }
    }
#endif
#endif

      VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                                 (vmm->number_of_locals * sizeof(Object*)));

      scope->prepare(args.recv(), msg.module, args.block(), cm, vmm->number_of_locals);

      InterpreterCallFrame frame;
      frame.stk = (Object**)alloca(vmm->stack_size * sizeof(Object*));
      frame.prepare(vmm->stack_size);

      frame.previous = previous;
      frame.flags =    0;
      frame.msg =      &msg;
      frame.cm =       cm;
      frame.scope =    scope;

      // If argument handling fails..
      ArgumentHandler arghandler;
      if(arghandler.call(state, vmm, scope, args) == false) {
        Exception* exc =
          Exception::make_argument_error(state, vmm->required_args, args.total(), msg.name);
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(1), &frame));
        state->thread_state()->raise_exception(exc);

        return NULL;
      }

      Object* ret;

#ifdef RBX_PROFILER
      if(unlikely(state->shared.profiling())) {
        profiler::MethodEntry method(state, msg, args, cm);
        ret = run_interpreter(state, vmm, &frame, args);
      } else {
        ret = run_interpreter(state, vmm, &frame, args);
      }
#else
      ret = run_interpreter(state, vmm, &frame, args);
#endif

      frame.scope->exit();

      return ret;
    }

  /** @todo Is this redundant after having gone through set_argument_handler? --rue */
  Object* VMMethod::execute(STATE, CallFrame* previous, Dispatch& msg, Arguments& args) {
    return execute_specialized<GenericArguments>(state, previous, msg, args);
  }

  /* This is a noop for this class. */
  void VMMethod::compile(STATE) { }

  /*
   * Turns a VMMethod into a C++ vector of Opcodes.
   */
  std::vector<Opcode*> VMMethod::create_opcodes() {
    std::vector<Opcode*> ops;
    std::map<int, size_t> stream2opcode;

    VMMethod::Iterator iter(this);

    /* Fill +ops+ with all our Opcode objects, maintain
     * the map from stream position to instruction. */
    for(size_t ipos = 0; !iter.end(); ipos++, iter.inc()) {
      stream2opcode[iter.position] = ipos;
      Opcode* lop = new Opcode(iter);
      ops.push_back(lop);
    }

    /* Iterate through the ops, fixing goto locations to point
     * to opcodes and set start_block on any opcode that is
     * the beginning of a block */
    bool next_new = false;

    for(std::vector<Opcode*>::iterator i = ops.begin(); i != ops.end(); i++) {
      Opcode* op = *i;
      if(next_new) {
        op->start_block = true;
        next_new = false;
      }

      /* We patch and mark where we branch to. */
      if(op->is_goto()) {
        op->arg1 = stream2opcode[op->arg1];
        ops.at(op->arg1)->start_block = true;
      }

      /* This terminates the block. */
      if(op->is_terminator()) {
        /* this ends a block. */
        next_new = true;
      }
    }

    // TODO take the exception table into account here

    /* Go through again and assign each opcode a block
     * number. */
    size_t block = 0;
    for(std::vector<Opcode*>::iterator i = ops.begin(); i != ops.end(); i++) {
      Opcode* op = *i;

      if(op->start_block) block++;
      op->block = block;
    }

    return ops;
  }

  /*
   * Ensures the specified IP value is a valid address.
   */
  bool VMMethod::validate_ip(STATE, size_t ip) {
    /* Ensure ip is valid */
    VMMethod::Iterator iter(this);
    for(; !iter.end(); iter.inc()) {
      if(iter.position >= ip) break;
    }
    return ip == iter.position;
  }

  /*
   * Sets breakpoint flags on the specified opcode.
   */
  void VMMethod::set_breakpoint_flags(STATE, size_t ip, bpflags flags) {
    if(validate_ip(state, ip)) {
      opcodes[ip] &= 0x00ffffff;    // Clear the high byte
      opcodes[ip] |= flags & 0xff000000;
    }
  }

  /*
   * Gets breakpoint flags on the specified opcode.
   */
  bpflags VMMethod::get_breakpoint_flags(STATE, size_t ip) {
    if(validate_ip(state, ip)) {
      return opcodes[ip] & 0xff000000;
    }
    return 0;
  }

  bool Opcode::is_goto() {
    switch(op) {
    case InstructionSequence::insn_goto_if_false:
    case InstructionSequence::insn_goto_if_true:
    case InstructionSequence::insn_goto_if_defined:
    case InstructionSequence::insn_goto:
      return true;
    }

    return false;
  }

  bool Opcode::is_terminator() {
    switch(op) {
    case InstructionSequence::insn_send_method:
    case InstructionSequence::insn_send_stack:
    case InstructionSequence::insn_send_stack_with_block:
    case InstructionSequence::insn_send_stack_with_splat:
    case InstructionSequence::insn_meta_send_op_plus:
    case InstructionSequence::insn_meta_send_op_minus:
    case InstructionSequence::insn_meta_send_op_equal:
    case InstructionSequence::insn_meta_send_op_lt:
    case InstructionSequence::insn_meta_send_op_gt:
    case InstructionSequence::insn_meta_send_op_tequal:
      return true;
    }

    return false;
  }

  bool Opcode::is_send() {
    switch(op) {
    case InstructionSequence::insn_send_method:
    case InstructionSequence::insn_send_stack:
    case InstructionSequence::insn_send_stack_with_block:
    case InstructionSequence::insn_send_stack_with_splat:
    case InstructionSequence::insn_meta_send_op_plus:
    case InstructionSequence::insn_meta_send_op_minus:
    case InstructionSequence::insn_meta_send_op_equal:
    case InstructionSequence::insn_meta_send_op_lt:
    case InstructionSequence::insn_meta_send_op_gt:
    case InstructionSequence::insn_meta_send_op_tequal:
      return true;
    }

    return false;

  }

  Object* VMMethod::run_interpreter(STATE, VMMethod* const vmm,
                                    InterpreterCallFrame* const call_frame,
                                    Arguments& args) {
    Object* return_value;
    static int tick = 0;

    for(;;) {
    continue_to_run:
      if(unlikely(++tick > 0xff)) {
        tick = 0;
        if(!state->check_stack(call_frame, &state)) return NULL;
      }

      if(unlikely(state->interrupts.check)) {
        state->interrupts.check = false;
        state->collect_maybe(call_frame);
      }

      if(unlikely(state->check_local_interrupts)) {
        if(!state->process_async(call_frame)) return NULL;
      }

      try {
        return_value = (*vmm->run)(state, vmm, call_frame, args);
      } catch(TypeError& e) {
        Exception* exc =
          Exception::make_type_error(state, e.type, e.object, e.reason);
        exc->locations(state, System::vm_backtrace(state, 0, call_frame));

        state->thread_state()->raise_exception(exc);
        return_value = 0;
      }

      if(return_value) return return_value;

      ThreadState* th = state->thread_state();
      // if return_value is NULL, then there is an exception outstanding
      //
      switch(th->raise_reason()) {
      case cException:
        if(call_frame->has_unwinds_p()) {
          UnwindInfo& info = call_frame->pop_unwind();
          call_frame->position_stack(info.stack_depth);
          call_frame->set_ip(info.target_ip);
          /*
          if(vmm->machine_method_.get()) {
            call_frame->set_native_ip(vmm->machine_method_->resolve_virtual_ip(info.target_ip));
          }
          */
        } else {
          return NULL;
        }
        break;

      case cReturn:
      case cBreak:
        // Otherwise, we're doing a long return/break unwind through
        // here. We need to run ensure blocks.
        while(call_frame->has_unwinds_p()) {
          UnwindInfo& info = call_frame->pop_unwind();
          if(info.for_ensure()) {
            call_frame->position_stack(info.stack_depth);
            call_frame->set_ip(info.target_ip);
            /*
            if(vmm->machine_method_.get()) {
              call_frame->set_native_ip(vmm->machine_method_->resolve_virtual_ip(info.target_ip));
            }
            */

            // Don't reset ep here, we're still handling the return/break.
            goto continue_to_run;
          }
        }

        // Ok, no ensures to run.
        if(th->raise_reason() == cReturn) {
          // If we're trying to return to here, we're done!
          if(th->destination_scope() == call_frame->scope) {
            Object* val = th->raise_value();
            th->clear_exception(true);
            return val;
          } else {
            // Give control of this exception to the caller.
            return NULL;
          }

        } else { // It's cBreak
          // If we're trying to break to here, we're done!
          if(th->destination_scope() == call_frame->scope) {
            call_frame->push(th->raise_value());
            th->clear_exception(true);
            // Don't return here, because we want to loop back to the top
            // and keep running this method.
          } else {
            // Give control of this exception to the caller.
            return NULL;
          }
        }
        break;
      case cExit:
        return NULL;
      default:
        std::cout << "bug!\n";
        call_frame->print_backtrace(state);
        abort();
      } // switch
    } // for(;;)
  }
}
