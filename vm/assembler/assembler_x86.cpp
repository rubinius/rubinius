#include "assembler_x86.hpp"

namespace assembler_x86 {
  Register eax = { 0 };
  Register ecx = { 1 };
  Register edx = { 2 };
  Register ebx = { 3 };
  Register esp = { 4 };
  Register ebp = { 5 };
  Register esi = { 6 };
  Register edi = { 7 };
  Register no_reg = { -1 };
  Register no_base = ebp;

  void AssemblerX86::show() {
    ud_t ud;

    ud_init(&ud);
    ud_set_mode(&ud, 32);
    ud_set_syntax(&ud, UD_SYN_ATT);
    ud_set_input_buffer(&ud, (uint8_t*)buffer_, pc_ - buffer_);

    while(ud_disassemble(&ud)) {
      std::cout << (void*)(reinterpret_cast<int>(buffer_) + ud_insn_off(&ud));
      std::cout << "  " << ud_insn_asm(&ud);

      if(ud.operand[0].type == UD_OP_JIMM) {
        const void* addr = (const void*)((uintptr_t)buffer_ + ud.pc + (int)ud.operand[0].lval.udword);
        std::cout << "    ; " << addr;
        if(ud.mnemonic == UD_Icall) {
          Dl_info info;
          if(dladdr(addr, &info)) {
            std::cout << " " << info.dli_sname;
          }
        }
      }

      std::cout << "\n";
    }
  }

  ud_t* AssemblerX86::disassemble() {
    ud_t *ud = new ud_t();
    ud_init(ud);
    ud_set_mode(ud, 32);
    ud_set_syntax(ud, UD_SYN_ATT);
    ud_set_input_buffer(ud, (uint8_t*)buffer_, pc_ - buffer_);
    ud_disassemble(ud);
    return ud;
  }

  void AssemblerX86::show_relocations() {
    std::cout << "Relocations:\n";
    for(Relocations::iterator i = relocations_.begin();
        i != relocations_.end();
        i++) {
      std::cout << i->first << ": ";
      std::cout << "address=" << i->second->address();
      switch(i->second->kind()) {
        std::cout << " absolute";
        break;
      case Relocation::Relative:
        std::cout << " relative";
        break;
      case Relocation::LocalAbsolute:
        std::cout << " local";
        break;
      }

      switch(i->second->target_kind()) {
      case Relocation::Absolute:
        break;
      case Relocation::Symbol:
        std::cout << " symbol='" << i->second->symbol() << "'";
        break;
      }

      std::cout << "\n";
    }
  }
}
