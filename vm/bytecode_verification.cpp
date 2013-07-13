#include "builtin/compiledcode.hpp"
#include "builtin/iseq.hpp"
#include "builtin/tuple.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "bytecode_verification.hpp"
#include "configuration.hpp"
#include "instruments/timing.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"
#include "version.h"

namespace rubinius {
  BytecodeVerification::BytecodeVerification(CompiledCode* code)
    : method_(code)
    , ops_(NULL)
    , total_(0)
    , max_stack_allowed_(0)
    , max_stack_seen_(0)
    , stack_(0)
    , locals_(0)
    , max_stack_local_(-1)
    , fail_reason_(0)
    , fail_ip_(0)
  {}

  BytecodeVerification::~BytecodeVerification() {
    if(stack_) delete[] stack_;
  }

  bool BytecodeVerification::verify(STATE) {
    // Do this setup here instead of the constructor so we can do
    // some validation of the CompiledCode's fields we read them.

    // Double check the method itself, since it might be a nil
    if(!kind_of<CompiledCode>(method_)) {
      fail("invalid method", -1);
      return false;
    }

    if(Fixnum* fix = try_as<Fixnum>(method_->local_count())) {
      locals_ = fix->to_native();
    } else {
      fail("method not initialized properly", -1);
      return false;
    }

    InstructionSequence* iseq = try_as<InstructionSequence>(method_->iseq());
    if(!iseq) {
      fail("method not initialized properly", -1);
      return false;
    }

    if(Tuple* tup = try_as<Tuple>(iseq->opcodes())) {
      ops_ = tup;
    } else {
      fail("method not initialized properly", -1);
      return false;
    }

    if(Fixnum* fix = try_as<Fixnum>(method_->stack_size())) {
      max_stack_allowed_ = fix->to_native();
    } else {
      fail("method not initialized properly", -1);
      return false;
    }

    // FIXME
    //
    // This is conditional because in 1.8 mode, CM's for blocks have arity
    // info that isn't used, but therefore fails these checks because
    // of the way 'for' works.
    //
    // FIXME
    if(!LANGUAGE_18_ENABLED) {
      if(Fixnum* fix = try_as<Fixnum>(method_->splat())) {
        if(fix->to_native() >= locals_) {
          fail("invalid splat position", -1);
          return false;
        }
      }

      Fixnum* tot = try_as<Fixnum>(method_->total_args());
      Fixnum* req = try_as<Fixnum>(method_->required_args());
      Fixnum* post = try_as<Fixnum>(method_->post_args());

      if(!tot || !req || !post) {
        fail("method not initialized properly (missing arg counts)", -1);
        return false;
      }

      if(tot->to_native() > locals_) {
        fail("more arguments than local slots", -1);
        return false;
      }

      if(req->to_native() > tot->to_native()) {
        fail("more required arguments than total", -1);
        return false;
      }

      if(post->to_native() > req->to_native()) {
        fail("more post arguments than required", -1);
        return false;
      }

      if(post->to_native() > tot->to_native()) {
        fail("more post arguments than total", -1);
        return false;
      }

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

      if(!verify_from(state, sp, ip, ips)) return false;
    }

    // Now, check there is a enough space for the stack locals.
    if(max_stack_seen_ + max_stack_local_ >= max_stack_allowed_) {
      fail("not enough space for stack locals", -1);
      return false;
    }

    return true;
  }

  namespace {
#include "gen/instruction_effects.hpp"
  }

  bool BytecodeVerification::verify_from(STATE, int sp, int ip,
                                         std::list<Section>& ips)
  {
    if(sp < 0) {
      fail("internal error", ip);
      return false;
    }

    for(;;) {
      int old_sp = stack_[ip];
      if(old_sp < 0) {
        stack_[ip] = sp;
      } else if(old_sp != sp) {
        fail("inconsistent stack depth", ip);
        return false;
      } else {
        // Already been here and stack is consistent, done.
        return true;
      }

      Fixnum* obj = try_as<Fixnum>(ops_->at(ip++));
      if(!obj) {
        fail("corrupt instruction sequence", ip);
        return false;
      }

      opcode op = obj->to_native();
      size_t width = InstructionSequence::instruction_width(op);

      opcode arg1 = 0;
      opcode arg2 = 0;

      switch(width) {
      case 1:
        // nothing, done.
        break;
      case 2:
        if(ip >= total_) {
          fail("truncated instruction sequence", ip);
          return false;
        }

        obj = try_as<Fixnum>(ops_->at(ip++));
        if(!obj) {
          fail("corrupt instruction sequence", ip);
          return false;
        }
        arg1 = obj->to_native();
        break;

      case 3:
        if(ip >= total_) {
          fail("truncated instruction sequence", ip);
          return false;
        }

        obj = try_as<Fixnum>(ops_->at(ip++));
        if(!obj) {
          fail("corrupt instruction sequence", ip);
          return false;
        }

        arg1 = obj->to_native();

        if(ip >= total_) {
          fail("truncated instruction sequence", ip);
          return false;
        }

        obj = try_as<Fixnum>(ops_->at(ip++));
        if(!obj) {
          fail("corrupt instruction sequence", ip);
          return false;
        }

        arg2 = obj->to_native();
        break;

      default:
        fail("invalid instruction", ip);
        return false;
      }

      int read=0, write=0;

      int effect = stack_difference(op, arg1, arg2, &read, &write);

      // Check for under read
      if(sp - read < 0) {
        fail("stack underflow on read", ip);
        return false;
      }

      // Apply the total effect to propagate it.
      sp += effect;
      if(sp < 0) {
        fail("stack underflow on effect", ip);
        return false;
      }

      // Make sure we don't use more than the declared stack size.
      if(sp > max_stack_allowed_) {
        fail("stack overflow", ip);
        return false;
      }

      // Keep track of the max stack depth seen
      if(sp > max_stack_seen_) max_stack_seen_ = sp;

      switch(op) {
      case InstructionSequence::insn_push_local:
      case InstructionSequence::insn_set_local:
        if((native_int)arg1 < 0 || (native_int)arg1 >= locals_) {
          fail("invalid local variable access", ip);
          return false;
        }
        break;
      case InstructionSequence::insn_goto:
        if((native_int)arg1 < 0 || (native_int)arg1 >= total_) {
          fail("invalid goto location", ip);
          return false;
        }

        // Only handle forward branches.
        if((int)arg1 > ip) {
          ip = (int)arg1;
        } else {
          return true;
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
          fail("invalid goto location", ip);
          return false;
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
        return true;
      }

      // Detect falling off the end of the stream
      if(ip >= total_) {
        fail("unterminated instruction sequence", ip);
        return false;
      }
    }
  }
}
