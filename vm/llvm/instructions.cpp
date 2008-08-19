
#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/contexts.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"

#include "jit_state.h"
#include "objectmemory.hpp"

using namespace rubinius;

#define OP(name, args...) void name(Task* task, struct jit_state* const js, ## args)
#define OP2(type, name, args...) type name(Task* task, struct jit_state* const js, ## args)
#define stack_push(val) *++js->stack = val
#define stack_pop() *js->stack--
#define stack_top() *js->stack
#define stack_back(count) *(js->stack - count)
#define stack_set_top(val) *js->stack = (val)

#define SHOW(obj) (((NormalObject*)(obj))->show(state))

#define state task->state

#define I2N(num) APPLY_TAG(num, TAG_FIXNUM)
#define both_fixnum_p(_p1, _p2) ((intptr_t)_p1 & (intptr_t)_p2 & TAG_FIXNUM)

#define cache_ip()

extern "C" {
  bool send_slowly(Task* task, struct jit_state* const js, SYMBOL name);

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
    OBJECT val = stack_pop();
    return !RTEST(val);
  }
  
  OP2(bool, jit_goto_if_true) {
    OBJECT val = stack_pop();
    return RTEST(val);
  }

  OP2(bool, jit_goto_if_defined) {
    OBJECT val = stack_pop();
    return val != Qundef;
  }

  bool send_slowly(Task* task, struct jit_state* const js, SYMBOL name) {
    Message& msg = *task->msg;
    msg.recv = stack_back(1);
    msg.import_arguments(state, task, 1);
    msg.name = name;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.block = Qnil;

    /* pull receiver off stack */
    stack_pop();

    bool res = task->send_message_slowly(msg);
    msg.reset();
    return res;
  }
}

/* A simple interface to the instructions by directly
 * executing an opcode stream. This is used primarly to
 * debug instructions. */

#define next_op() *stream++
#define next_int (opcode)(next_op())

void rubinius::Task::execute_stream(opcode* stream) {
  opcode op;
  Task* task = this;
  struct jit_state* js = &active->js;

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

void VMMethod::resume(Task* task, MethodContext* ctx) {
  opcode* stream = ctx->vmm->opcodes;
  struct jit_state* js = &ctx->js;
  opcode op;

  for(;;) {
    op = stream[ctx->ip++];
#if 0
    std::cout << *ctx->cm->name->to_str(state) << "+" <<
      ctx->ip << ": " << InstructionSequence::get_instruction_name(op) << 
        "        " << js->stack << "\n";
#endif

#ruby <<CODE
io = StringIO.new
si.generate_decoder_switch impl, io, true
puts io.string
CODE
  }

}
