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

#include "instruments/timing.hpp"

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

  Object* BytecodeVerifier::verify_object(STATE, int index, int ip) {
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

  void BytecodeVerifier::verify(STATE) {
    timer::StopWatch<timer::microseconds> timer(
        state->vm()->metrics().machine.bytecode_verifier_us);

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

  namespace {
#include "gen/instruction_effects.hpp"
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
      size_t width = InstructionSequence::instruction_width(op);

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
        case InstructionSequence::insn_push_literal:
        case InstructionSequence::insn_push_memo:
          verify_object(state, arg1, insn_ip);
          break;
        case InstructionSequence::insn_create_block:
          verify_code(state, arg1, insn_ip);
          break;
        case InstructionSequence::insn_send_vcall:
        case InstructionSequence::insn_send_method:
        case InstructionSequence::insn_send_stack:
        case InstructionSequence::insn_send_stack_with_block:
        case InstructionSequence::insn_send_stack_with_splat:
        case InstructionSequence::insn_check_serial:
        case InstructionSequence::insn_check_serial_private:
        case InstructionSequence::insn_invoke_primitive:
        case InstructionSequence::insn_set_ivar:
        case InstructionSequence::insn_push_ivar:
        case InstructionSequence::insn_set_const:
        case InstructionSequence::insn_set_const_at:
        case InstructionSequence::insn_object_to_s:
        case InstructionSequence::insn_push_const:
        case InstructionSequence::insn_find_const:
        case InstructionSequence::insn_zsuper:
          verify_symbol(state, arg1, insn_ip);
          break;
        case InstructionSequence::insn_send_super_stack_with_block:
        case InstructionSequence::insn_send_super_stack_with_splat:
          verify_symbol_or_nil(state, arg1, insn_ip);
          break;
      }

      int read = 0, write = 0;

      int effect = stack_difference(op, arg1, arg2, arg3, &read, &write);
      // TODO: instructions
      // int effect = Interpreter::instruction_data_(op).stack_effect(arg1, arg2, arg3);

      // Check for under read
      if(sp - read < 0) {
        fail(state, "stack underflow on read", insn_ip);
      }

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
      case InstructionSequence::insn_push_local:
      case InstructionSequence::insn_set_local:
        if((native_int)arg1 < 0 || (native_int)arg1 >= locals_) {
          fail(state, "invalid local variable access", insn_ip);
        }
        break;
      case InstructionSequence::insn_goto:
        if((native_int)arg1 < 0 || (native_int)arg1 >= total_) {
          fail(state, "invalid goto location", insn_ip);
        }

        // Only handle forward branches.
        if((int)arg1 > ip) {
          ip = (int)arg1;
        } else {
          return;
        }
        break;
      case InstructionSequence::insn_push_stack_local:
      case InstructionSequence::insn_set_stack_local:
        if((int)arg1 > max_stack_local_) {
          max_stack_local_ = (int)arg1;
        }
        break;
      case InstructionSequence::insn_goto_if_false:
      case InstructionSequence::insn_goto_if_true:
      case InstructionSequence::insn_setup_unwind:
        if((native_int)arg1 < 0 || (native_int)arg1 >= total_) {
          fail(state, "invalid goto location", insn_ip);
        }

        if((int)arg1 > ip) {
          ips.push_back(Section(sp, arg1));
        }

        break;
      case InstructionSequence::insn_ret:
      case InstructionSequence::insn_raise_exc:
      case InstructionSequence::insn_raise_return:
      case InstructionSequence::insn_ensure_return:
      case InstructionSequence::insn_raise_break:
      case InstructionSequence::insn_reraise:
        return;
      }

      // Detect falling off the end of the stream
      if(ip >= total_) {
        fail(state, "unterminated instruction sequence", insn_ip);
      }
    }
  }
}
