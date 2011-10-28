#ifdef ENABLE_LLVM

#include "vm/config.h"
#include "llvm/disassembler.hpp"
#if RBX_LLVM_API_VER == 208
#include <llvm/System/Host.h>
#elif RBX_LLVM_API_VER == 209
#include <llvm/Support/Host.h>
#endif
#include <llvm/Instructions.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetInstrDesc.h>
#include <llvm/MC/MCInstPrinter.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCInst.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <dlfcn.h>

namespace rubinius {

  JITDisassembler::JITDisassembler(void* buffer, size_t size)
  : asm_info(0)
  , disassembler(0)
  , memory_object(0)
  {
    std::string host = llvm::sys::getHostTriple();
    std::string error;
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeAllDisassemblers();
    target = llvm::TargetRegistry::lookupTarget(host, error);
    target_machine = target->createTargetMachine(host, error);
    asm_info = target_machine->getMCAsmInfo();
    if(asm_info) {
      disassembler = target->createMCDisassembler();
      memory_object = new JITMemoryObject((const uint8_t*)buffer, (uint64_t) size);
    }
  }

  JITDisassembler::~JITDisassembler() {
    if(memory_object) delete memory_object;
    if(disassembler) delete disassembler;
    if(target_machine) delete target_machine;
  }

  std::string JITDisassembler::print_machine_code() {

    std::ostringstream output;
    output << std::hex;

    if(!asm_info) {
      return std::string("Can't create assembly information for target");
    }

    llvm::MCInstPrinter* printer = target->createMCInstPrinter(asm_info->getAssemblerDialect(), *asm_info);
    if(!printer) {
      return std::string("No instruction printer for target");
    }

    uint64_t instruction_pointer = 0;
    uint64_t instruction_offset = memory_object->getOffset();
    uint64_t instruction_count = memory_object->getExtent();

    const llvm::TargetInstrInfo* inst_info = target_machine->getInstrInfo();

    while(instruction_pointer < instruction_count) {
      std::string tmp;
      llvm::raw_string_ostream out(tmp);
      llvm::MCInst instruction;
      uint64_t instruction_size;
      uint64_t instruction_position = instruction_offset + instruction_pointer;
      if(disassembler->getInstruction(instruction, instruction_size,
                                      *memory_object, instruction_pointer,
                                      llvm::nulls())) {
        printer->printInst(&instruction, out);

        output << "0x" << instruction_position << "  ";
        output << std::setw(30) << std::left << out.str();

        const llvm::TargetInstrDesc &inst_descr = inst_info->get(instruction.getOpcode());

        for(uint8_t i = 0; i < instruction.getNumOperands(); ++i) {
          llvm::MCOperand& op = instruction.getOperand(i);
          if(op.isImm()) {
            uint64_t val = op.getImm();
            // If it's a branch we want to add the complete location
            // as an easier way to see where we are jumping to.
            if(inst_descr.isBranch()) {
              output << "; 0x" << instruction_position + instruction_size + val;
            }
            // Check whether it might be a immediate that references
            // a specific method for a jump later or in this call instruction.
            Dl_info info;
            if(dladdr((void*)val, &info)) {
              output << "; " << info.dli_sname;
            }
          }
        }
        output << std::endl;
        instruction_pointer += instruction_size;
      } else {
        break;
      }
    }

    return output.str();
  }

}

#endif
