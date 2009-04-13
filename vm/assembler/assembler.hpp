#ifndef RBX_ASSEMBLER
#define RBX_ASSEMBLER

#include <stdint.h>
#include "field_offset.hpp"

namespace assembler {
  class Assembler {
  protected:
    uint8_t* buffer_;
    uint8_t* pc_;
    bool     own_buffer_;

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
    Assembler() : own_buffer_(true) {
      buffer_ = new uint8_t[1024];
      pc_ = buffer_;
    }

    Assembler(uint8_t* buffer)
      : buffer_(buffer)
      , pc_(buffer)
      , own_buffer_(false)
    { }

    ~Assembler() {
      if(own_buffer_) {
        delete[] buffer_;
      }
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
