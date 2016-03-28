namespace rubinius {
  class OpcodeIterator {
    MachineCode* machine_code_;

    opcode ip_;
    int width_;

    opcode op_;
    const char* name_;

  private:
    void update() {
      op_ = machine_code_->opcodes[ip_];

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
        throw LLVMState::CompileError("unknown opcode");
      }
    }


#include "gen/instruction_names.hpp"

  public:
    OpcodeIterator(MachineCode* mcode, int ip=0)
      : machine_code_(mcode)
      , ip_(ip)
      , width_(0)
    {
      update();
    }

    opcode* stream() {
      return machine_code_->opcodes;
    }

    opcode ip() {
      return ip_;
    }

    int next_ip() {
      return ip_ + width_;
    }

    bool next_p() {
      return ip_ < machine_code_->total;
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
      return machine_code_->opcodes[ip_ + which + 1];
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
      case insn_goto_if_nil:
      case insn_goto_if_not_nil:
      case insn_goto_if_undefined:
      case insn_goto_if_not_undefined:
      case insn_goto_if_equal:
      case insn_goto_if_not_equal:
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
