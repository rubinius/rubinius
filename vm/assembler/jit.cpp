#include "oop.hpp"
#include "jit_state.h"
#include "assembler/assembler_x86.hpp"
#include "assembler/operations.hpp"
#include "instructions.hpp"
#include "vmmethod.hpp"

#include "builtin/iseq.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"

#include "instructions.hpp"
#include "assembler/jit.hpp"
#include "event.hpp"

using namespace assembler;
using namespace assembler_x86;
using namespace operations;
using namespace rubinius;

extern "C" Object* send_slowly(STATE, VMMethod* vmm, CallFrame* const call_frame, Symbol* name, size_t args);

// It's not thread safe. Although it's okay for now (because we use GIL),
//  at some point in the future you might want to get rid of it
static uint8_t * global_buffer = new uint8_t[1024*1024];

namespace rubinius {
  JITCompiler::JITCompiler()
    : stack_cached_(false)
    , own_buffer_(true)
    , buffer_(global_buffer)
    , a(buffer_)
    , s(a, ebx)
    , ops(s) { }

  JITCompiler::JITCompiler(uint8_t* buf)
    : stack_cached_(false)
    , own_buffer_(false)
    , buffer_(buf)
    , a(buffer_)
    , s(a, ebx)
    , ops(s) { }

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

 Object * JITCompiler::slow_plus_path(STATE, VMMethod* const vmm, CallFrame* const call_frame) {
   return send_slowly(state, vmm, call_frame, G(sym_plus), 1);
 }

 Object * JITCompiler::slow_minus_path(STATE, VMMethod* const vmm, CallFrame* const call_frame) {
   return send_slowly(state, vmm, call_frame, G(sym_minus), 1);
 }

 Object * JITCompiler::slow_equal_path(STATE, VMMethod* const vmm, CallFrame* const call_frame) {
   return send_slowly(state, vmm, call_frame, G(sym_equal), 1);
 }

 Object * JITCompiler::slow_lt_path(STATE, VMMethod* const vmm, CallFrame* const call_frame) {
   return send_slowly(state, vmm, call_frame, G(sym_lt), 1);
 }

 Object * JITCompiler::slow_gt_path(STATE, VMMethod* const vmm, CallFrame* const call_frame) {
   return send_slowly(state, vmm, call_frame, G(sym_gt), 1);
 }

  void JITCompiler::maybe_return(int i, uintptr_t **last_imm, AssemblerX86::NearJumpLocation &fin) {

    // after every call instruction that's passed the ctx
    cache_stack();

    // EDX will contain the native ip, to be stored
    // back into the CallStack in the epilogue.
    a.mov_delayed(edx, last_imm);

    // ECX will contain the virtual ip, which is stored
    // back into the CallStack in the epilogue
    // The + 1 is to match the interpreter, where the ip points
    // to the next instruction rather than the current one
    a.mov(ecx, i);

    // If the return value of the operation (located in eax),
    // is NULL, then jump to the epilogue, which
    // stores ecx as the virtual ip and returns.
    a.cmp(eax, 0);
    a.jump_if_equal(fin);
  }

  void JITCompiler::emit_fast_math(AssemblerX86::NearJumpLocation& done, bool add) {
    AssemblerX86::NearJumpLocation slow_path;

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
    if(add) {
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

    a.jump(done);

    a.set_label(slow_path);
    uncache_stack();
    if(add) {
      ops.call_via_symbol((void*)JITCompiler::slow_plus_path);
    } else {
      ops.call_via_symbol((void*)JITCompiler::slow_minus_path);
    }
    cache_stack();
  }

  void JITCompiler::emit_fast_equal(AssemblerX86::NearJumpLocation& done, bool equal) {
    AssemblerX86::NearJumpLocation equal_path;
    AssemblerX86::NearJumpLocation slow_path;

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

    if(equal) {
      a.mov(eax, cFalse);
      a.jump(done);

      a.set_label(equal_path);

      a.mov(eax, cTrue);
      a.jump(done);
    } else {
      a.mov(eax, cTrue);
      a.jump(done);

      a.set_label(equal_path);

      a.mov(eax, cFalse);
      a.jump(done);
    }

    a.set_label(slow_path);
    uncache_stack();

    if(equal) {
     ops.call_via_symbol((void*)JITCompiler::slow_equal_path);
    }

    cache_stack();
    s.pop();
  }

  void JITCompiler::emit_fast_compare(AssemblerX86::NearJumpLocation& done, bool less) {
    AssemblerX86::NearJumpLocation slow_path;

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

    if(less) {
      AssemblerX86::NearJumpLocation less_path;

      a.jump_if_less(less_path);
      a.mov(eax, cFalse);
      a.jump(done);

      a.set_label(less_path);
      a.mov(eax, cTrue);
      a.jump(done);
    } else {
      AssemblerX86::NearJumpLocation greater_path;

      a.jump_if_greater(greater_path);
      a.mov(eax, cFalse);
      a.jump(done);

      a.set_label(greater_path);
      a.mov(eax, cTrue);
      a.jump(done);
    }

    a.set_label(slow_path);
    uncache_stack();

    if(less) {
     ops.call_via_symbol((void*)JITCompiler::slow_lt_path);
    } else {
     ops.call_via_symbol((void*)JITCompiler::slow_gt_path);
    }
    cache_stack();
    s.pop();
  }

  void JITCompiler::compile(STATE, VMMethod* vmm) {
    // Used for fixups
    uintptr_t* last_imm = NULL;

    // A label pointing to the code for each virtual ip
    std::vector<AssemblerX86::NearJumpLocation> labels(vmm->total);

    // The location of the instructions that save ip into the current
    // CallStack then clear the stack and return
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
      // op = instructions::reverse_superop(op);
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
      case InstructionSequence::insn_ret:
        s.load_nth(eax, 0);
        a.jump(real_fin);
        break;
      // Now, for a bit more complicated ones...

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
        AssemblerX86::NearJumpLocation done;
        emit_fast_math(done, op == InstructionSequence::insn_meta_send_op_plus);
        maybe_return(i+width, &last_imm, fin);

        // This is a phi point, where the fast path and slow path merge.
        // We have to be sure that the stack cache settings are in sync
        // for both paths taken at this point. To be sure of that, we
        // always run cache_stack() after calling slow_path_plus.
        a.set_label(done);
        // Remove one from the stack
        s.pop();
        // Put the result on the stack
        s.set_top(eax);
        break;
      }

      case InstructionSequence::insn_meta_send_op_equal: {
        AssemblerX86::NearJumpLocation done;
        emit_fast_equal(done, op == InstructionSequence::insn_meta_send_op_equal);
        maybe_return(i+width, &last_imm, fin);

        a.set_label(done);

        // Put the result on the stack
        s.set_top(eax);

        break;
      }

      case InstructionSequence::insn_meta_send_op_lt:
      case InstructionSequence::insn_meta_send_op_gt: {
        AssemblerX86::NearJumpLocation done;
        emit_fast_compare(done, op == InstructionSequence::insn_meta_send_op_lt);
        maybe_return(i+width, &last_imm, fin);

        a.set_label(done);
        // Put the result on the stack
        s.set_top(eax);
        break;
      }

      case InstructionSequence::insn_push_const_fast: {
        AssemblerX86::NearJumpLocation slow_path;
        AssemblerX86::NearJumpLocation done;

        ops.get_literal(eax, vmm->opcodes[i + 2]);
        a.cmp(eax, reinterpret_cast<uintptr_t>(Qnil));
        a.jump_if_equal(slow_path);
        a.mov(eax, a.address(eax, 0)); // FIELD_OFFSET(rubinius::LookupTableAssociation, value_)));
        // TODO this doesn't support autoload!
        s.push(eax);

        a.jump(done);

        a.set_label(slow_path);
        uncache_stack();
#if 0
        const instructions::Implementation* impl = instructions::implementation(op);
        ops.call_operation(impl->address, impl->name,
            vmm->opcodes[i + 1],
            vmm->opcodes[i + 2]);
        maybe_return(i, &last_imm, fin);
#endif

        a.set_label(done);
        break;
      }
        // for any instruction we don't handle with a special code sequence,
        // just call the regular function for it.
      default: {
call_op:
        uncache_stack();
#if 0
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
#endif
        cache_stack();

#if 0
        instructions::Status status = instructions::check_status(op);
        if(status == instructions::MightReturn) {
          maybe_return(i + width, &last_imm, fin);
        } else if(status == instructions::Terminate) {
          a.jump(real_fin);
        }
#endif
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

  /*
  static void show_info(VMMethod* const vmm, Task* const task,
                        MethodContext* const ctx, void* ebx) {
    std::cout << (void*)ctx << " " <<
      InstructionSequence::get_instruction_name(vmm->opcodes[ctx->ip - 1]) <<
      " @ " << ctx->ip - 1 <<
      " stack=" << ebx << "/" << ctx->js.stack << "/" << ctx->stk <<
      "\n";
  }

  static void show_info2(VMMethod* const vmm, Task* const task,
                        MethodContext* const ctx, void* ebx) {
    std::cout << "     " <<
      InstructionSequence::get_instruction_name(vmm->opcodes[ctx->ip - 1]) <<
      " @ " << ctx->ip - 1 <<
      " stack=" << ebx <<
      "\n";
  }
  */

  void JITCompiler::emit_opcode(opcode op, AssemblerX86::NearJumpLocation& fin) {
    switch(op) {
    case InstructionSequence::insn_noop:
      break;
    case InstructionSequence::insn_goto:
      ops.load_next_opcode(eax);
      ops.store_virtual_ip(eax);
      break;
    case InstructionSequence::insn_goto_if_false: {
      ops.load_and_increment_ip(edx);
      s.load_nth(eax, 0);
      s.pop();

      AssemblerX86::NearJumpLocation set, done;

      ops.jump_if_false(eax, set);
      a.jump(done);

      a.set_label(set);
      ops.load_opcode(eax, edi, edx);
      ops.store_virtual_ip(eax);

      a.set_label(done);
      break;
    }
    case InstructionSequence::insn_goto_if_true: {
      ops.load_and_increment_ip(edx);
      s.load_nth(eax, 0);
      s.pop();

      AssemblerX86::NearJumpLocation set, done;

      ops.jump_if_true(eax, set);
      a.jump(done);

      a.set_label(set);
      ops.load_opcode(eax, edi, edx);
      ops.store_virtual_ip(eax);

      a.set_label(done);
      break;
    }
    case InstructionSequence::insn_pop:
      s.pop();
      break;
    case InstructionSequence::insn_dup_top:
      s.load_nth(eax, 0);
      s.push(eax);
      break;
    case InstructionSequence::insn_swap_stack:
      s.load_nth(eax, 0);
      s.load_nth(ecx, 1);
      a.mov(s.position(1), eax);
      s.set_top(ecx);
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
      ops.load_next_opcode(eax);
      ops.tag_fixnum(eax);
      s.push(eax);
      break;
    case InstructionSequence::insn_push_self:
      ops.load_self(eax);
      s.push(eax);
      break;
    case InstructionSequence::insn_push_local:
      ops.load_next_opcode(ecx);
      ops.get_local(eax, ecx);        // eax == local
      s.push(eax);
      break;
    case InstructionSequence::insn_set_local:
      ops.load_next_opcode(ecx);
      s.load_nth(edx, 0);
      ops.set_local(edx, ecx);
      break;
    case InstructionSequence::insn_push_literal:
      ops.load_next_opcode(ecx);
      ops.get_literal(eax, ecx);
      s.push(eax);
      break;
    case InstructionSequence::insn_meta_send_op_minus:
    case InstructionSequence::insn_meta_send_op_plus: {
      AssemblerX86::NearJumpLocation done;
      emit_fast_math(done, op == InstructionSequence::insn_meta_send_op_plus);

      // If the return value of the operation (located in eax),
      // is cExecuteRestart, then jump to the epilogue, which
      // stores ecx as the virtual ip and returns.
      a.cmp(eax, cExecuteRestart);
      a.jump_if_equal(fin);

      // This is a phi point, where the fast path and slow path merge.
      // We have to be sure that the stack cache settings are in sync
      // for both paths taken at this point. To be sure of that, we
      // always run cache_stack() after calling slow_path_plus.
      a.set_label(done);
      break;
    }

    case InstructionSequence::insn_meta_send_op_equal: {
      AssemblerX86::NearJumpLocation done;
      emit_fast_equal(done, op == InstructionSequence::insn_meta_send_op_equal);

      // If the return value of the operation (located in eax),
      // is cExecuteRestart, then jump to the epilogue, which
      // stores ecx as the virtual ip and returns.
      a.cmp(eax, cExecuteRestart);
      a.jump_if_equal(fin);

      a.set_label(done);
      break;
    }


    case InstructionSequence::insn_meta_send_op_lt:
    case InstructionSequence::insn_meta_send_op_gt: {
      AssemblerX86::NearJumpLocation done;
      emit_fast_compare(done, op == InstructionSequence::insn_meta_send_op_lt);
      a.cmp(eax, cExecuteRestart);
      a.jump_if_equal(fin);

      a.set_label(done);
      break;
    }

    case InstructionSequence::insn_set_call_flags:
      ops.load_next_opcode(ecx);
      ops.store_call_flags(ecx);
      break;

    default: {
      uncache_stack();
      // ops.call_operation(reinterpret_cast<void*>(show_info), "show_info", ebx);

#if 0
      size_t width = InstructionSequence::instruction_width(op);
      const instructions::Implementation* impl = instructions::implementation(op);
      switch(width) {
      case 1:
        ops.call_operation(impl->address, impl->name);
        break;
      case 2:
        ops.load_next_opcode(eax);
        ops.call_operation(impl->address, impl->name, eax);
        break;
      case 3:
        ops.load_next_opcode(eax);
        ops.load_next_opcode(ecx);
        ops.call_operation(impl->address, impl->name, eax, ecx);
        break;
      default:
        std::cout << "Invalid width '" << width << "' for instruction '" <<
          op << "'\n";
        abort();
      }
#endif

      cache_stack();

#if 0
      instructions::Status status = instructions::check_status(op);
      if(status == instructions::MightReturn) {
        a.cmp(eax, cExecuteRestart);
        a.jump_if_equal(fin);
      } else if(status == instructions::Terminate) {
        a.jump(fin);
      }
#endif
      break;
    }
    }
  }

  // Generate a function to interprete the bytecode of a method.
  // Fixed registers:
  //   ebx: the stack top. Sync'd with the context when needed
  //   edi: the opcodes array. Pulled out at the top only.
  //
  // Specialty registers:
  //   esi: Holds either the context or the task. Loaded as needed
  //        by operations methods.
  //
  // Scratch registers:
  //   eax, ecx, edx
  //
  void** JITCompiler::create_interpreter(STATE) {
    // A label for each operation
    std::vector<AssemblerX86::NearJumpLocation> labels(InstructionSequence::cTotal);

    // The lookup table to use
    void** table = new void*[InstructionSequence::cTotal];

    AssemblerX86::NearJumpLocation fin;

    ops.prologue();
    cache_stack();
    ops.load_opcodes(edi);
    ops.load_and_increment_ip(eax);
    ops.load_opcode(eax, edi, eax);
    a.jump_via_table(table, eax);

    for(opcode i = 0; i < InstructionSequence::cTotal; i++) {
      a.set_label(labels[i]);

      // ops.call_operation(reinterpret_cast<void*>(show_info2), "show_info2", ebx);

      emit_opcode(i, fin);
      ops.load_and_increment_ip(eax);
      ops.load_opcode(eax, edi, eax);
      a.jump_via_table(table, eax);
    }

    ops.reset_usage();
    a.set_label(fin);
    uncache_stack();
    ops.epilogue();

    // Fill out table now
    for(opcode i = 0; i < InstructionSequence::cTotal; i++) {
      table[i] = labels[i].destination();
    }

    return table;
  }
}
