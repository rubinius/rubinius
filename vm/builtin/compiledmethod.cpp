#include "builtin/compiledmethod.hpp"
#include "ffi.hpp"
#include "marshal.hpp"
#include "primitives.hpp"
#include "llvm.hpp"
#include "objectmemory.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"

namespace rubinius {

  void CompiledMethod::init(STATE) {
    GO(cmethod).set(state->new_class("CompiledMethod", G(executable), CompiledMethod::fields));
    G(cmethod)->set_object_type(CMethodType);

    GO(cmethod_vis).set(state->new_class("Visibility", G(object),
        MethodVisibility::fields, G(cmethod)));
    G(cmethod_vis)->set_object_type(CMVisibilityType);
  }

  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = (CompiledMethod*)state->new_object(G(cmethod));
    SET(cm, local_count, Fixnum::from(0));
    cm->executable = NULL;
    return cm;
  }

  CompiledMethod* CompiledMethod::generate_tramp(STATE, size_t stack_size) {
    CompiledMethod* cm = CompiledMethod::create(state);

    SET(cm, stack_size, Fixnum::from(stack_size));
    SET(cm, required_args, Fixnum::from(0));
    SET(cm, total_args, cm->required_args);
    SET(cm, name, String::create(state, "__halt__")->to_sym(state));

    SET(cm, iseq, InstructionSequence::create(state, 1));
    cm->iseq->opcodes->put(state, 0, Fixnum::from(InstructionSequence::insn_halt));

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
          //std::cout << "resolving: "; inspect(state, name);
          executor func = Primitives::resolve_primitive(state, name);

          VMMethod* vmm = new VMPrimitiveMethod(state, this, func);
          executable = vmm;
          return vmm;
        } else {
          //std::cout << "Invalid primitive id (not a symbol)" << std::endl;
        }
      }
      VMMethod* vmm;
      /* Controls whether we use LLVM out of the gate or not. */
      if(state->config.compile_up_front) {
        if(ondemand) {
          vmm = new VMLLVMMethodUncompiled(state, this);
        } else {
          VMLLVMMethod* llvm = new VMLLVMMethod(state, this);
          llvm->compile(state);
          vmm = llvm;
        }
      } else {
        vmm = new VMMethod(state, this);
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
    return local_count->to_native();
  }

  void CompiledMethod::set_scope(StaticScope* scope) {
    this->scope = scope;
  }

}
