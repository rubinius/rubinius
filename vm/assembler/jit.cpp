#include "assembler_x86.hpp"
#include "oop.hpp"
#include "jit_state.h"
#include "operations.hpp"
#include "instructions.hpp"
#include "vmmethod.hpp"

#include "builtin/iseq.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"

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

  void JITCompiler::slow_plus_path() {

  }

  static void maybe_return(AssemblerX86 &a, int i, uint32_t **last_imm,
      AssemblerX86::NearJumpLocation &fin) {
    // EDX will contain the native ip, to be stored
    // back into the MethodContext in the epilogue.
    a.mov_delayed(edx, last_imm);

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

  void JITCompiler::compile(VMMethod* vmm) {
    // Used for fixups
    uint32_t* last_imm = NULL;

    // A label pointing to the code for each virtual ip
    std::vector<AssemblerX86::NearJumpLocation> labels(vmm->total);

    // The location of the instructions that save ip into the current
    // MethodContext then clear the stack and return
    AssemblerX86::NearJumpLocation fin;

    // The location of just the instructions that clear the stack and return
    AssemblerX86::NearJumpLocation real_fin;

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

      // Update our table of virtual ip to native ip
      virtual2native[i] = reinterpret_cast<void*>(a.pc());

      // If we registers an immediate to be update, do it now.
      // TODO a.pc() is bigger than a uint32_t on 64bit
      if(last_imm) {
        *last_imm = (uint32_t)a.pc();
        Relocation* rel = new Relocation(Relocation::LocalAbsolute,
            last_imm, a.pc(), 0);
        a.add_relocation(last_imm, rel);

        last_imm = NULL;
        // Because this is now a jump destination, reset the register
        // usage since we don't know the state off things when we're
        // jumped here.
        ops.reset_usage();
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
      case InstructionSequence::insn_meta_push_0:
        cache_stack();
        s.push((int)Fixnum::from(0));
        break;
      case InstructionSequence::insn_meta_push_1:
        cache_stack();
        s.push((int)Fixnum::from(1));
        break;
      case InstructionSequence::insn_meta_push_2:
        cache_stack();
        s.push((int)Fixnum::from(2));
        break;
      case InstructionSequence::insn_meta_push_neg_1:
        cache_stack();
        s.push((int)Fixnum::from(-1));
        break;
      case InstructionSequence::insn_push_self:
        cache_stack();
        ops.load_self(eax);
        s.push(eax);
        break;

      // Now, for a bit more complicated ones...

      case InstructionSequence::insn_meta_send_op_plus: {
        cache_stack();
        AssemblerX86::NearJumpLocation slow_plus;
        AssemblerX86::NearJumpLocation done;

        // This code is HIGHLY aware that the tag bit for fixnum
        // is a 1 in the low position only.

        // Pull in the top 2 entries on the stack into registers
        s.load_nth(eax, 0);
        s.load_nth(ecx, 1);

        // Perform the bit and to find out if they're both fixnums
        a.mov(edx, eax);
        a.bit_and(edx, ecx);
        a.test(edx, TAG_FIXNUM);
        a.jump_if_not_equal(slow_plus);

        // Ok, they're are both fixnums...
        // And add them together directly
        a.add(eax, ecx);

        // Check the x86 overflow bit, and if so, run the slow path
        a.jump_if_overflow(slow_plus);

        // Everything was good, so subtract 1 because the tag adds an
        // extra 1 to the result
        a.dec(eax);

        // Remove one from the stack
        s.pop();

        // Put the result on the stack
        s.set_top(eax);
        a.jump(done);

        a.set_label(slow_plus);
        ops.call_via_symbol((void*)JITCompiler::slow_plus_path);
        maybe_return(a, i, &last_imm, fin);
        a.set_label(done);
        break;
      }

      case InstructionSequence::insn_set_call_flags:
        ops.store_call_flags(vmm->opcodes[i + 1]);
        break;

        // for any instruction we don't handle with a special code sequence,
        // just call the regular function for it.
      default: {
        uncache_stack();
        const instructions::Implementation* impl = instructions::implementation(op);
        switch(width) {
        case 1:
          ops.call_operation(impl->address, impl->name);
          break;
        case 2:
          ops.call_operation(impl->address, impl->name,
              vmm->opcodes[i + 1]);
          break;
        case 3:
          ops.call_operation(impl->address, impl->name,
              vmm->opcodes[i + 1],
              vmm->opcodes[i + 2]);
          break;
        default:
          std::cout << "Invalid width '" << width << "' for instruction '" <<
            op << "'\n";
          abort();
        }

        instructions::Status status = instructions::check_status(op);
        if(status == instructions::MightReturn) {
          maybe_return(a, i, &last_imm, fin);
        } else if(status == instructions::Terminate) {
          a.jump(real_fin);
        }
        break;
      }
      }

      i += width;
    }

    a.set_label(fin);
    ops.store_ip(ecx, edx);

    a.set_label(real_fin);
    ops.epilogue();

    a.show();
    a.show_relocations();

    std::cout << "Virtual to Native:\n";
    for(CodeMap::iterator i = virtual2native.begin();
        i != virtual2native.end();
        i++) {
      std::cout << i->first << " => " << i->second << "\n";
    }

    std::cout << "\n== reloc ==\n";

    uint8_t *buf2 = new uint8_t[1024];
    AssemblerX86 a2(buf2, a);
    a2.show_relocations();
    std::cout << "\n== code ==\n";
    a2.show();
  }
}
