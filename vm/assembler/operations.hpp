#ifndef RBX_ASM_OPERATIONS
#define RBX_ASM_OPERATIONS

#include "assembler_x86.hpp"
#include "oop.hpp"
#include "jit_state.h"
#include "builtin/tuple.hpp"
#include "call_frame.hpp"

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
      return a.address(sp, -(which * EntryWidth));
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
      a.mov(dst, a.address(sp, -(which * EntryWidth)));
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
      s.assembler().bit_and(scratch, s.position(1));
      s.assembler().test(scratch, TAG_FIXNUM);
      s.assembler().jump_if_not_equal(are_not);
    }

    void load_mc() {
      if(esi_usage != Arg2) {
        s.assembler().load_arg(esi, 2);
        esi_usage = Arg2;
      }
    }

    void load_stack_pointer() {
      load_mc();

      // Pull jit_state.stack into the stack pointer
      s.assembler().mov(s.stack_pointer(), s.assembler().address(esi,
        FIELD_OFFSET(rubinius::InterpreterCallFrame, js.stack)));
    }

    void save_stack_pointer() {
      load_mc();

      // Mov the stack pointer into  jit_state.stack
      s.assembler().mov(
          s.assembler().address(esi,
            FIELD_OFFSET(rubinius::InterpreterCallFrame, js.stack)),
          s.stack_pointer());
    }

    void prologue() {
      AssemblerX86& a = s.assembler();

      // Do the normal prologue
      a.prologue(0);

      // Interpreter state
      a.push(0);
      a.push(0);
      a.mov(eax, esp);
      // seed the stack with the operation arguments so we don't have
      // to push them over and over again
      //
      // First, add some space for 2 extra args
      // This seems like a lot, but we need space for at least 2 AND
      // we have to keep the stack aligned properly.
      a.sub(esp, 0x08);

      a.push(eax);
      // Now push the incoming arguments on
      a.push_arg(2);
      a.push_arg(1);
      a.push_arg(0);
    }

    void epilogue() {
      AssemblerX86::NearJumpLocation fail;
      // Remove the spots for 8 args we setup in prologue
      s.assembler().add(esp, 0x20);

      // Do the normal epilogue too
      s.assembler().epilogue();
    }

    // Attempts to call +func+ as a symbol.
    void call_via_symbol(void* func) {
      // Who cares about func name?
      // Dl_info info;
      // if(!dladdr(func, &info)) {
      //   throw std::runtime_error("Unable to find symbol");
      // }
      //
      // if(!info.dli_sname || info.dli_saddr != func) {
      //   throw std::runtime_error("Unable to resolve symbol properly");
      // }

      // s.assembler().call(func, info.dli_sname);
      s.assembler().call(func);
    }

    void call_operation(void* func, const char* name) {
      s.assembler().call(func, name);
    }

    void call_operation(void* func, const char* name, int arg) {
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esp, 16), arg);
      a.call(func, name);
    }

    void call_operation(void* func, const char* name, Register& arg) {
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esp, 16), arg);
      a.call(func, name);
    }

    void call_operation(void* func, const char* name, int arg, int arg2) {
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esp, 16), arg);
      a.mov(a.address(esp, 20), arg2);
      a.call(func, name);
    }

    void call_operation(void* func, const char* name, Register& arg, Register& arg2) {
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esp, 16), arg);
      a.mov(a.address(esp, 20), arg2);
      a.call(func, name);
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
      store_mc_field(reg, FIELD_OFFSET(rubinius::CallFrame, ip));
    }

    void store_native_ip(Register& reg) {
      // store_mc_field(reg, FIELD_OFFSET(rubinius::CallFrame, native_ip));
    }

    void load_native_ip(Register& reg) {
      // load_mc_field(reg, FIELD_OFFSET(rubinius::CallFrame, native_ip));
    }

    void store_ip(Register& virtual_ip, Register& native_ip) {
      load_mc();
      AssemblerX86 &a = s.assembler();
      a.mov(a.address(esi, FIELD_OFFSET(rubinius::CallFrame, ip)), virtual_ip);
      // a.mov(a.address(esi, FIELD_OFFSET(rubinius::CallFrame, native_ip)), native_ip);
    }

    void load_and_increment_ip(Register& reg) {
      load_mc();
      AssemblerX86 &a = s.assembler();
      a.mov(reg, a.address(esi, FIELD_OFFSET(rubinius::CallFrame, ip)));
      a.add(a.address(esi, FIELD_OFFSET(rubinius::CallFrame, ip)), 1);
    }

    void load_opcodes(Register& reg) {
      load_mc();
      AssemblerX86 &a = s.assembler();
      a.load_arg(reg, 0);
      a.mov(reg, a.address(reg, FIELD_OFFSET(rubinius::VMMethod, opcodes)));
    }

    void load_opcode(Register& dest, Register& table, Register& index) {
      AssemblerX86 &a = s.assembler();
      a.mov_scaled(dest, table, index, sizeof(int));
    }

    void load_next_opcode(Register& dest) {
      load_and_increment_ip(dest);
      load_opcode(dest, edi, dest);
    }

    void load_self(Register& reg) {
      AssemblerX86 &a = s.assembler();
      load_call_frame(reg);
      a.mov(reg, a.address(reg, FIELD_OFFSET(rubinius::CallFrame, scope)));
      a.mov(reg, a.address(reg, FIELD_OFFSET(rubinius::VariableScope, self_)));
    }

    void load_call_frame(Register& reg) {
      s.assembler().load_arg(reg, 2);
    }

    void load_literals(Register& reg) {
      AssemblerX86 &a = s.assembler();
      load_call_frame(reg);
      a.mov(reg, a.address(reg, FIELD_OFFSET(rubinius::CallFrame, cm)));
      a.mov(reg, a.address(reg, FIELD_OFFSET(rubinius::CompiledMethod, literals_)));
    }

    void set_local(Register& val, int which) {
      load_call_frame(eax);
      AssemblerX86 &a = s.assembler();
      int base =   FIELD_OFFSET(rubinius::VariableScope, locals_);
      int offset = which * sizeof(void*);
      //a.mov(eax, a.address(eax, FIELD_OFFSET(rubinius::CallFrame, top_scope)));
      a.mov(a.address(eax, base + offset), val);
    }

    void set_local(Register& val, Register& which) {
      load_call_frame(eax);
      AssemblerX86 &a = s.assembler();
      int base =   FIELD_OFFSET(rubinius::VariableScope, locals_);
      // a.mov(eax, a.address(eax, FIELD_OFFSET(rubinius::CallFrame, top_scope)));
      a.mov_to_table(eax, which, sizeof(void*), base, val);
    }

    void get_local(Register& val, int which) {
      load_call_frame(eax);
      AssemblerX86 &a = s.assembler();
      int base =   FIELD_OFFSET(rubinius::VariableScope, locals_);
      int offset = which * sizeof(void*);
      // a.mov(eax, a.address(eax, FIELD_OFFSET(rubinius::CallFrame, top_scope)));
      a.mov(val, a.address(eax, base + offset));
    }

    void get_local(Register& val, Register& which) {
      load_call_frame(eax);
      AssemblerX86 &a = s.assembler();
      int base =   FIELD_OFFSET(rubinius::VariableScope, locals_);
      // a.mov(eax, a.address(eax, FIELD_OFFSET(rubinius::CallFrame, top_scope)));
      a.mov_from_table(val, eax, which, sizeof(void*), base);
    }

    void get_literal(Register& dst, int which) {
      load_literals(dst);

      int base =   FIELD_OFFSET(rubinius::Tuple, field);
      int offset = which * sizeof(void*);
      AssemblerX86 &a = s.assembler();
      a.mov(dst, a.address(dst, base + offset));
    }

    void get_literal(Register& dst, Register& which) {
      load_literals(dst);

      int base =   FIELD_OFFSET(rubinius::Tuple, field);
      AssemblerX86 &a = s.assembler();
      a.mov_from_table(dst, dst, which, sizeof(void*), base);
    }

    void tag_fixnum(Register& val) {
      AssemblerX86 &a = s.assembler();
      a.shift_left(val, 1);
      a.bit_or(val, 1);
    }

    void untag_fixnum(Register& val) {
      s.assembler().shift_right(val, 1);
    }

    /** @todo Fix to use InterpreterState's call flags, Task is gone --rue */
    void store_call_flags(int val) {
//      AssemblerX86 &a = s.assembler();
//      a.load_arg(eax, 1);
//      a.mov(a.address(eax, FIELD_OFFSET(rubinius::Task, call_flags)), val);
    }

    /** @todo Fix to use InterpreterState's call flags, Task is gone --rue */
    void store_call_flags(Register& val) {
//      AssemblerX86 &a = s.assembler();
//      a.load_arg(eax, 1);
//      a.mov(a.address(eax, FIELD_OFFSET(rubinius::Task, call_flags)), val);
    }

    // If the value in +reg+ is what ruby calls true (not false or nil),
    // jump to +lbl+
    void jump_if_true(Register& reg, AssemblerX86::NearJumpLocation& lbl) {
      AssemblerX86 &a = s.assembler();
      a.bit_and(reg, FALSE_MASK); // the common mask for both nil and false
      a.cmp(reg, rubinius::cFalse);     // both nil and false have Qfalse in the low 3 bits
      a.jump_if_not_equal(lbl);
    }

    // If the value in +reg+ is what ruby calls false (false or nil),
    // jump to +lbl+
    void jump_if_false(Register& reg, AssemblerX86::NearJumpLocation& lbl) {
      AssemblerX86 &a = s.assembler();
      a.bit_and(reg, FALSE_MASK); // common mask for both nil and false
      a.cmp(reg, rubinius::cFalse);    // both nil and false have Qfalse in the low 3 bits
      a.jump_if_equal(lbl);
    }

  };
}

#endif
