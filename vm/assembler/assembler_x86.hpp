#ifndef RBX_ASSEMBLER_X86
#define RBX_ASSEMBLER_X86

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>

#include <stdint.h>
#include <stdio.h>
#include <dlfcn.h>

#include "assembler.hpp"
#include "udis86.h"

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

  class Relocation {
  public: // Types
    enum Kind {
      Relative,
      LocalAbsolute
    };

    enum TargetKind {
      Absolute,
      Symbol
    };

  private:
    Kind  kind_;
    void* instruction_location_;
    void* address_;
    int   offset_;
    TargetKind target_kind_;
    std::string* symbol_;

  public:

    Relocation(Kind kind, void* insn, void* address, int offset = 4)
      : kind_(kind)
      , instruction_location_(insn)
      , address_(address)
      , offset_(offset)
      , target_kind_(Absolute)
      , symbol_(0)
    { }

    Kind kind() {
      return kind_;
    }

    TargetKind target_kind() {
      return target_kind_;
    }

    void* instruction_location() {
      return instruction_location_;
    }

    void* address() {
      return address_;
    }

    intptr_t value() {
      intptr_t insn = reinterpret_cast<intptr_t>(instruction_location_);
      intptr_t val = reinterpret_cast<intptr_t>(address_);
      if(kind_ == Relative) {
        val -= (insn + offset_);
      }

      return val;
    }

    // Write the relocation out to memory
    void write() {
      intptr_t* write_to = reinterpret_cast<intptr_t*>(instruction_location_);
      *write_to = value();
    }

    void references_symbol(const char* name) {
      if(symbol_) delete symbol_;
      symbol_ = new std::string(name);
      target_kind_ = Symbol;
    }

    std::string& symbol() {
      return *symbol_;
    }

    static void* resolve_symbol(std::string& str) {
      return dlsym(RTLD_DEFAULT, str.c_str());
    }

    void resolve_and_write() {
      if(target_kind_ == Symbol) {
        if(!symbol_) {
          std::cout << "Symbol relocation setup, but no symbol present!\n";
          return;
        }

        void* new_addr = Relocation::resolve_symbol(*symbol_);
        if(new_addr) {
          address_ = new_addr;
        } else {
          std::cout << "Unable to resolve symbol '" << *symbol_ << "': "
                    << dlerror() << "\n";
        }
      }

      write();
    }

    void adjust_base(void* old_base, void* new_base) {
      intptr_t ob = reinterpret_cast<intptr_t>(old_base);
      intptr_t nb = reinterpret_cast<intptr_t>(new_base);

      instruction_location_ = reinterpret_cast<void*>(
        nb + (reinterpret_cast<intptr_t>(instruction_location_) - ob));

      if(kind_ == Relocation::LocalAbsolute) {
        intptr_t diff = reinterpret_cast<intptr_t>(address_) - ob;
        address_ = reinterpret_cast<void*>(nb + diff);
      }
    }
  };

  typedef std::map<void*, Relocation*> Relocations;

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
      ModXMM = 3
    };

  private:

    Relocations relocations_;

    const static int AddOperation = 0;
    const static int SubOperation = 5;
    const static int CompareOperation = 7;

    void emit_math(int operation, Register& reg, int val) {
      emit(0x81);
      emit_modrm(ModReg2Reg, operation, reg.code());
      emit_w(val);
    }

    void emit_modrm(ModType mod, int reg, int rm) {
      emit(((int)mod << 6) | (reg & 0x7) << 3 | (rm & 0x7));
    }

  public:
    AssemblerX86() : Assembler() { }
    AssemblerX86(uint8_t* buffer, AssemblerX86 &other)
      : Assembler(buffer)
    {
      std::memcpy(buffer, other.buffer(), other.used_bytes());
      for(Relocations::iterator i = other.relocations_.begin();
          i != other.relocations_.end();
          i++) {
        Relocation* rel = new Relocation(*i->second);
        rel->adjust_base(other.buffer(), buffer);
        relocations_[rel->instruction_location()] = rel;

        rel->resolve_and_write();
      }

      pc_ = buffer + other.used_bytes();
    }

    ~AssemblerX86() {
      for(Relocations::iterator i = relocations_.begin();
          i != relocations_.end();
          i++) {
        delete i->second;
      }
    }

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

    // Relocation
    Relocation* find_relocation(void* address) {
      return relocations_[address];
    }

    void add_relocation(void* addr, Relocation* rel) {
      relocations_[addr] = rel;
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
        emit_modrm(Mod32Displacement, 0, 4);
        // esp.code() means ignore the index.
        emit(0 << 6 | esp.code() << 3 | addr.base().code());
      } else {
        emit_modrm(Mod32Displacement, 0, addr.base().code());
      }

      emit_w(addr.offset());
      emit_w(val);
    }

    void mov(Register &dst, Register &src) {
      emit(0x8b);
      emit_modrm(ModReg2Reg, dst.code(), src.code());
    }

    void mov(Register &dst, const Address addr) {
      emit(0x8b);
      emit_modrm(Mod32Displacement, dst.code(), addr.base().code());
      emit_w(addr.offset());
    }

    void mov(const Address addr, Register &val) {
      emit(0x89);
      // esp is a special case, so we have to use the SIB extension
      // to deref via esp directly.
      if(addr.base() == esp) {
        // 4 means use SIB, so we have to use it.
        emit_modrm(Mod32Displacement, val.code(), 4);
        // 0 means no scale
        // esp.code() means ignore the index.
        emit_modrm(ModNone, esp.code(), addr.base().code());
      } else {
        emit_modrm(Mod32Displacement, val.code(), addr.base().code());
      }
      emit_w(addr.offset());
    }

    // Sets up a mov instruction of an immediate value to register.
    // The immediate value is initialized to 0, and it's location
    // in memory is returned so it can be updated directly.
    void mov_delayed(Register &reg, uint32_t** loc) {
      emit(0xb8 | reg.code());
      *loc = (uint32_t*)pc_;
      emit_w(0);
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

      emit_modrm(Mod32Displacement, 6, addr.base().code());
      emit_w(addr.offset());
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
      emit_modrm(Mod32Displacement, CompareOperation, addr.base().code());
      emit_w(addr.offset());
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
      emit_modrm(Mod32Displacement, dst.code(), addr.base().code());
      emit_w(addr.offset());
    }

    void bit_and(Register &reg, int val) {
      emit(0x81);
      emit_modrm(ModReg2Reg, 4, reg.code());
      emit_w(val);
    }

    void bit_and(Register &dst, const Address addr) {
      emit(0x23);
      emit_modrm(Mod32Displacement, dst.code(), addr.base().code());
      emit_w(addr.offset());
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

      Relocation* rel = new Relocation(Relocation::Relative, position, func);
      relocations_[position] = rel;

      rel->write();
    }

    // Used for calling to a symbol that may need to be fixed up
    // via dlsym() later.
    void call(void* func, const char* name) {
      emit(0xe8);
      void* position = (void*)pc_;
      emit_w(0); // save the space

      Relocation* rel = new Relocation(Relocation::Relative, position, func);
      rel->references_symbol(name);

      relocations_[position] = rel;

      rel->write();

    }

    // Jump

    typedef std::vector<uint8_t*> Locations;

    class NearJumpLocation {
      Locations *fixups_;
      uint8_t *destination_;

    public:

      NearJumpLocation() : fixups_(0), destination_(0) { }

      Locations& fixups() {
        return *fixups_;
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

    void jump(void* address) {
      emit(0xe9);
      void* position = (void*)pc_;
      emit_w(0); // save the space

      Relocation* rel = new Relocation(Relocation::Relative, position, address);
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
    ud_t* disassemble();
    void show_relocations();
  };

}

#endif
