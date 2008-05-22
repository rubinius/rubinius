#include "objects.hpp"

namespace rubinius {
  VMMethod::VMMethod(STATE, CompiledMethod* meth) : 
      original(state, meth), type(NULL) {
    total = meth->iseq->opcodes->field_count;

    opcodes = new opcode[total];

    for(size_t index = 0; index < total; index++) {
      OBJECT val = meth->iseq->opcodes->at(index);
      if(val->nil_p()) {
        opcodes[index] = 0;
      } else {
        opcodes[index] = as<Fixnum>(val)->n2i();
      }
    }
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

  void VMMethod::execute(STATE, Task* task, Message& msg) {
    MethodContext* ctx = task->generate_context(msg.recv, original.get(), this);

    task->import_arguments(ctx, msg);
    task->make_active(ctx);
  }

  VMPrimitiveMethod::VMPrimitiveMethod(STATE, CompiledMethod* meth, primitive_func func) :
      VMMethod(state, meth), fp(func) { }

  void VMPrimitiveMethod::execute(STATE, Task* task, Message& msg) {
    try {
      OBJECT ret = CALL_PRIM(state->primitives, fp)(state, msg);
      task->primitive_return(ret, msg);
    } catch(PrimitiveFailed& e) {
      VMMethod::execute(state, task, msg);
      return;
    }
  }
}
