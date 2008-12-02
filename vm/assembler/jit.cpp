#include "assembler_x86.hpp"
#include "oop.hpp"
#include "jit_state.h"
#include "operations.hpp"
#include "instructions.hpp"
#include "vmmethod.hpp"

#include "builtin/iseq.hpp"
#include "builtin/contexts.hpp"
#include "instructions.hpp"
#include "jit.hpp"

using namespace assembler_x86;
using namespace operations;
using namespace rubinius;

namespace rubinius {
  JITCompiler::JITCompiler()
    : stack_cached_(false)
    , a()
    , s(a, ebx)
    , ops(s) { }

  void JITCompiler::cache_stack() {
    if(stack_cached_) return;
    stack_cached_ = true;
    ops.load_stack_pointer();
  }

  void JITCompiler::uncache_stack() {
    if(!stack_cached_) return;
    stack_cached_ = false;
    ops.save_stack_pointer();
  }

  void JITCompiler::compile(VMMethod* vmm) {
    uint32_t* last_imm = NULL;

    // A label pointing to the code for each virtual ip
    std::vector<AssemblerX86::NearJumpLocation> labels(vmm->total);

    AssemblerX86::NearJumpLocation fin;
    ops.prologue();

    // Pull native_ip out of the method_context and jump to it if
    // it's not 0.
    //
    // NOTE we don't pull the stack pointer out into ebx by default,
    // which means that any code that is jumped to has to assume it
    // needs to pull it out manually. This is currently not a problem
    // because our jump destinations are always right after calls
    // out to implementions and thus have uncached ebx.
    AssemblerX86::NearJumpLocation normal_start;

    ops.load_native_ip(eax);
    a.cmp(eax, 0);
    a.jump_if_equal(normal_start);
    a.jump(eax);

    a.set_label(normal_start);

    for(size_t i = 0; i < vmm->total;) {
      opcode op = vmm->opcodes[i];
      size_t width = InstructionSequence::instruction_width(op);

      // Set the label location
      a.set_label(labels[i]);

      // If we registers an immediate to be update, do it now.
      // TODO a.pc() is bigger than a uint32_t on 64bit
      if(last_imm) {
        *last_imm = (uint32_t)a.pc();
        last_imm = NULL;
      }

      switch(op) {
      case InstructionSequence::insn_goto:
        a.jump(labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_goto_if_false:
        cache_stack();
        s.load_nth(esi, 0);
        ops.jump_if_false(esi, labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_goto_if_true:
        cache_stack();
        s.load_nth(esi, 0);
        ops.jump_if_true(esi, labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_goto_if_defined:
        cache_stack();
        s.load_nth(esi, 0);
        a.cmp(esi, (int)Qundef);
        a.jump_if_not_equal(labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_pop:
        cache_stack();
        s.pop();
        break;
      case InstructionSequence::insn_push_true:
        cache_stack();
        s.push((int)Qtrue);
        break;
      case InstructionSequence::insn_push_self:
        cache_stack();
        ops.load_self(eax);
        s.push(eax);
        break;

      case InstructionSequence::insn_set_call_flags:
        ops.store_call_flags(vmm->opcodes[i + 1]);
        break;

        // for any instruction we don't handle with a special code sequence,
        // just call the regular function for it.
      default: {
        uncache_stack();
        void* impl = instructions::implementation(op);
        switch(width) {
        case 1:
          ops.call_operation(impl);
          break;
        case 2:
          ops.call_operation(impl,
              vmm->opcodes[i + 1]);
          break;
        case 3:
          ops.call_operation(impl,
              vmm->opcodes[i + 1],
              vmm->opcodes[i + 2]);
          break;
        default:
          std::cout << "Invalid width '" << width << "' for instruction '" <<
            op << "'\n";
          abort();
        }

        if(instructions::check_status(op)) {
          // EDX will contain the native ip, to be stored
          // back into the MethodContext in the epilogue.
          a.mov_delayed(edx, &last_imm);

          // ECX will contain the virtual ip, which is stored
          // back into the MethodContext in the epilogue
          // The + 1 is to match the interpreter, where the ip points
          // to the next instruction rather than the current one
          a.mov(ecx, i + 1);

          // If the return value of the operation (located in eax),
          // is cExecuteRestart, then jump to the epilogue, which
          // stores ecx as the virtual ip and returns.
          a.cmp(eax, cExecuteRestart);
          a.jump_if_equal(fin);
        }
        break;
      }
      }

      i += width;
    }

    a.set_label(fin);
    ops.store_ip(ecx, edx);
    a.epilogue();

    a.show();
  }
}
