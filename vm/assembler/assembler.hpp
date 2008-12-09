#ifndef RBX_ASSEMBLER
#define RBX_ASSEMBLER

#include <stdint.h>

// Stolen from Squirrelfish Extreme's JIT.cpp
// FIELD_OFFSET: Like the C++ offsetof macro, but you can use it with classes.
// The magic number 0x4000 is insignificant. We use it to avoid using NULL, since
// NULL can cause compiler problems, especially in cases of multiple inheritance.
#ifdef FIELD_OFFSET
#undef FIELD_OFFSET // Fix conflict with winnt.h.
#endif
#define FIELD_OFFSET(class, field) (reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<class*>(0x4000)->field)) - 0x4000)

namespace assembler {
  class Assembler {
  protected:
    uint8_t* buffer_;
    uint8_t* pc_;

    void emit(uint8_t val) {
      *reinterpret_cast<uint8_t*>(pc_) = val;
      pc_ += 1;
    }

    void emit_s(uint16_t val) {
      *reinterpret_cast<uint16_t*>(pc_) = val;
      pc_ += 2;
    }

    void emit_w(uint32_t val) {
      *reinterpret_cast<uint32_t*>(pc_) = val;
      pc_ += 4;
    }

    void emit_at_w(uint8_t* pc, uint32_t val) {
      *reinterpret_cast<uint32_t*>(pc) = val;
    }

    void emit_dw(uint64_t val) {
      *reinterpret_cast<uint64_t*>(pc_) = val;
      pc_ += 8;
    }

  public:
    Assembler() {
      buffer_ = new uint8_t[1024];
      pc_ = buffer_;
    }

    Assembler(uint8_t* buffer)
      : buffer_(buffer)
      , pc_(buffer)
    { }

    ~Assembler() {
      delete buffer_;
    }

    void* buffer() {
      return (void*)buffer_;
    }

    void* pc() {
      return (void*)pc_;
    }

    int used_bytes() {
      return pc_ - buffer_;
    }
  };
}

#endif
