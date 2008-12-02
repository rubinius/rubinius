/** \file   Base template for generating vm/gen/instructions.cpp. */

#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/taskprobe.hpp"
#include "builtin/tuple.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/nativemethodcontext.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/contexts.hpp"

#include "objectmemory.hpp"
#include "message.hpp"
#include "instructions.hpp"

#define USE_JUMP_TABLE

using namespace rubinius;

// #define OP(name, args...) void name(Task* task, struct jit_state* const js, ## args)
#define OP2(type, name, args...) type name(Task* task, MethodContext* const ctx, ## args)

// HACK: sassert is stack protection
// #define stack_push(val) ({ OBJECT _v = (val); sassert(_v && js->stack < js->stack_top); *++js->stack = _v; })

/** We have to use the local here we need to evaluate val before we alter
 * the stack. The reason is evaluating val might throw an exception. The
 * old code used an undefined behavior, this forces the order. */
#define stack_push(val) ({ Object* __stack_v = (val); *++ctx->js.stack = __stack_v; })

#define stack_pop() *ctx->js.stack--
#define stack_top() *ctx->js.stack
#define stack_back(count) *(ctx->js.stack - count)
#define stack_set_top(val) *ctx->js.stack = (val)

#define SHOW(obj) (((NormalObject*)(obj))->show(state))

#define state task->state

#define I2N(num) APPLY_TAG(num, TAG_FIXNUM)
#define both_fixnum_p(_p1, _p2) (FIXNUM_P(_p1) && FIXNUM_P(_p2))

#define cache_ip()

extern "C" {
  ExecuteStatus send_slowly(VMMethod* vmm, Task* task, MethodContext* const ctx, Symbol* name, size_t args);

#define RETURN(val) return val

#ruby <<CODE
require 'stringio'
require 'vm/instructions.rb'
si = Instructions.new
impl = si.decode_methods
io = StringIO.new
si.generate_functions impl, io
puts io.string
CODE

/*
  OP2(bool, fixed_args_prelude, size_t required) {
    Message* const msg = task->msg;

    if(msg->args != required) {
      return false;
    }

    MethodContext* ctx = MethodContext::create(state, msg->recv, original.get());
    task->make_active(ctx);

    for(size_t i = 0; i < required; i++) {
      ctx->stack->put(task->state, i, msg->get_argument(i));
    }

    return true;
  }

  OP2(bool, zero_args_prelude) {
    Message* const msg = task->msg;
    if(task->msg.args != 0) return false;

    MethodContext* ctx = MethodContext::create(state, msg->recv, original.get());
    task->make_active(ctx);
    return true;
  }

  OP2(bool, full_prelude) {
    task->import_arguments(task->ctx, *task->msg);
  }

  */

  OP2(bool, jit_goto_if_false) {
    Object* val = stack_pop();
    return !RTEST(val);
  }
  
  OP2(bool, jit_goto_if_true) {
    Object* val = stack_pop();
    return RTEST(val);
  }

  OP2(bool, jit_goto_if_defined) {
    Object* val = stack_pop();
    return val != Qundef;
  }

  ExecuteStatus send_slowly(VMMethod* vmm, Task* task, MethodContext* const ctx, Symbol* name, size_t args) {
    Message& msg = *task->msg;
    msg.recv = stack_back(args);
    msg.use_from_task(task, args);
    msg.name = name;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.block = Qnil;
    msg.stack = args + 1;

    return task->send_message_slowly(msg);
  }
}

/* A simple interface to the instructions by directly
 * executing an opcode stream. This is used primarly to
 * debug instructions. */

#define next_op() *stream++
#define next_int (opcode)(next_op())

#undef RETURN
#define RETURN(val) (void)val; return;

void rubinius::Task::execute_stream(opcode* stream) {
  opcode op;
  Task* task = this;
  MethodContext* ctx = active_;
  VMMethod* const vmm = ctx->vmm;

  op = next_op();

#ruby <<CODE
io = StringIO.new
si.generate_decoder_switch impl, io
puts io.string
CODE

}

/* Use a simplier next_int */
#undef next_int
#define next_int ((opcode)(stream[ctx->ip++]))

#undef RETURN
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { continue; }

void VMMethod::resume(Task* task, MethodContext* ctx) {
  VMMethod* const vmm = this;
  opcode* stream = ctx->vmm->opcodes;
#ifdef USE_JUMP_TABLE

#undef RETURN
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { \
  if(unlikely(state->interrupts.check)) return;\
  goto *insn_locations[stream[ctx->ip++]]; \
}

#ruby <<CODE
io = StringIO.new
si.generate_jump_implementations impl, io, true
puts io.string
CODE

#else
  opcode op;

#undef RETURN
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { continue; }
  for(;;) {
    op = stream[ctx->ip++];

#if FLAG_FIRE_PROBE_INSTRUCTION
    if(!task->probe()->nil_p()) {
      task->probe()->execute_instruction(task, ctx, op);
    }
#endif

#ruby <<CODE
io = StringIO.new
si.generate_decoder_switch impl, io, true
puts io.string
CODE
  }
#endif // USE_JUMP_TABLE
}

namespace rubinius {
namespace instructions {
#ruby <<CODE
puts si.generate_implementation_info
CODE
}
}
