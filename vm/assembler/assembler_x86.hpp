#ifndef RBX_ASSEMBLER_X86
#define RBX_ASSEMBLER_X86

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <iomanip>

#include <stdint.h>
#include <stdio.h>
#include <dlfcn.h>

#include "assembler/assembler.hpp"
#include <udis86.h>
#include "assembler/relocation.hpp"
#include "assembler/code_map.hpp"

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

    bool operator==(Register& other) {
      return code() == other.code();
    }
  };

  // Set in assembler_x86.cpp so they're not defined multiple times.
  extern Register eax;
  extern Register ecx;
  extern Register edx;
  extern Register ebx;
  extern Register esp;
  extern Register ebp;
  extern Register esi;
  extern Register edi;
  extern Register no_reg;
  extern Register no_base;

  class AssemblerX86 : public assembler::Assembler {
  public: // Types
    // See http://wiki.osdev.org/X86_Instruction_Encoding
    // for info on how the mod bits are interpretted
    enum ModType {
      ModNone = 0,
      ModAddr2Reg = 1,
      ModReg2Addr = 2,
      ModReg2Reg = 3,

      Mod8Displacement = 1,
      Mod32Displacement = 2,
      ModXMM = 3,

      Mod2 = 2,
      Mod3 = 3
    };

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


  private:

    assembler::Relocations relocations_;

    const static int AddOperation = 0;
    const static int SubOperation = 5;
    const static int CompareOperation = 7;

    void emit_math(int operation, Register& reg, int val) {
      if(val < 128 && val >= -128) {
        emit(0x83);
        emit_modrm(ModReg2Reg, operation, reg.code());
        emit((int8_t)val);
      } else {
        emit(0x81);
        emit_modrm(ModReg2Reg, operation, reg.code());
        emit_w(val);
      }
    }

    void emit_modrm(ModType mod, int reg, int rm) {
      emit(((int)mod << 6) | (reg & 0x7) << 3 | (rm & 0x7));
    }

    void emit_modrm(const Address& addr, int reg, int rm) {
      ModType mod;
      int displacement = addr.offset();

      if(displacement < 128 && displacement >= -128) {
        mod = Mod8Displacement;
      } else {
        mod = Mod32Displacement;
      }

      emit(((int)mod << 6) | (reg & 0x7) << 3 | (rm & 0x7));
    }

    void emit_displacement(const Address& addr) {
      int displacement = addr.offset();
      if(displacement < 128 && displacement >= -128) {
        // Be sure to cast so that the sign is extended properly.
        emit((int8_t)displacement);
      } else {
        emit_w(displacement);
      }

    }

  public:
    AssemblerX86() : Assembler() { }

    AssemblerX86(uint8_t* buffer) : Assembler(buffer) { }

    AssemblerX86(uint8_t* buffer, AssemblerX86 &other)
      : Assembler(buffer)
    {
      std::memcpy(buffer, other.buffer(), other.used_bytes());
      for(assembler::Relocations::iterator i = other.relocations_.begin();
          i != other.relocations_.end();
          i++) {
        assembler::Relocation* rel = new assembler::Relocation(*i->second);
        rel->adjust_base(other.buffer(), buffer);
        relocations_[rel->instruction_location()] = rel;

        rel->resolve_and_write();
      }

      pc_ = buffer + other.used_bytes();
    }

    ~AssemblerX86() {
      for(assembler::Relocations::iterator i = relocations_.begin();
          i != relocations_.end();
          i++) {
        delete i->second;
      }
    }

    Address address(Register &r, int o = 0) {
      return Address(r, o);
    }

    // Relocation
    assembler::Relocation* find_relocation(void* address) {
      return relocations_[address];
    }

    void add_relocation(void* addr, assembler::Relocation* rel) {
      relocations_[addr] = rel;
    }

    assembler::Relocations& relocations() {
      return relocations_;
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

      // esp is a special case, so we have to use the SIB extension
      // to deref via esp directly.
      if(addr.base() == esp) {
        // 4 means use SIB, so we have to use it.
        emit_modrm(addr, 0, 4);
        // esp.code() means ignore the index.
        emit_modrm(ModNone, esp.code(), addr.base().code());
      } else {
        emit_modrm(addr, 0, addr.base().code());
      }

      emit_displacement(addr);
      emit_w(val);
    }

    void mov(Register &dst, Register &src) {
      emit(0x8b);
      emit_modrm(ModReg2Reg, dst.code(), src.code());
    }

    void mov(Register &dst, const Address addr) {
      emit(0x8b);

      emit_modrm(addr, dst.code(), addr.base().code());
      emit_displacement(addr);
    }

    void mov_scaled(Register& dst, Register& base, Register& index, int scale) {
      emit(0x8b);
      emit_modrm(ModNone, dst.code(), esp.code());
      emit_modrm(scale == 4 ? Mod2 : Mod3, index.code(), base.code());
    }

    void mov_to_table(Register& base, Register& index, int scale, int disp, Register& val) {
      emit(0x89);
      emit_modrm(Mod2, val.code(), esp.code());
      emit_modrm(scale == 4 ? Mod2 : Mod3, index.code(), base.code());
      emit_w(disp);
    }

    void mov_from_table(Register& val, Register& base, Register& index,
                        int scale, int disp) {
      emit(0x8b);
      emit_modrm(Mod2, val.code(), esp.code());
      emit_modrm(scale == 4 ? Mod2 : Mod3, index.code(), base.code());
      emit_w(disp);
    }

    void mov(const Address addr, Register &val) {
      emit(0x89);

      // esp is a special case, so we have to use the SIB extension
      // to deref via esp directly.
      if(addr.base() == esp) {
        // 4 means use SIB, so we have to use it.
        emit_modrm(addr, val.code(), 4);
        // 0 means no scale
        // esp.code() means ignore the index.
        emit_modrm(ModNone, esp.code(), addr.base().code());
      } else {
        emit_modrm(addr, val.code(), addr.base().code());
      }

      emit_displacement(addr);
    }


    // Sets up a mov instruction of an immediate value to register.
    // The immediate value is initialized to 0, and it's location
    // in memory is returned so it can be updated directly.
    void mov_delayed(Register &reg, uintptr_t** loc) {
      emit(0xb8 | reg.code());
      *loc = (uintptr_t*)pc_;
      emit_w(0);
    }

    void push(Register &reg) {
      emit(0x50 | reg.code());
    }

    void push(uint32_t val) {
      emit(0x68);
      emit_w(val);
    }

    void push(const Address addr) {
      emit(0xff);

      emit_modrm(addr, 6, addr.base().code());
      emit_displacement(addr);
    }

    void pop(Register &dst) {
      emit(0x58 | dst.code());
    }

    void lea(Register &dest, Register &base, int offset) {
      emit(0x8d);
      emit_modrm(Mod32Displacement, dest.code(), base.code());
      emit_w(offset);
    }

    void nop() {
      emit(0x90);
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
      emit_modrm(ModReg2Reg, dst.code(), src.code());
    }

    void sub(Register &dst, Register &src) {
      emit(0x2b);
      emit_modrm(ModReg2Reg, dst.code(), src.code());
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
      emit_modrm(ModReg2Reg, lhs.code(), rhs.code());
    }

    void cmp(const Address addr, int val) {
      emit(0x81);

      emit_modrm(addr, CompareOperation, addr.base().code());
      emit_displacement(addr);
      emit_w(val);
    }

    void shift_right(Register &reg, int count) {
      emit(0xc1);
      emit_modrm(ModReg2Reg, 7, reg.code());
      emit(count);
    }

    void shift_left(Register &reg, int count) {
      emit(0xc1);
      emit_modrm(ModReg2Reg, 4, reg.code());
      emit(count);
    }

    void bit_or(Register &reg, int val) {
      emit(0x81);
      emit_modrm(ModReg2Reg, 1, reg.code());
      emit_w(val);
    }

    void bit_or(Register &dst, const Address addr) {
      emit(0x0b);

      emit_modrm(addr, dst.code(), addr.base().code());
      emit_displacement(addr);
    }

    void bit_and(Register &reg, int val) {
      emit(0x81);
      emit_modrm(ModReg2Reg, 4, reg.code());
      emit_w(val);
    }

    void bit_and(Register &dst, const Address addr) {
      emit(0x23);

      emit_modrm(addr, dst.code(), addr.base().code());
      emit_displacement(addr);
    }

    void bit_and(Register &dst, Register &reg) {
      emit(0x23);
      emit_modrm(ModReg2Reg, dst.code(), reg.code());
    }

    // Testing

    void test(Register &lhs, Register &rhs) {
      emit(0x85);
      emit_modrm(ModReg2Reg, rhs.code(), lhs.code());
    }

    void test(Register &reg, const int val) {
      emit(0xf7);
      emit_modrm(ModReg2Reg, 0, reg.code());
      emit_w(val);
    }

    // Calling

    void call(Register &reg) {
      emit(0xff);
      emit_modrm(ModReg2Reg, 2, reg.code());
    }

    void call(void* func) {
      emit(0xe8);
      void* position = (void*)pc_;
      emit_w(0); // save the space

      assembler::Relocation* rel = new assembler::Relocation(
          assembler::Relocation::Relative, position, func);
      relocations_[position] = rel;

      rel->write();
    }

    // Used for calling to a symbol that may need to be fixed up
    // via dlsym() later.
    void call(void* func, const char* name) {
      emit(0xe8);
      void* position = (void*)pc_;
      emit_w(0); // save the space

      assembler::Relocation* rel = new assembler::Relocation(
          assembler::Relocation::Relative, position, func);
      rel->references_symbol(name);

      relocations_[position] = rel;

      rel->write();

    }

    // Jump

    typedef std::vector<uint8_t*> Locations;

    class NearJumpLocation {
      Locations *fixups_;
      uint8_t *destination_;
      uint32_t flags_;

    public:

      NearJumpLocation() : fixups_(0), destination_(0), flags_(0) { }

      Locations& fixups() {
        return *fixups_;
      }

      uint32_t& flags() {
        return flags_;
      }

      void set_destination(uint8_t *dest) {
        destination_ = dest;

        if(!fixups_) return;

        for(Locations::iterator i = fixups_->begin();
            i != fixups_->end();
            i++) {
          *reinterpret_cast<uint32_t*>(*i) = dest - (*i + 4);
        }

        // We're done with the fixups, get rid of them.
        delete fixups_;
        fixups_ = 0;
      }

      uint8_t *destination() {
        return destination_;
      }

      bool bound_p() {
        return destination_ != 0;
      }

      int operand(uint8_t* location) {
        if(!bound_p()) {
          if(!fixups_) fixups_ = new Locations;
          fixups_->push_back(location);
          return 0;
        }
        // The 4 here is because thats the size of the operand
        // is 4 bytes and  EIP points to the next instruction
        // when jump runs
        return destination_ - (location + 4);
      }
    };

    void jump(NearJumpLocation& loc) {
      emit(0xe9);
      emit_w(loc.operand(pc_));
    }

    void jump(Register &reg) {
      emit(0xff);
      emit_modrm(ModReg2Reg, 4, reg.code());
    }

    void jump_via_table(void** table, Register& index) {
      emit(0xff);
      // 0 == No table register
      // 4 == /4 is the spec for the jump instruction
      // 4 == Use SIB byte
      emit_modrm(ModNone, 4, 4);
      ModType scale = (sizeof(void*) == 4) ? Mod2 : Mod3;

      // scale == how much to multiple the index by
      // index == what register to use as the table index
      // 5     == add displacement
      emit_modrm(scale, index.code(), 5);
      emit_w(reinterpret_cast<uintptr_t>(table));
    }

    void jump(void* address) {
      emit(0xe9);
      void* position = (void*)pc_;
      emit_w(0); // save the space

      assembler::Relocation* rel = new assembler::Relocation(
          assembler::Relocation::Relative, position, address);
      relocations_[position] = rel;

      rel->write();
    }

    void jump_if_equal(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x84);
      emit_w(loc.operand(pc_));
    }

    void jump_if_not_equal(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x85);
      emit_w(loc.operand(pc_));
    }

    void jump_if_greater(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x8f);
      emit_w(loc.operand(pc_));
    }

    void jump_if_less(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x8c);
      emit_w(loc.operand(pc_));
    }

    void jump_if_overflow(NearJumpLocation& loc) {
      emit(0x0f);
      emit(0x80);
      emit_w(loc.operand(pc_));
    }

    void set_label(NearJumpLocation& loc) {
      loc.set_destination(pc_);
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
    const static int EpilogueSize = 5;
    void epilogue() {
      pop(ebx);
      pop(esi);
      pop(edi);

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

    void push_arg(int which) {
      push(address(ebp, 8 + (which * 4)));
    }

    // Disassembling
    void show();
    static void show_buffer(void* buffer, size_t size, bool show_hex = false,
        rubinius::AddressComments* comments = NULL);
    ud_t* disassemble();
    void show_relocations();
  };

}

#endif
