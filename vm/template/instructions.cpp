/** \file   Base template for generating vm/gen/instructions.cpp. */

#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/autoload.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/taskprobe.hpp"
#include "builtin/tuple.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/proc.hpp"
#include "builtin/thread.hpp"
#include "builtin/system.hpp"
#include "builtin/global_cache_entry.hpp"

#include "call_frame.hpp"

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "instructions.hpp"
#include "instruments/profiler.hpp"

#include "helpers.hpp"

using namespace rubinius;

#define STACK_PTR stack_ptr

// HACK: sassert is stack protection
#ifdef RBX_STACK_DEBUG

#define stack_push(val) ({ Object* _v = (val); sassert(_v && (int)_v != 0x10 && call_frame->js.stack < call_frame->js.stack_top); *++call_frame->js.stack = _v; })
#define stack_pop() ({ assert(call_frame->js.stack >= call_frame->stk); *call_frame->js.stack--; })
#define stack_set_top(val) ({ Object* _v = (val); assert((int)(_v) != 0x10); *call_frame->js.stack = _v; })

#else

/** We have to use the local here we need to evaluate val before we alter
 * the stack. The reason is evaluating val might throw an exception. The
 * old code used an undefined behavior, this forces the order. */
#define stack_push(val) ({ Object* __stack_v = (val); *++STACK_PTR = __stack_v; })
#define stack_pop() *STACK_PTR--
#define stack_set_top(val) *STACK_PTR = (val)

#define USE_JUMP_TABLE

#endif

#define stack_top() *STACK_PTR
#define stack_back(count) *(STACK_PTR - count)
#define stack_clear(count) STACK_PTR -= count

#define stack_position(where) (STACK_PTR = call_frame->stk + where)
#define stack_calculate_sp() (STACK_PTR - call_frame->stk)

#define next_int ((opcode)(stream[call_frame->inc_ip()]))

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

extern "C" {
  Object* send_slowly(STATE, VMMethod* vmm, InterpreterCallFrame* const call_frame, Symbol* name, Object** stk_pos, size_t args);

#define HANDLE_EXCEPTION(val) if(val == NULL) goto exception
#define RUN_EXCEPTION() goto exception

#define SET_CALL_FLAGS(val) is.call_flags = (val)
#define CALL_FLAGS() is.call_flags

#define SET_ALLOW_PRIVATE(val) is.allow_private = (val)
#define ALLOW_PRIVATE() is.allow_private

#define stack_back_position(count) (STACK_PTR - (count - 1))

#ruby <<CODE
  require 'stringio'
    require 'vm/instructions.rb'
    si = Instructions.new
    impl = si.decode_methods
    nil
CODE

    Object* send_slowly(STATE, VMMethod* vmm, InterpreterCallFrame* const call_frame, Symbol* name, Object** stk_pos, size_t count) {
      Object* recv = stk_pos[0];
      Arguments args(recv, count, stk_pos+1);
      Dispatch dis(name);

      return dis.send(state, call_frame, args);
    }
}


Object* VMMethod::interpreter(STATE, VMMethod* const vmm,
    InterpreterCallFrame* const call_frame, Arguments& args)
{
#ruby <<CODE
  impl2 = si.decode_methods
  si.generate_jump_table impl2, true
CODE

  InterpreterState is;

#ifdef __i386__
  register void** ip_ptr asm ("esi") = vmm->addresses;
#else
  register void** ip_ptr = vmm->addresses;
#endif

  Object** stack_ptr = call_frame->stk - 1;
  Object* return_value;
  static int tick = 0;

  int current_unwind = 0;
  UnwindInfo unwinds[kMaxUnwindInfos];

  for(;;) {
continue_to_run:
    if(unlikely(++tick > 0xff)) {
      tick = 0;
      if(!state->check_stack(call_frame, &state)) return NULL;
    }

    if(unlikely(state->interrupts.check)) {
      state->interrupts.check = false;
      state->collect_maybe(call_frame);
    }

    if(unlikely(state->check_local_interrupts)) {
      if(!state->process_async(call_frame)) return NULL;
    }

    try {

#undef DISPATCH
#define DISPATCH goto **ip_ptr++
// #define DISPATCH goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#define next_int ((opcode)(*ip_ptr++))

#define cache_ip(which) ip_ptr = vmm->addresses + which
#define flush_ip() call_frame->calculate_ip(ip_ptr)

#ruby <<CODE
      io = StringIO.new
      si.generate_jump_implementations impl2, io
      puts io.string
CODE
    } catch(TypeError& e) {
      Exception* exc =
        Exception::make_type_error(state, e.type, e.object, e.reason);
      exc->locations(state, System::vm_backtrace(state, 0, call_frame));

      state->thread_state()->raise_exception(exc);
      return_value = 0;
    }

    if(return_value) return return_value;

exception:
    ThreadState* th = state->thread_state();
    // if return_value is NULL, then there is an exception outstanding
    //
    switch(th->raise_reason()) {
    case cException:
      if(current_unwind > 0) {
        UnwindInfo* info = &unwinds[--current_unwind];
        stack_position(info->stack_depth);
        call_frame->set_ip(info->target_ip);
        cache_ip(info->target_ip);
      } else {
        return NULL;
      }
      break;

    case cReturn:
    case cBreak:
      // Otherwise, we're doing a long return/break unwind through
      // here. We need to run ensure blocks.
      while(current_unwind > 0) {
        UnwindInfo* info = &unwinds[--current_unwind];
        if(info->for_ensure()) {
          stack_position(info->stack_depth);
          call_frame->set_ip(info->target_ip);
          cache_ip(info->target_ip);

          // Don't reset ep here, we're still handling the return/break.
          goto continue_to_run;
        }
      }

      // Ok, no ensures to run.
      if(th->raise_reason() == cReturn) {
        // If we're trying to return to here, we're done!
        if(th->destination_scope() == call_frame->scope) {
          Object* val = th->raise_value();
          th->clear_exception(true);
          return val;
        } else {
          // Give control of this exception to the caller.
          return NULL;
        }

      } else { // It's cBreak
        // If we're trying to break to here, we're done!
        if(th->destination_scope() == call_frame->scope) {
          stack_push(th->raise_value());
          th->clear_exception(true);
          // Don't return here, because we want to loop back to the top
          // and keep running this method.
        } else {
          // Give control of this exception to the caller.
          return NULL;
        }
      }
      break;
    case cExit:
      return NULL;
    default:
      std::cout << "bug!\n";
      call_frame->print_backtrace(state);
      abort();
    } // switch
  } // for(;;)
}


/* The debugger interpreter loop is used to run a method when a breakpoint
 * has been set. It has additional overhead, since it needs to inspect
 * each opcode for the breakpoint flag. It is installed on the VMMethod when
 * a breakpoint is set on compiled method.
 */
Object* VMMethod::debugger_interpreter(STATE, VMMethod* const vmm,
                                       InterpreterCallFrame* const call_frame,
                                       Arguments& args) {
#ruby <<CODE
    impl2 = si.decode_methods
    si.generate_jump_table impl2
CODE

  opcode* stream = vmm->opcodes;
  InterpreterState is;

  int current_unwind = 0;
  UnwindInfo unwinds[kMaxUnwindInfos];

  opcode op;

  // TODO: ug, cut and paste of the whole interpreter above. Needs to be fast,
  // maybe could use a function template?
  //
  // The only thing different is the DISPATCH macro, to check for debugging
  // instructions.

  Object** stack_ptr = call_frame->stk - 1;
  Object* return_value;
  static int tick = 0;

  for(;;) {
continue_to_run:
    if(unlikely(++tick > 0xff)) {
      tick = 0;
      if(!state->check_stack(call_frame, &state)) return NULL;
    }

    if(unlikely(state->interrupts.check)) {
      state->interrupts.check = false;
      state->collect_maybe(call_frame);
    }

    if(unlikely(state->check_local_interrupts)) {
      if(!state->process_async(call_frame)) return NULL;
    }

    try {

#undef DISPATCH
#define DISPATCH op = stream[call_frame->inc_ip()]; \
  if(unlikely(op & cBreakpoint)) { \
    call_frame->dec_ip(); \
    Helpers::yield_debugger(state, call_frame); \
    call_frame->inc_ip(); \
    op &= 0x00ffffff; \
  } \
  goto *insn_locations[op];

#undef next_int
#undef cache_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define cache_ip(which)
#define flush_ip()

#ruby <<CODE
      io = StringIO.new
      si.generate_jump_implementations impl2, io
      puts io.string
CODE
    } catch(TypeError& e) {
      Exception* exc =
        Exception::make_type_error(state, e.type, e.object, e.reason);
      exc->locations(state, System::vm_backtrace(state, 0, call_frame));

      state->thread_state()->raise_exception(exc);
      return_value = 0;
    }

    if(return_value) return return_value;

exception:
    ThreadState* th = state->thread_state();
    // if return_value is NULL, then there is an exception outstanding
    //
    switch(th->raise_reason()) {
    case cException:
      if(current_unwind > 0) {
        UnwindInfo* info = &unwinds[--current_unwind];
        stack_position(info->stack_depth);
        call_frame->set_ip(info->target_ip);
        cache_ip(info->target_ip);
      } else {
        return NULL;
      }
      break;

    case cReturn:
    case cBreak:
      // Otherwise, we're doing a long return/break unwind through
      // here. We need to run ensure blocks.
      while(current_unwind > 0) {
        UnwindInfo* info = &unwinds[--current_unwind];
        stack_position(info->stack_depth);

        if(info->for_ensure()) {
          stack_position(info->stack_depth);
          call_frame->set_ip(info->target_ip);
          cache_ip(info->target_ip);

          // Don't reset ep here, we're still handling the return/break.
          goto continue_to_run;
        }
      }

      // Ok, no ensures to run.
      if(th->raise_reason() == cReturn) {
        // If we're trying to return to here, we're done!
        if(th->destination_scope() == call_frame->scope) {
          Object* val = th->raise_value();
          th->clear_exception(true);
          return val;
        } else {
          // Give control of this exception to the caller.
          return NULL;
        }

      } else { // It's cBreak
        // If we're trying to break to here, we're done!
        if(th->destination_scope() == call_frame->scope) {
          stack_push(th->raise_value());
          th->clear_exception(true);
          // Don't return here, because we want to loop back to the top
          // and keep running this method.
        } else {
          // Give control of this exception to the caller.
          return NULL;
        }
      }
      break;
    case cExit:
      return NULL;
    default:
      std::cout << "bug!\n";
      call_frame->print_backtrace(state);
      abort();
    } // switch
  } // for(;;)
}
