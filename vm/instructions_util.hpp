#ifndef RBX_INSTRUCTIONS_UTIL_HPP
#define RBX_INSTRUCTIONS_UTIL_HPP

#define SPECIFIC (static_cast<SubClass*>(this))

namespace rubinius {
  template <class SubClass>
    class VisitInstructions {
    protected:
      opcode* stream_;
      int size_;
      int next_ip_;

    public:

      void set_stream(opcode* stream, int size) {
        stream_ = stream;
        size_ = size;
      }

      void set_stream(VMMethod* vmm) {
        set_stream(vmm->opcodes, vmm->total);
      }

      void visit(opcode code, opcode arg1, opcode arg2) { }

#define HANDLE_INST0(code, name) \
      void visit_ ## name() { SPECIFIC->visit(code, -1, -1); }

#define HANDLE_INST1(code, name) \
      void visit_ ## name(opcode arg1) { SPECIFIC->visit(code, arg1, -1); }

#define HANDLE_INST2(code, name) \
      void visit_ ## name(opcode arg1, opcode arg2) { SPECIFIC->visit(code, arg1, arg2); }

#include "vm/gen/instruction_visitors.hpp"

#undef HANDLE_INST0
#undef HANDLE_INST1
#undef HANDLE_INST2

      void at_ip(int ip) { }
      bool before(opcode op, opcode arg1 = 0, opcode arg2 = 0) { return true; }

      int dispatch(int ip) {
        SPECIFIC->at_ip(ip);

        switch(stream_[ip]) {
#define HANDLE_INST0(code, name) \
        case code: \
                   next_ip_ = ip + 1; \
                   if(SPECIFIC->before(stream_[ip])) { \
                     SPECIFIC->visit_ ## name(); } return next_ip_;

#define HANDLE_INST1(code, name) \
        case code: \
                   next_ip_ = ip + 2; \
                   if(SPECIFIC->before(stream_[ip], stream_[ip + 1])) { \
                     SPECIFIC->visit_ ## name(stream_[ip + 1]); } return next_ip_;

#define HANDLE_INST2(code, name) \
        case code: \
                   next_ip_ = ip + 3; \
                   if(SPECIFIC->before(stream_[ip], stream_[ip + 1], stream_[ip + 2])) { \
                     SPECIFIC->visit_ ## name(stream_[ip + 1], stream_[ip + 2]); } return next_ip_;

#include "vm/gen/instruction_visitors.hpp"

#undef HANDLE_INST0
#undef HANDLE_INST1
#undef HANDLE_INST2

        default:
          abort();
          return -1;
        }
      }

      opcode next_op() {
        if(next_ip_ >= size_) return 0;
        return stream_[next_ip_];
      }

      opcode next_op_operand(size_t which) {
        if(next_ip_ >= size_) return 0;
        return stream_[next_ip_ + 1 + which];
      }

      static int opcode_size(opcode op) {
#undef HANDLE_INST0
#undef HANDLE_INST1
#undef HANDLE_INST2

        switch(op) {
#define HANDLE_INST0(code, name) case code: return 1;
#define HANDLE_INST1(code, name) case code: return 2;
#define HANDLE_INST2(code, name) case code: return 3;

#include "vm/gen/instruction_visitors.hpp"

#undef HANDLE_INST0
#undef HANDLE_INST1
#undef HANDLE_INST2
        }

        return -1;
      }

      void skip_next_op() {
        if(next_ip_ >= size_) return;
        opcode op = stream_[next_ip_];

        next_ip_ += opcode_size(op);
      }

      void drive(opcode* stream, int size, int start = 0) {
        set_stream(stream, size);

        int ip = start;
        while(ip < size) {
          ip = dispatch(ip);
        }
      }

      void drive(VMMethod* vmm) {
        drive(vmm->opcodes, vmm->total);
      }
    };
}

#endif
