#include "udis86.h"
#include <iostream>

#include <stdint.h>
#include <stdio.h>

#include "assembler.hpp"

namespace assembler_x86 {

  struct Register {
  public:
    int code_;

  public:
    void set_code(int code) {
      code_ = code;
    }

    int code() {
      return code_;
    }
  };

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


  class AssemblerX86 : public assembler::Assembler {
  private:
    // Emits the r/m operand code for transfering between
    // 2 registers
    void emit_rm_reg2reg(Register &dst, Register& src) {
      //  | mod   | reg             | r/m       |
      emit(3 << 6 | dst.code() << 3 | src.code());
    }

    void emit_rm_disp(Register &reg, int val) {
      //  | mod   | reg             | r/m|
      emit(0 << 6 | reg.code() << 3 | no_base.code());
      emit_w(val);
    }

    void emit_rm_reg2reg_offset(Register &dst, Register &src, int8_t offset) {
      //  | mod   | reg             | r/m |
      emit(1 << 6 | dst.code() << 3 | src.code());
      emit(offset);
    }

    const static int AddOperation = 0;
    const static int SubOperation = 5;
    const static int CompareOperation = 7;

    void emit_math(int operation, Register& reg, int val) {
      emit(0x81);
      Register op = { operation };
      emit_rm_reg2reg(op, reg);
      emit_w(val);
    }

  public:
    AssemblerX86() : Assembler() { }

    class Address {
      Register& base_;
      int offset_;

    public:

      Address(Register& r, int o) : base_(r), offset_(o) { }

      Register& base() const {
        return base_;
      }

      int offset() const {
        return offset_;
      }
    };

    Address address(Register &r, int o = 0) {
      return Address(r, o);
    }

    // Data movement

    // Used by mov_location()
    const static int MovWithRegisterWidth = 1;

    void mov(Register &reg, uint32_t val) {
      emit(0xb8 | reg.code());
      emit_w(val);
    }

    void mov(const Address addr, int val) {
      emit(0xc7);

      Register op = { 0 };

      //  | mod   | reg             | r/m|
      emit(2 << 6 | op.code() << 3 | addr.base().code());
      emit_w(addr.offset());
      emit_w(val);
    }

    void mov(Register &dst, Register &src) {
      emit(0x8b);
      emit_rm_reg2reg(dst, src);
    }

    void mov(Register &dst, const Address addr) {
      emit(0x8b);
      emit_rm_reg2reg_offset(dst, addr.base(), addr.offset());
    }

    void mov(const Address addr, Register &val) {
      emit(0x89);
      emit(2 << 6 | val.code() << 3 | addr.base().code());
      emit_w(addr.offset());
    }

    void push(Register &reg) {
      emit(0x50 | reg.code());
    }

    void push(uint32_t val) {
      emit(0x68);
      emit_w(val);
    }

    void push_address(void* addr) {
      push((uint32_t)addr);
    }

    void push(const Address addr) {
      emit(0xff);

      Register op = { 6 };

      emit(2 << 6 | op.code() << 3 | addr.base().code());
      emit_w(addr.offset());
    }

    void pop(Register &dst) {
      emit(0x58 | dst.code());
    }

    void lea(Register &dest, Register &base, int offset) {
      emit(0x8d);
      emit(2 << 6 | dest.code() << 3 | base.code());
      emit_w(offset);
    }

    // Function setup/teardown

    void leave() {
      emit(0xc9);
    }

    void ret() {
      emit(0xc3);
    }

    // Math

    void sub(Register &reg, int val) {
      emit_math(SubOperation, reg, val);
    }

    void add(Register &reg, int val) {
      emit_math(AddOperation, reg, val);
    }

    void add(const Address addr, int val) {
      emit(0x81);
      Register op = { 0 };

      //  | mod   | reg             | r/m|
      emit(2 << 6 | op.code() << 3 | addr.base().code());
      emit_w(addr.offset());
      emit_w(val);
    }

    void add(Register &dst, Register &src) {
      emit(0x03);
      emit_rm_reg2reg(dst, src);
    }

    void dec(Register &reg) {
      emit(0x48 | reg.code());
    }

    void inc(Register &reg) {
      emit(0x40 | reg.code());
    }

    void cmp(Register &reg, int val) {
      emit_math(CompareOperation, reg, val);
    }

    void cmp(Register &lhs, Register &rhs) {
      emit(0x3b);
      emit_rm_reg2reg(lhs, rhs);
    }

    void cmp(const Address addr, int val) {
      emit(0x81);
      Register op = { CompareOperation};
      emit_rm_reg2reg_offset(op, addr.base(), addr.offset());
      emit_w(val);
    }

    void shift_right(Register &reg, int count) {
      emit(0xc1);
      Register op = { 7 };
      emit_rm_reg2reg(op, reg);
      emit(count);
    }

    void shift_left(Register &reg, int count) {
      emit(0xc1);
      Register op = { 4 };
      emit_rm_reg2reg(op, reg);
      emit(count);
    }

    void bit_or(Register &reg, int val) {
      emit(0x81);
      Register op = { 1 };
      emit_rm_reg2reg(op, reg);
      emit_w(val);
    }

    // Testing

    void test(Register &lhs, Register &rhs) {
      emit(0x85);
      emit_rm_reg2reg(rhs, lhs);
    }

    void test(Register &reg, const int val) {
      emit(0xf7);
      Register op = { 0 };
      emit_rm_reg2reg(op, reg);
      emit_w(val);
    }

    // Calling

    void call(Register &reg) {
      emit(0xff);
      Register op = { 2 };
      emit_rm_reg2reg(op, reg);
    }

    void call(void* func) {
      emit(0xe8);
      uint8_t* addr = (uint8_t*)func;
      emit_w(addr - (pc_ + sizeof(uint32_t)));
    }

    // Jump

    class NearJumpLocation {
      uint8_t *pc_;
      uint8_t *destination_;

    public:

      NearJumpLocation() : pc_(0), destination_(0) { }

      void set_pc(uint8_t *pc) {
        pc_ = pc;
      }

      uint8_t* pc() { return pc_; }

      void set_destination(uint8_t *dest) {
        destination_ = dest;
      }

      uint8_t *destination() {
        return destination_;
      }

      int operand() {
        if(destination_ == 0) return 0;
        // The 4 here is because thats the size of the operand
        // is 4 bytes and  EIP points to the next instruction
        // when jump runs
        return destination_ - (pc_ + 4);
      }
    };

    void jump(NearJumpLocation& loc) {
      emit(0xe9);
      loc.set_pc(pc_);
      emit_w(loc.operand());
    }

    void jump(Register &reg) {
      emit(0xff);
      Register op = { 4 };
      emit_rm_reg2reg(op, reg);
    }

    void jump_if_equal(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x84);
      loc.set_pc(pc_);
      emit_w(loc.operand());
    }

    void jump_if_overflow(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x80);
      loc.set_pc(pc_);
      emit_w(loc.operand());
    }

    void set_label(NearJumpLocation& loc) {
      loc.set_destination(pc_);

      if(loc.pc() != NULL) {
        emit_at_w(loc.pc(), loc.operand());
      }
    }

    class InstructionDisplacement {
      uint8_t* start_;
      uint8_t* end_;
      uint8_t* replace_at_;

    public:
      InstructionDisplacement() : start_(0), end_(0), replace_at_(0) { }

      void set_start(uint8_t* start) {
        start_ = start;
      }

      uint8_t* start() {
        return start_;
      }

      void set_end(uint8_t* end) {
        end_ = end;
        *reinterpret_cast<uint32_t*>(replace_at_) = end_ - start_;
      }

      int difference() {
        return end_ - start_;
      }

      uint8_t* end() {
        return end_;
      }

      void set_replace_at(uint8_t* replace_at) {
        replace_at_ = replace_at;
      }

      uint8_t* replace_at() {
        return replace_at_;
      }
    };

    // A way to read and update eip without guesswork. The API for
    // InstructionDisplacement lets you calculate the number of bytes
    // to adjust eip after it's read so it points to instruction you want.
    void read_eip(Register& reg, InstructionDisplacement& loc) {
      // Emit a call with a destination offset of 0, ie, the next instruction.
      emit(0xe8);
      emit_w(0);

      loc.set_start(pc_);
      // The call puts eip on the stack, so pop it into the register
      pop(reg);
      // Push the value past pop and add instructions
      // Also adjust via the custom ammount passed in
      loc.set_replace_at(pc_ + 2); // 2 is where add's operand is

      // the 0 is a placeholder, which is updated when loc is update
      add(reg, 0);
    }

    void set_label(InstructionDisplacement& loc) {
      loc.set_end(pc_);
    }

    // Meta instructions

    class FuturePosition {
      uint8_t *replace_at_;

    public:
      FuturePosition() : replace_at_(0) { }

      void set_replace_at(uint8_t* rp) {
        replace_at_ = rp;
      }

      void update(void* loc) {
        *reinterpret_cast<uint32_t*>(replace_at_) = (uint32_t)loc;
      }
    };

    void mov(Register &reg, FuturePosition &pos) {
      pos.set_replace_at(pc_ + MovWithRegisterWidth);
      mov(eax, 0);
    }

    int prologue(int stack) {
      push(ebp);
      mov(ebp, esp);

      // the return address and our save of ebp are on the stack, so
      // adjust at least 8 more.
      if(stack <= 8) {
        stack = 8;
      } else {
        stack += 8;
        stack = (stack + 15) & ~15;
      }

      // Add an extra 4 so the push's to save the registers keeps things aligned
      stack += 4;

      sub(esp, stack);

      // Save the callee save registers.
      // After these push's, the stack is aligned because of the extra 4 above.
      push(edi);
      push(esi);
      push(ebx);

      return stack;
    }

    // bytes for the epilogue code, used with read_eip
    const static int EpilogueSize = 11;
    void epilogue(int stack) {
      pop(ebx);
      pop(esi);
      pop(edi);

      add(esp, stack);
      leave();
      ret();
    }

    // Pad the stack so each call site is at 16byte alignment
    void start_call(int args) {
      int bytes = args * sizeof(uint32_t);
      int mod = bytes % 16;
      if(mod != 0) {
        sub(esp, 16 - mod);
      }
    }

    // Remove the space allocated by the args from the stack
    void end_call(int args) {
      int bytes = args * sizeof(uint32_t);
      int mod = bytes % 16;
      add(esp, bytes + (16 - mod));
    }

    void load_arg(Register &reg, int which) {
      mov(reg, address(ebp, 8 + (which * 4)));
    }

    // Disassembling

    void show() {
      ud_t ud;

      ud_init(&ud);
      ud_set_mode(&ud, 32);
      ud_set_syntax(&ud, UD_SYN_ATT);
      ud_set_input_buffer(&ud, (uint8_t*)buffer_, pc_ - buffer_);

      while(ud_disassemble(&ud)) {
        std::cout << (void*)(reinterpret_cast<int>(buffer_) + ud_insn_off(&ud));
        std::cout << "  " << ud_insn_asm(&ud) << "\n";
      }
    }

    ud_t* disassemble() {
      ud_t *ud = new ud_t();
      ud_init(ud);
      ud_set_mode(ud, 32);
      ud_set_syntax(ud, UD_SYN_ATT);
      ud_set_input_buffer(ud, (uint8_t*)buffer_, pc_ - buffer_);
      ud_disassemble(ud);
      return ud;
    }
  };

}
