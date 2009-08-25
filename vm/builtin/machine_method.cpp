#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/machine_method.hpp"
#include "vmmethod.hpp"
#include "assembler/jit.hpp"
#include "dispatch.hpp"

#include "vm/exception.hpp"

#include "detection.hpp"

#ifdef ENABLE_LLVM
#include "llvm/jit.hpp"
#include <llvm/Support/CommandLine.h>
#endif

// #define MM_DEBUG

namespace rubinius {

  void MachineMethod::init(STATE) {
    GO(machine_method).set(state->new_class_under("MachineMethod", G(rubinius)));
    GO(machine_method)->name(state, state->symbol("Rubinius::MachineMethod"));

#ifdef ENABLE_LLVM
    //setenv("RBX_JIT", "--debug-only=jit", 1);
    llvm::cl::ParseEnvironmentOptions("rbx", "RBX_JIT", "blah", false);
#endif
  }


  static void* adjust(void* old_base, void* new_base, void* address) {
    uintptr_t diff = reinterpret_cast<uintptr_t>(new_base) -
                     reinterpret_cast<uintptr_t>(old_base);

    return reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(address) + diff);
  }

  MachineMethod* MachineMethod::create(STATE, VMMethod* vmm, JITCompiler& jit) {
    size_t code_size = jit.assembler().used_bytes();
    MachineMethod* mm = state->new_struct<MachineMethod>(G(machine_method));

    mm->vmmethod_ = vmm;
    mm->code_size_ = code_size;

    mm->set_function(reinterpret_cast<void*>(malloc(code_size)));
    std::memcpy(mm->function(), jit.assembler().buffer(), code_size);

    assembler::Relocations& current = jit.assembler().relocations();
    mm->relocations_ = new assembler::Relocation*[current.size()];

    int j = 0;
    for(assembler::Relocations::iterator i = current.begin();
        i != current.end();
        i++, j++) {
      assembler::Relocation* rel = new assembler::Relocation(*i->second);
      rel->adjust_base(jit.assembler().buffer(), mm->function());
      mm->relocations_[j] = rel;

      rel->resolve_and_write();
    }

    mm->virtual2native_ = new CodeMap();
    CodeMap& v2n = *mm->virtual2native_;

    for(CodeMap::iterator i = jit.code_map().begin();
        i != jit.code_map().end();
        i++) {
      v2n[i->first] = adjust(jit.assembler().buffer(), mm->function(), i->second);
    }

    mm->comments_ = new AddressComments();
    AddressComments& comments = *mm->comments_;

    for(AddressComments::iterator i = jit.comments().begin();
        i != jit.comments().end();
        i++) {
      comments[adjust(jit.assembler().buffer(), mm->function(), i->first)] =
          i->second;
    }

    return mm;
  }

  MachineMethod* MachineMethod::create(STATE, VMMethod* vmm) {
#ifdef ENABLE_LLVM
    LLVMState* ls = LLVMState::get(state);
    timer::Running timer(ls->time_spent);

    LLVMCompiler* jit = new LLVMCompiler();
    jit->compile(ls, vmm);

    MachineMethod* mm = state->new_struct<MachineMethod>(G(machine_method));

    mm->vmmethod_ = vmm;
    mm->code_size_ = 0;
    mm->set_function(jit->function_pointer(state));
    mm->relocations_ = 0;
    mm->virtual2native_ = 0;
    mm->comments_ = 0;

    mm->jit_data_ = reinterpret_cast<void*>(jit);
    vmm->set_jitted(jit->llvm_function(), jit->code_bytes(), jit->function_pointer());

    return mm;
#else
    return (MachineMethod*)Qnil;
#endif
  }

  void MachineMethod::update(VMMethod* vmm, LLVMCompiler* jit) {
#ifdef ENABLE_LLVM
    vmmethod_ = vmm;
    code_size_ = 0;
    set_function(jit->function_pointer());
    relocations_ = 0;
    virtual2native_ = 0;
    comments_ = 0;

    jit_data_ = reinterpret_cast<void*>(jit);
#endif
  }

  void* MachineMethod::resolve_virtual_ip(int ip) {
    CodeMap::iterator i = virtual2native_->find(ip);
    if(i == virtual2native_->end()) return NULL;
    return i->second;
  }

  Object* MachineMethod::show(STATE) {
#ifdef ENABLE_LLVM
    if(code_size_ == 0) {
      std::cout << "== llvm assembly ==\n";
      reinterpret_cast<LLVMCompiler*>(jit_data_)->show_assembly(state);
    } else {
      std::cout << "== stats ==\n";
      std::cout << "number of bytecodes: " << vmmethod_->total << "\n";
      std::cout << " bytes of assembley: " << code_size_ << "\n";
      double ratio = (double)code_size_ / (double)vmmethod_->total;
      std::cout << "       direct ratio: " << ratio << "\n";
      ratio = (double)code_size_ / ((double)vmmethod_->total * sizeof(rubinius::opcode));
      std::cout << "       memory ratio: " << ratio << "\n";
      std::cout << "\n== x86 assembly ==\n";
      assembler_x86::AssemblerX86::show_buffer(function(), code_size_, false, comments_);
    }
#endif

    return Qnil;
  }

  Object * MachineMethod::run_code(STATE, CallFrame* previous, Dispatch& msg, Arguments& args) {
    CompiledMethod* cm = as<CompiledMethod>(msg.method);
    VMMethod* vmm = cm->backend_method();
    MachineMethod* mm = vmm->machine_method();
    void* func = mm->function();
    return ((executor)func)(state, previous, msg, args);
  }

  Object* MachineMethod::activate() {
    if(!function()) return Qfalse;
#ifdef MM_DEBUG
    vmmethod_->original.get()->execute = &MachineMethod::run_code;
#else
    vmmethod_->original.get()->execute = reinterpret_cast<executor>(function());
#endif
    vmmethod_->set_machine_method(this);
    return Qtrue;
  }
}
