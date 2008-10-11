#include "vmmethod.hpp"
#include "message.hpp"
#include "objectmemory.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"
#include "builtin/contexts.hpp"
#include "builtin/class.hpp"
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

    Tuple* ops = meth->iseq()->opcodes();
    OBJECT val;
    for(size_t index = 0; index < total; index++) {
      val = ops->at(state, index);
      if(val->nil_p()) {
        opcodes[index] = 0;
      } else {
        opcodes[index] = as<Fixnum>(val)->to_native();
      }
    }
  }

  VMMethod::~VMMethod() {
    delete[] opcodes;
  }

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

  /* This is the executo implementation used by normal Ruby code,
   * as opposed to Primitives or FFI functions.
   * It prepares a Ruby method for execution.
   * Here, +exec+ is a VMMethod instance accessed via the +vmm+ slot on
   * CompiledMethod.
   */
  bool VMMethod::execute(STATE, Executable* exec, Task* task, Message& msg) {
    CompiledMethod* cm = as<CompiledMethod>(exec);

    MethodContext* ctx = MethodContext::create(state, msg.recv, cm);
    ctx->module(state, msg.module);
    ctx->name(state, msg.name);

    task->import_arguments(ctx, msg);
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
