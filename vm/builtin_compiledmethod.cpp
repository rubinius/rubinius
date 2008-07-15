#include "builtin.hpp"
#include "ffi.hpp"
#include "marshal.hpp"
#include "primitives.hpp"
#include "llvm.hpp"

namespace rubinius {
  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = (CompiledMethod*)state->new_object(G(cmethod));
    SET(cm, local_count, Object::i2n(0));
    cm->executable = NULL;
    return cm;
  }

  CompiledMethod* CompiledMethod::generate_tramp(STATE) {
    CompiledMethod* cm = CompiledMethod::create(state);

    // Use a stack of 1 so that the return value of the executed method
    // has a place to go
    SET(cm, stack_size, Object::i2n(1));
    SET(cm, required_args, Object::i2n(0));
    SET(cm, total_args, cm->required_args);

    SET(cm, iseq, InstructionSequence::create(state, 1));
    cm->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_halt));

    StaticScope* ss = StaticScope::create(state);
    SET(ss, module, G(object));
    SET(cm, scope, ss);

    cm->formalize(state, false);

    return cm;
  }

  MethodVisibility* MethodVisibility::create(STATE) {
    return (MethodVisibility*)state->new_object(G(cmethod_vis));
  }

  VMMethod* CompiledMethod::formalize(STATE, bool ondemand) {
    if(!executable) {
      if(!primitive->nil_p()) {
        if(SYMBOL name = try_as<Symbol>(primitive)) {
          std::cout << "resolving: "; inspect(state, name);
          executor func = Primitives::resolve_primitive(state, name);

          VMMethod* vmm = new VMPrimitiveMethod(state, this, func);
          executable = vmm;
          return vmm;
        } else {
          std::cout << "Invalid primitive id (not a symbol)" << std::endl;
        }
      }
      VMMethod* vmm;
      if(ondemand) {
        vmm = new VMLLVMMethodUncompiled(state, this);
      } else {
        VMLLVMMethod* llvm = new VMLLVMMethod(state, this);
        llvm->compile();
        vmm = llvm;
      }
      executable = vmm;
      return vmm;
    }

    return dynamic_cast<VMMethod*>(executable);
  }

  void CompiledMethod::specialize(TypeInfo* ti) {
    dynamic_cast<VMMethod*>(executable)->specialize(ti);
  }

  void CompiledMethod::post_marshal(STATE) {
    formalize(state); // side-effect, populates executable
  }

  size_t CompiledMethod::number_of_locals() {
    return local_count->to_nint();
  }

  void CompiledMethod::set_scope(StaticScope* scope) {
    this->scope = scope;
  }

}
