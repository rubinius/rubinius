namespace rubinius {
  class OpcodeIterator {
    VMMethod* vmm_;

    opcode ip_;
    int width_;

    opcode op_;
    const char* name_;

  private:
    void update() {
      op_ = vmm_->opcodes[ip_];

      switch(op_) {
#define HANDLE_INST0(code, name) \
      case code: \
        name_ = #name; \
        width_ = 1; \
        break;
#define HANDLE_INST1(code, name) \
      case code: \
        name_ = #name; \
        width_ = 2; \
        break;
#define HANDLE_INST2(code, name) \
      case code: \
        name_ = #name; \
        width_ = 3; \
        break;
#include "gen/instruction_visitors.hpp"
#undef HANDLE_INST0
#undef HANDLE_INST1
#undef HANDLE_INST2

      default:
        rubinius::bug("Unknown opcode");
      }
    }


#include "gen/instruction_names.hpp"

  public:
    OpcodeIterator(VMMethod* vmm, int ip=0)
      : vmm_(vmm)
      , ip_(ip)
      , width_(0)
    {
      update();
    }

    opcode* stream() {
      return vmm_->opcodes;
    }

    opcode ip() {
      return ip_;
    }

    int next_ip() {
      return ip_ + width_;
    }

    bool next_p() {
      return ip_ < vmm_->total;
    }

    void switch_to(int ip) {
      ip_ = ip;
      update();
    }

    void advance() {
      switch_to(next_ip());
    }

    opcode op() {
      return op_;
    }

    const char* name() {
      return name_;
    }

    int operands() {
      return width_ - 1;
    }

    opcode operand(int which) {
      return vmm_->opcodes[ip_ + which + 1];
    }

    // flags
    bool terminator_p() {
      switch(op_) {
      case insn_ret:
      case insn_goto:
      case insn_raise_exc:
      case insn_raise_return:
      case insn_raise_break:
      case insn_ensure_return:
      case insn_reraise:
        return true;
      default:
        return false;
      }
    }

    bool goto_p() {
      switch(op_) {
      case insn_goto:
      case insn_goto_if_true:
      case insn_goto_if_false:
      case insn_setup_unwind:
        return true;
      default:
        return false;
      }
    }

    opcode goto_target() {
      // All gotos use operand 0 as the target
      return operand(0);
    }

  };
}
