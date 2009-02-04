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
#include "builtin/lookuptable.hpp"
#include "builtin/block_wrapper.hpp"
#include "builtin/thread.hpp"

#include "call_frame.hpp"

#include "objectmemory.hpp"
#include "message.hpp"
#include "instructions.hpp"
#include "profiler.hpp"

using namespace rubinius;

// HACK: sassert is stack protection
#ifdef RBX_DEBUG

#define stack_push(val) ({ Object* _v = (val); sassert(_v && (int)_v != 0x10 && call_frame->js.stack < call_frame->js.stack_top); *++call_frame->js.stack = _v; })
#define stack_pop() ({ assert(call_frame->js.stack >= call_frame->stk); *call_frame->js.stack--; })
#define stack_set_top(val) ({ Object* _v = (val); assert((int)(_v) != 0x10); *call_frame->js.stack = _v; })

#else

/** We have to use the local here we need to evaluate val before we alter
 * the stack. The reason is evaluating val might throw an exception. The
 * old code used an undefined behavior, this forces the order. */
#define stack_push(val) ({ Object* __stack_v = (val); *++call_frame->js.stack = __stack_v; })
#define stack_pop() *call_frame->js.stack--
#define stack_set_top(val) *call_frame->js.stack = (val)

#define USE_JUMP_TABLE

#endif

#define stack_top() *call_frame->js.stack
#define stack_back(count) *(call_frame->js.stack - count)
#define stack_clear(count) call_frame->clear_stack(count)

#define SHOW(obj) (((NormalObject*)(obj))->show(state))

#define state task->state

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define cache_ip()

extern "C" {
  Object* send_slowly(VMMethod* vmm, Task* task, CallFrame* const call_frame, Symbol* name, size_t args);

#define HANDLE_EXCEPTION(val) if(val == NULL) return NULL
#define RUN_EXCEPTION() return NULL

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

  Object* send_slowly(VMMethod* vmm, Task* task, CallFrame* const call_frame, Symbol* name, size_t args) {
    Message& msg = *task->msg;
    msg.setup(static_cast<SendSite*>(Qnil), stack_back(args), call_frame, args, args + 1);
    msg.name = name;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.block = Qnil;

    return task->send_message_slowly(call_frame, msg);
  }
}

/* A simple interface to the instructions by directly
 * executing an opcode stream. This is used primarly to
 * debug instructions. */

#define next_op() *stream++
#define next_int (opcode)(next_op())

#undef RETURN
#define RETURN(val) (void)val; return;

Object* rubinius::Task::execute_stream(CallFrame* call_frame, opcode* stream) {
  opcode op;
  Task* task = this;
  VMMethod* const vmm = call_frame->vmm;

  op = next_op();

#ruby <<CODE
io = StringIO.new
si.generate_decoder_switch impl, io
puts io.string
CODE

  return Qnil;
}

/* Use a simplier next_int */
#undef next_int
#define next_int ((opcode)(stream[call_frame->ip++]))

#undef RETURN
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { continue; }

Object* VMMethod::interpreter(VMMethod* const vmm, Task* const task, CallFrame* const call_frame) {
  opcode* stream = call_frame->vmm->opcodes;
#ifdef USE_JUMP_TABLE

#undef DISPATCH_NEXT_INSN
#define DISPATCH_NEXT_INSN goto *insn_locations[stream[call_frame->ip++]];

#undef RETURN
  /*
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { \
  if(unlikely(state->interrupts.check)) return;\
  DISPATCH_NEXT_INSN; \
}
*/
#define RETURN(val) (void)val; DISPATCH_NEXT_INSN;

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
    op = stream[call_frame->ip++];

#if FLAG_FIRE_PROBE_INSTRUCTION
    if(!task->probe()->nil_p()) {
      task->probe()->execute_instruction(task, call_frame, op);
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


/* The debugger interpreter loop is used to run a method when a breakpoint
 * has been set. It has additional overhead, since it needs to inspect
 * each opcode for the breakpoint flag. It is installed on the VMMethod when
 * a breakpoint is set on compiled method.
 */
Object* VMMethod::debugger_interpreter(VMMethod* const vmm, Task* const task, CallFrame* const call_frame) {
  opcode* stream = call_frame->vmm->opcodes;
  opcode op;
#ifdef USE_JUMP_TABLE

#undef DISPATCH_NEXT_INSN
#define DISPATCH_NEXT_INSN op = stream[call_frame->ip++]; \
  if(unlikely(op & cBreakpoint)) { \
    if(G(current_thread)->frozen_stack() == Qfalse) { \
      call_frame->ip--; \
      task->yield_debugger(); \
    } else { \
      G(current_thread)->frozen_stack(state, Qfalse); \
    } \
  } \
  op &= 0x00ffffff; \
  goto *insn_locations[op];

#undef RETURN
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { \
  DISPATCH_NEXT_INSN; \
}

#ruby <<CODE
io = StringIO.new
si.generate_jump_implementations impl, io, true
puts io.string
CODE

#else

#undef RETURN
#define RETURN(val) if((val) == cExecuteRestart) { return; } else { continue; }
  for(;;) {
    op = stream[call_frame->ip++];

#if FLAG_FIRE_PROBE_INSTRUCTION
    if(!task->probe()->nil_p()) {
      task->probe()->execute_instruction(task, call_frame, op);
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
