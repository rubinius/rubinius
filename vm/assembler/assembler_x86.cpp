#include "detection.hpp"
#include "assembler_x86.hpp"

#include <iostream>
#include <iomanip>
#include <cstdlib>

// for abi::__cxa_demangle
#include <cxxabi.h>

namespace assembler_x86 {
  using namespace assembler;

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
    show_buffer(buffer_, pc_ - buffer_);
  }

  void AssemblerX86::show_buffer(void* buffer, size_t size, bool show_hex,
      rubinius::AddressComments* comments) {
    ud_t ud;

    ud_init(&ud);
#ifdef IS_X8664
    ud_set_mode(&ud, 64);
#else
    ud_set_mode(&ud, 32);
#endif
    ud_set_syntax(&ud, UD_SYN_ATT);
    ud_set_input_buffer(&ud, reinterpret_cast<uint8_t*>(buffer), size);

    while(ud_disassemble(&ud)) {
      void* address = reinterpret_cast<void*>(
          reinterpret_cast<uintptr_t>(buffer) + ud_insn_off(&ud));

      if(comments) {
        rubinius::AddressComments::iterator i = comments->find(address);
        if(i != comments->end()) {
          std::cout << "         ;  " << i->second << "\n";
        }
      }

      std::cout << std::setw(10) << std::right
                << address
                << "  ";

      if(show_hex) {
        if(ud_insn_len(&ud) <= 6) {
          std::cout << std::setw(12);
        } else {
          std::cout << std::setw(24);
        }
        std::cout << std::left << ud_insn_hex(&ud) << "  ";
      }
      std::cout << std::setw(24) << std::left << ud_insn_asm(&ud);

      if(ud.operand[0].type == UD_OP_JIMM) {
        const void* addr = (const void*)((uintptr_t)buffer + ud.pc + (int)ud.operand[0].lval.udword);
        std::cout << " ; " << addr;
        if(ud.mnemonic == UD_Icall) {
          Dl_info info;
          if(dladdr(addr, &info)) {
            int status = 0;
            char* cpp_name = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);
            if(status >= 0) {
              // Chop off the arg info from the signature output
              char *paren = strstr(cpp_name, "(");
              *paren = 0;
              std::cout << " " << cpp_name;
              free(cpp_name);
            } else {
              std::cout << " " << info.dli_sname;
            }
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
