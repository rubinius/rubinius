#include "class/compiled_code.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/iseq.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"

#include "bytecode_verifier.hpp"
#include "configuration.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"
#include "interpreter.hpp"
#include "instructions.hpp"

#include "diagnostics/machine.hpp"
#include "diagnostics/timing.hpp"

namespace rubinius {
  BytecodeVerifier::BytecodeVerifier(CompiledCode* code)
    : method_(code)
    , ops_(NULL)
    , total_(0)
    , max_stack_allowed_(0)
    , max_stack_seen_(0)
    , stack_(0)
    , locals_(0)
    , max_stack_local_(-1)
  {}

  BytecodeVerifier::~BytecodeVerifier() {
    if(stack_) delete[] stack_;
  }

  void BytecodeVerifier::fail(STATE, const char* reason, int ip) {
    Exception::bytecode_error(state, method_, ip, reason);
  }

  void BytecodeVerifier::verify_width(STATE, int ip) {
    if(ip >= total_) {
      fail(state, "truncated instruction sequence", ip);
    }
  }

  Fixnum* BytecodeVerifier::verify_opcode(STATE, int ip) {
    verify_width(state, ip);

    if(Fixnum* fix = try_as<Fixnum>(ops_->at(ip))) {
      return fix;
    } else {
      fail(state, "instruction is not a Fixnum", ip);
    }
  }

  Fixnum* BytecodeVerifier::verify_argument(STATE, int ip) {
    verify_width(state, ip);

    if(Fixnum* fix = try_as<Fixnum>(ops_->at(ip))) {
      return fix;
    } else {
      fail(state, "instruction argument is not a Fixnum", ip);
    }
  }

  void BytecodeVerifier::verify_jump_location(STATE, int index, int ip) {
    if(index < 0 || index >= total_) {
      fail(state, "invalid goto location", ip);
    }
  }

  void BytecodeVerifier::verify_past_location(STATE, int index, int ip) {
    if(index < 0 || index >= ip) {
      fail(state, "invalid goto_past location", ip);
    }
  }

  void BytecodeVerifier::verify_future_location(STATE, int index, int ip) {
    if(index <= (ip + 1) || index >= total_) {
      fail(state, "invalid goto_future location", ip);
    }
  }

  void BytecodeVerifier::verify_unwind(STATE, int index, int ip) {
    opcode op = static_cast<opcode>(verify_opcode(state, index)->to_native());

    if(op != instructions::data_unwind.id) {
      fail(state, "next instruction must be unwind", ip);
    }
  }

  Object* BytecodeVerifier::verify_literal(STATE, int index, int ip) {
    if(Object* obj = try_as<Object>(method_->literals()->at(index))) {
      return obj;
    } else {
      fail(state, "instruction argument is not an Object", ip);
    }
  }

  Symbol* BytecodeVerifier::verify_symbol(STATE, int index, int ip) {
    if(Symbol* sym = try_as<Symbol>(method_->literals()->at(index))) {
      return sym;
    } else {
      fail(state, "instruction argument is not a Symbol", ip);
    }
  }

  Object* BytecodeVerifier::verify_symbol_or_nil(STATE, int index, int ip) {
    Object* val = method_->literals()->at(index);

    if(val->nil_p()) {
      return val;
    } else if(Symbol* sym = try_as<Symbol>(val)) {
      return sym;
    } else {
      fail(state, "instruction argument is not a Symbol or nil", ip);
    }
  }

  Object* BytecodeVerifier::verify_code(STATE, int index, int ip) {
    if(CompiledCode* code = try_as<CompiledCode>(method_->literals()->at(index))) {
      return force_as<Object>(code);
    } else if(String* str = try_as<String>(method_->literals()->at(index))) {
      return force_as<Object>(str);
    } else {
      fail(state, "instruction argument is not a String or CompiledCode", ip);
    }
  }

  void BytecodeVerifier::verify_register(STATE, int reg, int ip) {
    if(reg < 0 || reg >= max_registers_) {
      fail(state, "register value exceeds max register count", ip);
    }
  }

  void BytecodeVerifier::verify_local(STATE, int local, int ip) {
    if(local < 0 || local >= locals_) {
      fail(state, "invalid local variable access", ip);
    }
  }

  void BytecodeVerifier::verify(STATE) {
    timer::StopWatch<timer::microseconds> timer(
        state->vm()->metrics()->bytecode_verifier_us);

    // Do this setup here instead of the constructor so we can do
    // some validation of the CompiledCode's fields we read them.

    // Double check the method itself, since it might be a nil
    if(!kind_of<CompiledCode>(method_)) {
      fail(state, "not passed a CompiledCode object", -1);
    }

    if(Fixnum* fix = try_as<Fixnum>(method_->local_count())) {
      locals_ = fix->to_native();
    } else {
      fail(state, "local_count is not a Fixnum", -1);
    }

    InstructionSequence* iseq = try_as<InstructionSequence>(method_->iseq());
    if(!iseq) {
      fail(state, "iseq is not an InstructionSequence", -1);
    }

    if(Tuple* tup = try_as<Tuple>(iseq->opcodes())) {
      ops_ = tup;
    } else {
      fail(state, "opcodes is not a Tuple", -1);
    }

    if(Fixnum* fix = try_as<Fixnum>(method_->stack_size())) {
      max_stack_allowed_ = fix->to_native();
    } else {
      fail(state, "stack_size is not a Fixnum", -1);
    }

    if(Fixnum* fix = try_as<Fixnum>(method_->registers())) {
      max_registers_ = fix->to_native();
    } else {
      fail(state, "registers is not a Fixnum", -1);
    }

    if(Fixnum* fix = try_as<Fixnum>(method_->splat())) {
      if(fix->to_native() >= locals_) {
        fail(state, "invalid splat position", -1);
      }
    }

    Fixnum* tot = try_as<Fixnum>(method_->total_args());
    if(!tot) {
      fail(state, "total_args is not a Fixnum", -1);
    }
    Fixnum* req = try_as<Fixnum>(method_->required_args());
    if(!req) {
      fail(state, "required_args is not a Fixnum", -1);
    }
    Fixnum* post = try_as<Fixnum>(method_->post_args());
    if(!post) {
      fail(state, "post_args is not a Fixnum", -1);
    }

    if(tot->to_native() > locals_) {
      fail(state, "more arguments than local slots", -1);
    }

    if(req->to_native() > tot->to_native()) {
      fail(state, "more required arguments than total", -1);
    }

    if(post->to_native() > req->to_native()) {
      fail(state, "more post arguments than required", -1);
    }

    if(post->to_native() > tot->to_native()) {
      fail(state, "more post arguments than total", -1);
    }

    total_ = ops_->num_fields();
    stack_ = new int32_t[total_];

    for(native_int i = 0; i < total_; i++) {
      stack_[i] = -1;
    }

    std::list<Section> ips;
    ips.push_back(Section(0, 0));

    while(!ips.empty()) {
      Section& section = ips.front();

      int ip = section.ip;
      int sp = section.sp;

      ips.pop_front();

      verify_from(state, sp, ip, ips);
    }

    // Now, check there is a enough space for the stack locals.
    if(max_stack_seen_ + max_stack_local_ >= max_stack_allowed_) {
      fail(state, "not enough space for stack locals", -1);
    }
  }

  void BytecodeVerifier::verify_from(STATE, int sp, int ip, std::list<Section>& ips) {
    int insn_ip = ip;

    if(sp < 0) {
      fail(state, "stack underflow error", ip);
    }

    for(;;) {
      int old_sp = stack_[ip];
      if(old_sp < 0) {
        stack_[ip] = sp;
      } else if(old_sp != sp) {
        fail(state, "inconsistent stack depth", ip);
      } else {
        // Already been here and stack is consistent, done.
        return;
      }

      opcode op = static_cast<opcode>(verify_opcode(state, ip++)->to_native());
      size_t width = Instructions::instruction_data(op).width;

      opcode arg1 = 0, arg2 = 0, arg3 = 0;

      switch(width) {
      case 1:
        break;
      case 2:
        arg1 = static_cast<opcode>(verify_argument(state, ip++)->to_native());
        break;
      case 3:
        arg1 = static_cast<opcode>(verify_argument(state, ip++)->to_native());
        arg2 = static_cast<opcode>(verify_argument(state, ip++)->to_native());
        break;
      case 4:
        arg1 = static_cast<opcode>(verify_argument(state, ip++)->to_native());
        arg2 = static_cast<opcode>(verify_argument(state, ip++)->to_native());
        arg3 = static_cast<opcode>(verify_argument(state, ip++)->to_native());
        break;
      default:
        fail(state, "invalid instruction", insn_ip);
      }

      switch(op) {
        case instructions::data_push_literal.id:
        case instructions::data_push_memo.id:
          verify_literal(state, arg1, insn_ip);
          break;
        case instructions::data_create_block.id:
          verify_code(state, arg1, insn_ip);
          break;
        case instructions::data_send_vcall.id:
        case instructions::data_send_method.id:
        case instructions::data_send_stack.id:
        case instructions::data_send_stack_with_block.id:
        case instructions::data_send_stack_with_splat.id:
        case instructions::data_check_serial.id:
        case instructions::data_check_serial_private.id:
        case instructions::data_invoke_primitive.id:
        case instructions::data_set_ivar.id:
        case instructions::data_push_ivar.id:
        case instructions::data_set_const.id:
        case instructions::data_set_const_at.id:
        case instructions::data_object_to_s.id:
        case instructions::data_push_const.id:
        case instructions::data_find_const.id:
        case instructions::data_zsuper.id:
          verify_symbol(state, arg1, insn_ip);
          break;
        case instructions::data_send_super_stack_with_block.id:
        case instructions::data_send_super_stack_with_splat.id:
          verify_symbol_or_nil(state, arg1, insn_ip);
          break;
        case instructions::data_b_if.id:
        case instructions::data_m_log.id:
          verify_register(state, arg1, insn_ip);
          break;
        case instructions::data_b_if_int.id:
        case instructions::data_a_instance.id:
        case instructions::data_a_kind.id:
        case instructions::data_a_method.id:
        case instructions::data_a_receiver_method.id:
        case instructions::data_a_type.id:
        case instructions::data_a_function.id:
        case instructions::data_a_equal.id:
        case instructions::data_a_not_equal.id:
        case instructions::data_a_less.id:
        case instructions::data_a_less_equal.id:
        case instructions::data_a_greater.id:
        case instructions::data_a_greater_equal.id:
          verify_register(state, arg1, insn_ip);
          verify_register(state, arg2, insn_ip);
          break;
        case instructions::data_b_if_serial.id:
          verify_literal(state, arg1, insn_ip);
          verify_register(state, arg2, insn_ip);
          break;
        case instructions::data_r_load_local.id:
        case instructions::data_r_store_local.id:
          verify_register(state, arg1, insn_ip);
          verify_local(state, arg2, insn_ip);
          break;
        case instructions::data_r_load_local_depth.id:
        case instructions::data_r_store_local_depth.id:
          verify_register(state, arg1, insn_ip);
          break;
        case instructions::data_r_load_stack.id:
        case instructions::data_r_store_stack.id:
          verify_register(state, arg1, insn_ip);
          break;
        case instructions::data_r_load_literal.id:
          verify_register(state, arg1, insn_ip);
          verify_literal(state, arg2, insn_ip);
          break;
        case instructions::data_r_load_nil.id:
          verify_register(state, arg1, insn_ip);
          break;
        case instructions::data_r_load_0.id:
        case instructions::data_r_load_1.id:
        case instructions::data_r_load_false.id:
        case instructions::data_r_load_true.id:
          verify_register(state, arg1, insn_ip);
          break;
        case instructions::data_r_load_int.id:
        case instructions::data_r_store_int.id:
        case instructions::data_r_copy.id:
          verify_register(state, arg1, insn_ip);
          verify_register(state, arg2, insn_ip);
          break;
        case instructions::data_n_iadd.id:
        case instructions::data_n_isub.id:
        case instructions::data_n_imul.id:
        case instructions::data_n_idiv.id:
        case instructions::data_n_iadd_o.id:
        case instructions::data_n_isub_o.id:
        case instructions::data_n_imul_o.id:
        case instructions::data_n_idiv_o.id:
        case instructions::data_n_ieq.id:
        case instructions::data_n_ine.id:
        case instructions::data_n_ilt.id:
        case instructions::data_n_ile.id:
        case instructions::data_n_igt.id:
        case instructions::data_n_ige.id:
          verify_register(state, arg1, insn_ip);
          verify_register(state, arg2, insn_ip);
          verify_register(state, arg3, insn_ip);
          break;
        case instructions::data_n_ipopcnt.id:
          verify_register(state, arg1, insn_ip);
          verify_register(state, arg2, insn_ip);
          break;
      }

      switch(op) {
        case instructions::data_cast_array.id:
        case instructions::data_cast_for_multi_block_arg.id:
        case instructions::data_cast_for_single_block_arg.id:
        case instructions::data_cast_for_splat_block_arg.id:
        case instructions::data_cast_multi_value.id:
        case instructions::data_check_frozen.id:
        case instructions::data_create_block.id:
        case instructions::data_ensure_return.id:
        case instructions::data_find_const.id:
        case instructions::data_invoke_primitive.id:
        case instructions::data_object_to_s.id:
        case instructions::data_passed_arg.id:
        case instructions::data_passed_blockarg.id:
        case instructions::data_push_block_arg.id:
        case instructions::data_push_const.id:
        case instructions::data_push_ivar.id:
        case instructions::data_push_local_depth.id:
        case instructions::data_push_proc.id:
        case instructions::data_raise_break.id:
        case instructions::data_raise_exc.id:
        case instructions::data_raise_return.id:
        case instructions::data_reraise.id:
        case instructions::data_send_method.id:
        case instructions::data_send_stack.id:
        case instructions::data_send_stack_with_block.id:
        case instructions::data_send_stack_with_splat.id:
        case instructions::data_send_super_stack_with_block.id:
        case instructions::data_send_super_stack_with_splat.id:
        case instructions::data_send_vcall.id:
        case instructions::data_set_ivar.id:
        case instructions::data_set_local_depth.id:
        case instructions::data_string_build.id:
        case instructions::data_string_dup.id:
        case instructions::data_yield_splat.id:
        case instructions::data_yield_stack.id:
        case instructions::data_zsuper.id:
          verify_unwind(state, ip, ip - width);
          break;
      }

      int effect = Instructions::instruction_data(op).stack_effect(arg1, arg2, arg3);

      // Apply the total effect to propagate it.
      sp += effect;
      if(sp < 0) {
        fail(state, "stack underflow on effect", insn_ip);
      }

      // Make sure we don't use more than the declared stack size.
      if(sp > max_stack_allowed_) {
        fail(state, "stack overflow", insn_ip);
      }

      // Keep track of the max stack depth seen
      if(sp > max_stack_seen_) max_stack_seen_ = sp;

      switch(op) {
        case instructions::data_push_local.id:
        case instructions::data_set_local.id:
          verify_local(state, arg1, insn_ip);
          break;
        case instructions::data_goto_past.id:
          verify_jump_location(state, arg1, insn_ip);
          break;
        case instructions::data_goto_future.id:
          verify_jump_location(state, arg1, insn_ip);
          break;
        case instructions::data_goto.id:
          verify_jump_location(state, arg1, insn_ip);

          // Only handle forward branches.
          if((int)arg1 > ip) {
            ip = (int)arg1;
          } else {
            return;
          }
          break;
        case instructions::data_push_stack_local.id:
        case instructions::data_set_stack_local.id:
          if((int)arg1 > max_stack_local_) {
            max_stack_local_ = (int)arg1;
          }
          break;
        case instructions::data_goto_if_equal.id:
        case instructions::data_goto_if_false.id:
        case instructions::data_goto_if_nil.id:
        case instructions::data_goto_if_not_equal.id:
        case instructions::data_goto_if_not_nil.id:
        case instructions::data_goto_if_not_undefined.id:
        case instructions::data_goto_if_true.id:
        case instructions::data_goto_if_undefined.id:
        case instructions::data_setup_unwind.id:
          verify_jump_location(state, arg1, insn_ip);

          if((int)arg1 > ip) {
            ips.push_back(Section(sp, arg1));
          }

          break;
        case instructions::data_b_if.id:
          verify_jump_location(state, arg2, insn_ip);

          if((int)arg2 > ip) {
            ips.push_back(Section(sp, arg2));
          }

          break;
        case instructions::data_b_if_int.id:
        case instructions::data_b_if_serial.id:
          verify_jump_location(state, arg3, insn_ip);

          if((int)arg3 > ip) {
            ips.push_back(Section(sp, arg3));
          }

          break;
        case instructions::data_ret.id:
        case instructions::data_raise_exc.id:
        case instructions::data_raise_return.id:
        case instructions::data_ensure_return.id:
        case instructions::data_raise_break.id:
        case instructions::data_reraise.id:
          return;
      }

      // Detect falling off the end of the stream
      if(ip >= total_) {
        fail(state, "unterminated instruction sequence", insn_ip);
      }
    }
  }
}
