#include "vmmethod.hpp"
#include "message.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"
#include "builtin/contexts.hpp"
#include "builtin/class.hpp"
#include "builtin/sendsite.hpp"

#include "profiler.hpp"

/*
 * An internalization of a CompiledMethod which holds the instructions for the
 * method.
 */
namespace rubinius {

  /*
   * Turns a CompiledMethod's InstructionSequence into a C array of opcodes.
   */
  VMMethod::VMMethod(STATE, CompiledMethod* meth) :
      original(state, meth), type(NULL) {

    meth->set_executor(VMMethod::execute);

    total = meth->iseq()->opcodes()->num_fields();
    if(Tuple* tup = try_as<Tuple>(meth->literals())) {
      blocks.resize(tup->num_fields(), NULL);
    }

    opcodes = new opcode[total];
    Tuple* literals = meth->literals();
    if(literals->nil_p()) {
      sendsites = NULL;
    } else {
      sendsites = new TypedRoot<SendSite*>[literals->num_fields()];
    }

    Tuple* ops = meth->iseq()->opcodes();
    OBJECT val;
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
        case InstructionSequence::insn_send_super_stack_with_splat:
          native_int which = opcodes[index + 1];
          sendsites[which].set(as<SendSite>(literals->at(state, which)), &state->globals.roots);
        }

        index += width;
      }
    }

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
  }

  VMMethod::~VMMethod() {
    delete[] opcodes;
  }

  // Argument handler implementations

  // For when the method expects no arguments at all (no splat, nothing)
  class NoArguments {
  public:
    bool call(STATE, VMMethod* vmm, MethodContext* ctx, Message& msg) {
      return msg.args() == 0;
    }
  };

  // For when the method expects a fixed number of arguments (no splat)
  class FixedArguments {
  public:
    bool call(STATE, VMMethod* vmm, MethodContext* ctx, Message& msg) {
      if((native_int)msg.args() != vmm->total_args) return false;

      for(native_int i = 0; i < vmm->total_args; i++) {
        ctx->set_local(i, msg.get_argument(i));
      }

      return true;
    }
  };

  // The fallback, can handle all cases
  class GenericArguments {
  public:
    bool call(STATE, VMMethod* vmm, MethodContext* ctx, Message& msg) {
      const bool has_splat = (vmm->splat_position >= 0);

      // expecting 0, got 0.
      if(vmm->total_args == 0 and msg.args() == 0) {
        if(has_splat) {
          ctx->set_local(vmm->splat_position, Array::create(state, 0));
        }

        return true;
      }

      // Too few args!
      if((native_int)msg.args() < vmm->required_args) return false;

      // Too many args (no splat!)
      if(!has_splat && (native_int)msg.args() > vmm->total_args) return false;

      // Umm... something too do with figuring out how to handle
      // splat and optionals.
      native_int fixed_args = vmm->total_args;
      if((native_int)msg.args() < vmm->total_args) {
        fixed_args = (native_int)msg.args();
      }

      // Copy in the normal, fixed position arguments
      for(native_int i = 0; i < fixed_args; i++) {
        ctx->set_local(i, msg.get_argument(i));
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
         * even if they're optional, so we can get msg.args() == 0, and
         * total == 1 */
        if((native_int)msg.args() > vmm->total_args) {
          size_t splat_size = msg.args() - vmm->total_args;
          ary = Array::create(state, splat_size);

          for(size_t i = 0, n = vmm->total_args; i < splat_size; i++, n++) {
            ary->set(state, i, msg.get_argument(n));
          }
        } else {
          ary = Array::create(state, 0);
        }

        ctx->set_local(vmm->splat_position, ary);
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
          opcodes[i] = InstructionSequence::insn_push_my_field;
          opcodes[i + 1] = it->second;
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

  template <typename ArgumentHandler>
  bool VMMethod::execute_specialized(STATE, Executable* exec, Task* task, Message& msg) {
    CompiledMethod* cm = as<CompiledMethod>(exec);

    MethodContext* ctx = MethodContext::create(state, msg.recv, cm);

    VMMethod* vmm = cm->backend_method_;

    // Copy in things we all need.
    ctx->module(state, msg.module);
    ctx->name(state, msg.name);

    ctx->block(state, msg.block);
    ctx->args = msg.args();

    // If argument handling fails..
    ArgumentHandler args;
    if(args.call(state, vmm, ctx, msg) == false) {
      // Clear the values from the caller
      task->active()->clear_stack(msg.stack);

      // TODO we've got full control here, we should just raise the exception
      // in the runtime here rather than throwing a C++ exception and raising
      // it later.

      Exception::argument_error(state, vmm->required_args, msg.args());
      // never reached!
    }

#if 0
    if(!probe_->nil_p()) {
      probe_->start_method(this, msg);
    }
#endif

    // Clear the values from the caller
    task->active()->clear_stack(msg.stack);

    task->make_active(ctx);

    if(unlikely(task->profiler)) {
      profiler::Method* prof_meth;
      if(MetaClass* mc = try_as<MetaClass>(msg.module)) {
        OBJECT attached = mc->attached_instance();
        if(Module* mod = try_as<Module>(attached)) {
          prof_meth = task->profiler->enter_method(msg.name, mod->name(), profiler::kNormal);
        } else {
          prof_meth = task->profiler->enter_method(msg.name, attached->id(state), profiler::kNormal);
        }
      } else {
        prof_meth = task->profiler->enter_method(msg.name, msg.module->name(), profiler::kSingleton);
      }

      if(!prof_meth->file()) {
        prof_meth->set_position(cm->file(), cm->start_line(state));
      }
    }

    return true;
  }

  void VMMethod::setup_argument_handler(CompiledMethod* meth) {
    if(total_args == 0 && splat_position == -1) {
      meth->set_executor(execute_specialized<NoArguments>);
    } else {
      meth->set_executor(execute_specialized<GenericArguments>);
    }
  }

  /* This is the execute implementation used by normal Ruby code,
   * as opposed to Primitives or FFI functions.
   * It prepares a Ruby method for execution.
   * Here, +exec+ is a VMMethod instance accessed via the +vmm+ slot on
   * CompiledMethod.
   */
  bool VMMethod::execute(STATE, Executable* exec, Task* task, Message& msg) {
    CompiledMethod* cm = as<CompiledMethod>(exec);

    MethodContext* ctx = MethodContext::create(state, msg.recv, cm);

    VMMethod* vmm = cm->backend_method_;

    // Copy in things we all need.
    ctx->module(state, msg.module);
    ctx->name(state, msg.name);

    ctx->block(state, msg.block);
    ctx->args = msg.args();

    // If argument handling fails..
    GenericArguments args;
    if(args.call(state, vmm, ctx, msg) == false) {
      // Clear the values from the caller
      task->active()->clear_stack(msg.stack);

      // TODO we've got full control here, we should just raise the exception
      // in the runtime here rather than throwing a C++ exception and raising
      // it later.

      Exception::argument_error(state, vmm->required_args, msg.args());
      // never reached!
    }

#if 0
    if(!probe_->nil_p()) {
      probe_->start_method(this, msg);
    }
#endif

    // Clear the values from the caller
    task->active()->clear_stack(msg.stack);

    task->make_active(ctx);

    if(unlikely(task->profiler)) {
      profiler::Method* prof_meth;
      if(MetaClass* mc = try_as<MetaClass>(msg.module)) {
        OBJECT attached = mc->attached_instance();
        if(Module* mod = try_as<Module>(attached)) {
          prof_meth = task->profiler->enter_method(msg.name, mod->name(), profiler::kNormal);
        } else {
          prof_meth = task->profiler->enter_method(msg.name, attached->id(state), profiler::kNormal);
        }
      } else {
        prof_meth = task->profiler->enter_method(msg.name, msg.module->name(), profiler::kSingleton);
      }

      if(!prof_meth->file()) {
        prof_meth->set_position(cm->file(), cm->start_line(state));
      }
    }

    return true;
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
    case InstructionSequence::insn_meta_send_op_nequal:
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
    case InstructionSequence::insn_meta_send_op_nequal:
      return true;
    }

    return false;

  }
}
