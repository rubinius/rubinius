#include "vmmethod.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"
#include "builtin/contexts.hpp"

namespace rubinius {
  VMMethod::VMMethod(STATE, CompiledMethod* meth) :
      original(state, meth), type(NULL) {

    this->execute = VMMethod::executor;

    total = meth->iseq->opcodes->field_count;
    if(Tuple* tup = try_as<Tuple>(meth->literals)) {
      blocks.resize(tup->field_count, NULL);
    }

    opcodes = new opcode[total];

    for(size_t index = 0; index < total; index++) {
      OBJECT val = meth->iseq->opcodes->at(index);
      if(val->nil_p()) {
        opcodes[index] = 0;
      } else {
        opcodes[index] = as<Fixnum>(val)->to_native();
      }
    }

    /*
    std::vector<Opcode*> ops = create_opcodes();
    size_t idx = 0;
    for(std::vector<Opcode*>::iterator i = ops.begin(); i != ops.end(); i++) {
      Opcode* op = *i;
      std::cout << idx++ << ":" << op->block << ": " << InstructionSequence::get_instruction_name(op->op);
      std::cout << "\n";
    }
    */
  }

  VMMethod::~VMMethod() {
    delete[] opcodes;
  }

  void VMMethod::specialize(TypeInfo* ti) {
    type = ti;
    for(size_t i = 0; i < total; i++) {
      opcode op = opcodes[i];

      if(op == InstructionSequence::insn_push_ivar) {
        native_int idx = opcodes[i + 1];
        native_int sym = as<Symbol>(original->literals->at(idx))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = InstructionSequence::insn_push_my_field;
          opcodes[++i] = it->second;
        }
      } else if(op == InstructionSequence::insn_set_ivar) {
        native_int idx = opcodes[i + 1];
        native_int sym = as<Symbol>(original->literals->at(idx))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = InstructionSequence::insn_store_my_field;
          opcodes[++i] = it->second;
        }
      }
    }
  }

  /* This is the executor implementation used by normal Ruby code,
   * as opposed to Primitives or FFI functions.
   * It prepares a Ruby method for execution.
   * Here, +exec+ is a VMMethod instance accessed via the +vmm+ slot on
   * CompiledMethod.
   */
  bool VMMethod::executor(STATE, VMExecutable* exec, Task* task, Message& msg) {
    VMMethod* meth = (VMMethod*)exec;

    MethodContext* ctx = MethodContext::create(state, msg.recv, meth->original.get());
    // TODO - Set the MethodContext's module based on the Message we were passed.

    task->import_arguments(ctx, msg);
    task->make_active(ctx);

    return true;
  }

  /* This is a noop for this class. */
  void VMMethod::compile(STATE) { }

  VMPrimitiveMethod::VMPrimitiveMethod(STATE, CompiledMethod* meth,
                                       rubinius::executor func) :
      VMMethod(state, meth) {
        this->execute = func;
      }

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
