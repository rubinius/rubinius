#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/machine_method.hpp"
#include "vm/exception.hpp"

#include "detection.hpp"

namespace rubinius {

  void MachineMethod::init(STATE) {
    GO(machine_method).set(state->new_class_under("MachineMethod", G(rubinius)));
  }

  MachineMethod* MachineMethod::create(STATE, VMMethod* vmm, JITCompiler& jit) {
    size_t code_size = jit.assembler().used_bytes();
    MachineMethod* mm = state->new_struct<MachineMethod>(G(machine_method));

    mm->vmmethod_ = vmm;
    mm->code_size_ = code_size;
    mm->virtual2native_ = new CodeMap(jit.code_map());
    mm->relocations_ = new assembler::Relocations();

    mm->set_function(reinterpret_cast<void*>(malloc(code_size)));
    std::memcpy(mm->function(), jit.assembler().buffer(), code_size);

    assembler::Relocations& current = jit.assembler().relocations();
    for(assembler::Relocations::iterator i = current.begin();
        i != current.end();
        i++) {
      assembler::Relocation* rel = new assembler::Relocation(*i->second);
      rel->adjust_base(jit.assembler().buffer(), mm->function());
      mm->relocations_->insert(std::pair<void*, assembler::Relocation*>(rel->instruction_location(), rel));

      rel->resolve_and_write();
    }

    return mm;
  }

  Object* MachineMethod::show() {
    std::cout << "== stats ==\n";
    std::cout << "number of bytecodes: " << vmmethod_->total << "\n";
    std::cout << " bytes of assembley: " << code_size_ << "\n";
    double ratio = (double)code_size_ / (double)vmmethod_->total;
    std::cout << "       direct ratio: " << ratio << "\n";
    ratio = (double)code_size_ / ((double)vmmethod_->total * sizeof(rubinius::opcode));
    std::cout << "       memory ratio: " << ratio << "\n";
    std::cout << "\n== x86 assembly ==\n";
    assembler_x86::AssemblerX86::show_buffer(function(), code_size_);
    return Qnil;
  }

  void MachineMethod::run_code(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
#ifdef IS_X86
    MachineMethod* mm = vmm->machine_method();
    void* func = mm->function();
    ((Runner)func)(vmm, task, ctx);
#else
    Assertion::raise("Only supported on x86");
#endif
  }

  Object* MachineMethod::activate() {
#ifdef IS_X86
    // vmmethod_->run = (Runner)function();
    vmmethod_->run = MachineMethod::run_code;
    vmmethod_->set_machine_method(this);
    return Qtrue;
#else
    Assertion::raise("Only supported on x86");
    return Qfalse;
#endif
  }
}
