#include "assembler/assembler_x86.hpp"
#include "oop.hpp"
#include "jit_state.h"
#include "operations.hpp"
#include "instructions.hpp"
#include "vmmethod.hpp"

#include "builtin/iseq.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"

#include "instructions.hpp"
#include "assembler/jit.hpp"
#include "event.hpp"

using namespace assembler;
using namespace assembler_x86;
using namespace operations;
using namespace rubinius;

extern "C" {
  ExecuteStatus send_slowly(VMMethod* vmm, Task* task, MethodContext* const ctx, Symbol* name, size_t args);
}

namespace rubinius {
  JITCompiler::JITCompiler()
    : stack_cached_(false)
    , buffer_(new uint8_t[1024*1024])
    , a(buffer_)
    , s(a, ebx)
    , ops(s) { }

  JITCompiler::~JITCompiler() {
    memset(buffer_, 0, 1024*1024);
    delete[] buffer_;
  }

  void JITCompiler::cache_stack(bool force) {
    if(!force && stack_cached_) return;
    stack_cached_ = true;
    ops.load_stack_pointer();
  }

  void JITCompiler::uncache_stack(bool force) {
    if(!force && !stack_cached_) return;
    stack_cached_ = false;
    ops.save_stack_pointer();
  }

  ExecuteStatus JITCompiler::check_interrupts(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    task->state->events->poll();
    return task->state->interrupts.check_events ? cExecuteRestart : cExecuteContinue;
  }

  ExecuteStatus JITCompiler::slow_plus_path(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    return send_slowly(vmm, task, ctx, task->state->globals.sym_plus.get(), 1);
  }

  ExecuteStatus JITCompiler::slow_minus_path(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    return send_slowly(vmm, task, ctx, task->state->globals.sym_minus.get(), 1);
  }

  ExecuteStatus JITCompiler::slow_equal_path(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    return send_slowly(vmm, task, ctx, task->state->globals.sym_equal.get(), 1);
  }

  ExecuteStatus JITCompiler::slow_nequal_path(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    return send_slowly(vmm, task, ctx, task->state->globals.sym_nequal.get(), 1);
  }

  ExecuteStatus JITCompiler::slow_lt_path(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    return send_slowly(vmm, task, ctx, task->state->globals.sym_lt.get(), 1);
  }

  ExecuteStatus JITCompiler::slow_gt_path(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx) {
    return send_slowly(vmm, task, ctx, task->state->globals.sym_gt.get(), 1);
  }

  void JITCompiler::maybe_return(int i, uintptr_t **last_imm, AssemblerX86::NearJumpLocation &fin) {

    // after every call instruction that's passed the ctx
    cache_stack();

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

  void JITCompiler::compile(STATE, VMMethod* vmm) {
    // Used for fixups
    uintptr_t* last_imm = NULL;

    // A label pointing to the code for each virtual ip
    std::vector<AssemblerX86::NearJumpLocation> labels(vmm->total);

    // The location of the instructions that save ip into the current
    // MethodContext then clear the stack and return
    AssemblerX86::NearJumpLocation fin;

    // The location of just the instructions that clear the stack and return
    AssemblerX86::NearJumpLocation real_fin;

    comments_[a.pc()] = "prologue";

    ops.prologue();
    cache_stack();

    // Pull native_ip out of the method_context and jump to it if
    // it's not 0.
    //
    // NOTE we don't pull the stack pointer out into ebx by default,
    // which means that any code that is jumped to has to assume it
    // needs to pull it out manually. This is currently not a problem
    // because our jump destinations are always right after calls
    // out to implementions and thus have uncached ebx.
    AssemblerX86::NearJumpLocation normal_start;

    comments_[a.pc()] = "method reentry";

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

      comments_[a.pc()] = InstructionSequence::get_instruction_name(op);

      // If we registers an immediate to be update, do it now.
      // TODO a.pc() is bigger than a uint32_t on 64bit
      if(last_imm) {

        *last_imm = (uintptr_t)a.pc();
        Relocation* rel = new Relocation(Relocation::LocalAbsolute,
            last_imm, a.pc(), 0);
        a.add_relocation(last_imm, rel);

        last_imm = NULL;
        // Because this is now a jump destination, reset the register
        // usage since we don't know the state off things when we're
        // jumped here.
        ops.reset_usage();
      } else if(labels[i].flags() & cFlagUnwoundTo) {
        // Update our table of virtual ip to native ip
        virtual2native[i] = reinterpret_cast<void*>(a.pc());

        labels[i].flags() |= cRecordV2N;
        // This is a jump destination for exceptions, reset
        // things and register it.
        ops.reset_usage();
      }

      switch(op) {
      case InstructionSequence::insn_noop:
        break;
      case InstructionSequence::insn_goto:
        a.jump(labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_goto_if_false:
        s.load_nth(eax, 0);
        s.pop();
        ops.jump_if_false(eax, labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_goto_if_true:
        s.load_nth(eax, 0);
        s.pop();
        ops.jump_if_true(eax, labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_goto_if_defined:
        s.load_nth(eax, 0);
        s.pop();
        a.cmp(eax, (uintptr_t)Qundef);
        a.jump_if_not_equal(labels[vmm->opcodes[i + 1]]);
        break;
      case InstructionSequence::insn_setup_unwind:
        labels[vmm->opcodes[i + 1]].flags() |= cFlagUnwoundTo;
        goto call_op;
      case InstructionSequence::insn_pop:
        s.pop();
        break;
      case InstructionSequence::insn_dup_top:
        s.load_nth(eax, 0);
        s.push(eax);
        break;
      case InstructionSequence::insn_rotate:
        if(vmm->opcodes[i + 1] != 2) goto call_op;
        // Fall through and use swap if it's just 2
      case InstructionSequence::insn_swap_stack:
        s.load_nth(eax, 0);
        s.load_nth(ecx, 1);
        a.mov(s.position(1), eax);
        s.set_top(ecx);
        break;
      case InstructionSequence::insn_halt:
        a.mov(edx, static_cast<uint32_t>(-1));
        a.mov(ecx, static_cast<uint32_t>(-1));
        a.jump(fin);
        break;
      case InstructionSequence::insn_push_true:
        s.push((uintptr_t)Qtrue);
        break;
      case InstructionSequence::insn_push_false:
        s.push((uintptr_t)Qfalse);
        break;
      case InstructionSequence::insn_push_nil:
        s.push((uintptr_t)Qnil);
        break;
      case InstructionSequence::insn_meta_push_0:
        s.push((uintptr_t)Fixnum::from(0));
        break;
      case InstructionSequence::insn_meta_push_1:
        s.push((uintptr_t)Fixnum::from(1));
        break;
      case InstructionSequence::insn_meta_push_2:
        s.push((uintptr_t)Fixnum::from(2));
        break;
      case InstructionSequence::insn_meta_push_neg_1:
        s.push((uintptr_t)Fixnum::from(-1));
        break;
      case InstructionSequence::insn_push_int:
        s.push((uintptr_t)Fixnum::from(vmm->opcodes[i + 1]));
        break;
      case InstructionSequence::insn_push_self:
        ops.load_self(eax);
        s.push(eax);
        break;

      // Now, for a bit more complicated ones...
      //
      case InstructionSequence::insn_push_local:
        ops.get_local(eax, vmm->opcodes[i + 1]);
        s.push(eax);
        break;

      case InstructionSequence::insn_set_local:
        s.load_nth(edx, 0);
        ops.set_local(edx, vmm->opcodes[i + 1]);
        break;

      case InstructionSequence::insn_push_literal:
        ops.get_literal(eax, vmm->opcodes[i + 1]);
        s.push(eax);
        break;

      case InstructionSequence::insn_meta_send_op_minus:
      case InstructionSequence::insn_meta_send_op_plus: {
        AssemblerX86::NearJumpLocation slow_path;
        AssemblerX86::NearJumpLocation done;

        // This code is HIGHLY aware that the tag bit for fixnum
        // is a 1 in the low position only.

        // Pull in the top 2 entries on the stack into registers
        s.load_nth(ecx, 0);
        s.load_nth(eax, 1);

        // Perform the bit and to find out if they're both fixnums
        a.mov(edx, eax);
        a.bit_and(edx, ecx);
        a.test(edx, TAG_FIXNUM);

        // This seems odd, like the condition is backwards, but thats
        // how test works.
        a.jump_if_equal(slow_path);

        // Ok, they're are both fixnums...
        if(op == InstructionSequence::insn_meta_send_op_plus) {
          // And add them together directly
          a.add(eax, ecx);

          // Check the x86 overflow bit, and if so, run the slow path
          a.jump_if_overflow(slow_path);

          // Everything was good, so subtract 1 because the tag adds an
          // extra 1 to the result
          a.sub(eax, 1);
        } else {
          // And subtract them together directly
          a.sub(eax, ecx);

          // Check the x86 overflow bit, and if so, run the slow path
          a.jump_if_overflow(slow_path);

          // Everything was good, so add 1 because the tag subtracts an
          // extra 1 to the result
          a.add(eax, 1);
        }

        // Remove one from the stack
        s.pop();

        // Put the result on the stack
        s.set_top(eax);

        a.jump(done);

        a.set_label(slow_path);
        uncache_stack();
        if(op == InstructionSequence::insn_meta_send_op_plus) {
          ops.call_via_symbol((void*)JITCompiler::slow_plus_path);
        } else {
          ops.call_via_symbol((void*)JITCompiler::slow_minus_path);
        }
        maybe_return(i, &last_imm, fin);

        // This is a phi point, where the fast path and slow path merge.
        // We have to be sure that the stack cache settings are in sync
        // for both paths taken at this point. To be sure of that, we
        // always run cache_stack() after calling slow_path_plus.
        a.set_label(done);
        break;
      }

      case InstructionSequence::insn_meta_send_op_equal:
      case InstructionSequence::insn_meta_send_op_nequal: {
        AssemblerX86::NearJumpLocation equal_path;
        AssemblerX86::NearJumpLocation slow_path;
        AssemblerX86::NearJumpLocation done;

        s.load_nth(ecx, 0);
        s.load_nth(edx, 1);

        a.mov(eax, ecx);
        a.bit_and(eax, TAG_REF_MASK);

        a.cmp(eax, TAG_REF);
        a.jump_if_equal(slow_path);

        a.mov(eax, edx);
        a.bit_and(eax, TAG_REF_MASK);

        a.cmp(eax, TAG_REF);
        a.jump_if_equal(slow_path);

        // Ok, both are not references

        s.pop();

        a.cmp(ecx, edx);
        a.jump_if_equal(equal_path);

        if(op == InstructionSequence::insn_meta_send_op_equal) {
          s.set_top(cFalse);
          a.jump(done);

          a.set_label(equal_path);

          s.set_top(cTrue);
          a.jump(done);
        }
        else {
          s.set_top(cTrue);
          a.jump(done);

          a.set_label(equal_path);

          s.set_top(cFalse);
          a.jump(done);
        }

        a.set_label(slow_path);
        uncache_stack();

        if(op == InstructionSequence::insn_meta_send_op_equal) {
          ops.call_via_symbol((void*)JITCompiler::slow_equal_path);
        } else {
          ops.call_via_symbol((void*)JITCompiler::slow_nequal_path);
        }
        maybe_return(i, &last_imm, fin);

        a.set_label(done);
        break;
      }

      case InstructionSequence::insn_meta_send_op_lt:
      case InstructionSequence::insn_meta_send_op_gt: {
        AssemblerX86::NearJumpLocation slow_path;
        AssemblerX86::NearJumpLocation done;

        s.load_nth(ecx, 0);
        s.load_nth(edx, 1);

        a.mov(eax, ecx);
        a.bit_and(eax, edx);
        a.bit_and(eax, TAG_FIXNUM_MASK);

        a.cmp(eax, TAG_FIXNUM);
        a.jump_if_not_equal(slow_path);

        // Ok, both are fixnums
        // no need to strip the tags in this case
        // stack top is rhs operand

        s.pop();
        a.cmp(edx, ecx);

        if(op == InstructionSequence::insn_meta_send_op_lt) {
          AssemblerX86::NearJumpLocation less_path;

          a.jump_if_less(less_path);
          s.set_top(cFalse);
          a.jump(done);

          a.set_label(less_path);
          s.set_top(cTrue);
          a.jump(done);
        }
        else {
          AssemblerX86::NearJumpLocation greater_path;

          a.jump_if_greater(greater_path);
          s.set_top(cFalse);
          a.jump(done);

          a.set_label(greater_path);
          s.set_top(cTrue);
          a.jump(done);
        }

        a.set_label(slow_path);
        uncache_stack();

        if(op == InstructionSequence::insn_meta_send_op_lt) {
          ops.call_via_symbol((void*)JITCompiler::slow_lt_path);
        } else {
          ops.call_via_symbol((void*)JITCompiler::slow_gt_path);
        }
        maybe_return(i, &last_imm, fin);

        a.set_label(done);
        break;
      }

      case InstructionSequence::insn_push_const_fast: {
        AssemblerX86::NearJumpLocation slow_path;
        AssemblerX86::NearJumpLocation done;

        ops.get_literal(eax, vmm->opcodes[i + 2]);
        a.cmp(eax, reinterpret_cast<uintptr_t>(Qnil));
        a.jump_if_equal(slow_path);
        a.mov(eax, a.address(eax, FIELD_OFFSET(rubinius::LookupTableAssociation, value_)));
        // TODO this doesn't support autoload!
        s.push(eax);

        a.jump(done);

        a.set_label(slow_path);
        uncache_stack();
        const instructions::Implementation* impl = instructions::implementation(op);
        ops.call_operation(impl->address, impl->name,
            vmm->opcodes[i + 1],
            vmm->opcodes[i + 2]);
        maybe_return(i, &last_imm, fin);

        a.set_label(done);
        break;
      }

      case InstructionSequence::insn_set_call_flags:
        ops.store_call_flags(vmm->opcodes[i + 1]);
        break;

        // for any instruction we don't handle with a special code sequence,
        // just call the regular function for it.
      default: {
call_op:
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
          maybe_return(i, &last_imm, fin);
        } else if(status == instructions::Terminate) {
          a.jump(real_fin);
          cache_stack();
        } else {
          cache_stack();
        }
        break;
      }
      }

      i += width;
    }

    comments_[a.pc()] = "epilogue";

    a.set_label(fin);

    // We could be jumping here from anywhere, assume nothing.
    ops.reset_usage();
    ops.store_ip(ecx, edx);
    uncache_stack();

    a.set_label(real_fin);
    ops.epilogue();
  }
}
