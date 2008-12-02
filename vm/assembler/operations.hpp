#ifndef RBX_ASM_OPERATIONS
#define RBX_ASM_OPERATIONS

#include "assembler_x86.hpp"
#include "oop.hpp"
#include "jit_state.h"
#include "builtin/contexts.hpp"
#include "builtin/task.hpp"

namespace operations {

  using namespace assembler_x86;

  class StackOperations {
    AssemblerX86 &a;
    Register &sp;

  public:
    const static int EntryWidth = 4;

    StackOperations(AssemblerX86 &a,
                    Register &reg)
      : a(a)
      , sp(reg) { }

    AssemblerX86& assembler() {
      return a;
    }

    Register& stack_pointer() {
      return sp;
    }

    AssemblerX86::Address position(int which) {
      return a.address(sp, which * EntryWidth);
    }

    void set_top(uint32_t val) {
      a.mov(a.address(sp, 0), val);
    }

    void set_top(Register &dst) {
      a.mov(a.address(sp, 0), dst);
    }

    void push(uint32_t val) {
      a.add(sp, EntryWidth);
      set_top(val);
    }

    void push(Register &dst) {
      a.add(sp, EntryWidth);
      set_top(dst);
    }

    void load_nth(Register &dst, int which) {
      a.mov(dst, a.address(sp, which * EntryWidth));
    }

    void clear(int count) {
      a.sub(sp, count * EntryWidth);
    }

    void pop() {
      clear(1);
    }
  };

  enum RegisterUsage {
    Unknown,
    Arg0,
    Arg1,
    Arg2,
    Arg3
  };

  class ObjectOperations {
    StackOperations &s;
    RegisterUsage esi_usage;

  public:
    ObjectOperations(StackOperations &s)
      : s(s)
      , esi_usage(Unknown)
    { }

    void reset_usage() {
      esi_usage = Unknown;
    }

    void check_both_fixnum(AssemblerX86::NearJumpLocation &are_not) {
      Register& scratch = ecx;

      s.load_nth(scratch, 0);
      s.assembler().bit_or(scratch, s.position(1));
      s.assembler().bit_and(scratch, TAG_MASK);
      s.assembler().cmp(scratch, TAG_FIXNUM);
      s.assembler().jump_if_not_equal(are_not);
    }

    void load_mc() {
      if(esi_usage != Arg2) s.assembler().load_arg(esi, 2);
      esi_usage = Arg2;
    }

    void load_stack_pointer() {
      load_mc();

      // Pull jit_state.stack into the stack pointer
      s.assembler().mov(s.stack_pointer(), s.assembler().address(esi,
        FIELD_OFFSET(rubinius::MethodContext, js.stack)));
    }

    void save_stack_pointer() {
      load_mc();

      // Mov the stack pointer into  jit_state.stack
      s.assembler().mov(
          s.assembler().address(esi,
            FIELD_OFFSET(rubinius::MethodContext, js.stack)),
          s.stack_pointer());
    }

    void prologue() {
      AssemblerX86& a = s.assembler();

      // Do the normal prologue
      a.prologue(0);

      // seed the stack with the operation arguments so we don't have
      // to push them over and over again
      //
      // First, add some space for 4 extra args
      a.sub(esp, 0x10);

      // Now push the incoming arguments on
      a.push_arg(3);
      a.push_arg(2);
      a.push_arg(1);
      a.push_arg(0);
    }

    void epilogue() {
      // Remove the spots for 8 args we setup in prologue
      s.assembler().add(esp, 0x20);

      // Do the normal epilogue too
      s.assembler().epilogue();
    }

    void call_operation(void* func) {
      s.assembler().call(func);
    }

    void call_operation(void* func, int arg) {
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esp, 24), arg);
      a.call(func);
    }

    void call_operation(void* func, int arg, int arg2) {
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esp, 24), arg);
      a.mov(a.address(esp, 28), arg2);
      a.call(func);
    }

    void store_mc_field(Register& reg, int pos) {
      load_mc();
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esi, pos), reg);
    }

    void load_mc_field(Register& reg, int pos) {
      load_mc();
      AssemblerX86 &a = s.assembler();
      a.mov(reg, a.address(esi, pos));
    }

    void store_virtual_ip(Register& reg) {
      store_mc_field(reg, FIELD_OFFSET(rubinius::MethodContext, ip));
    }

    void store_native_ip(Register& reg) {
      store_mc_field(reg, FIELD_OFFSET(rubinius::MethodContext, native_ip));
    }

    void load_native_ip(Register& reg) {
      load_mc_field(reg, FIELD_OFFSET(rubinius::MethodContext, native_ip));
    }

    void store_ip(Register& virtual_ip, Register& native_ip) {
      load_mc();
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esi, FIELD_OFFSET(rubinius::MethodContext, ip)), virtual_ip);
      a.mov(a.address(esi, FIELD_OFFSET(rubinius::MethodContext, native_ip)), native_ip);
    }

    void load_self(Register& reg) {
      load_mc_field(reg, FIELD_OFFSET(rubinius::MethodContext, self_));
    }

    void store_call_flags(int val) {
      AssemblerX86 &a = s.assembler();
      if(esi_usage != Arg1) a.load_arg(esi, 1);
      esi_usage = Arg1;
      a.mov(a.address(esi, FIELD_OFFSET(rubinius::Task, call_flags)), val);
    }

    // If the value in +reg+ is what ruby calls true (not false or nil),
    // jump to +lbl+
    void jump_if_true(Register& reg, AssemblerX86::NearJumpLocation& lbl) {
      AssemblerX86 &a = s.assembler();
      a.bit_and(reg, 0x7); // 0x7 is the common mask for both nil and false
      a.cmp(reg, 0x6);     // both nil and false have 0x6 in the low 3 bits
      a.jump_if_not_equal(lbl);
    }

    // If the value in +reg+ is what ruby calls false (false or nil),
    // jump to +lbl+
    void jump_if_false(Register& reg, AssemblerX86::NearJumpLocation& lbl) {
      AssemblerX86 &a = s.assembler();
      a.bit_and(reg, 0x7); // 0x7 is the common mask for both nil and false
      a.cmp(reg, 0x6);     // both nil and false have 0x6 in the low 3 bits
      a.jump_if_equal(lbl);
    }
  };
}

#endif
