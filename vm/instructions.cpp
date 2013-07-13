#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/autoload.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/iseq.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/proc.hpp"
#include "builtin/thread.hpp"
#include "builtin/system.hpp"
#include "builtin/constant_cache.hpp"
#include "builtin/location.hpp"
#include "builtin/thread_state.hpp"
#include "builtin/call_site.hpp"
#include "builtin/variable_scope.hpp"

#include "instruments/tooling.hpp"

#include "call_frame.hpp"

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "instructions.hpp"
#include "configuration.hpp"
#include "on_stack.hpp"
#include "version.h"

#include "helpers.hpp"

#include "gen/instruction_defines.hpp"

#define interp_assert(code) if(!(code)) { Exception::internal_error(state, call_frame, "assertion failed: " #code); RUN_EXCEPTION(); }

using namespace rubinius;

#define STACK_PTR stack_ptr

/** We have to use the local here we need to evaluate val before we alter
 * the stack. The reason is evaluating val might throw an exception. The
 * old code used an undefined behavior, this forces the order. */
#define stack_push(val) ({ Object* __stack_v = (val); *++STACK_PTR = __stack_v; })
#define stack_pop() (*STACK_PTR--)
#define stack_set_top(val) *STACK_PTR = (val)

#define stack_top() (*STACK_PTR)
#define stack_back(count) (*(STACK_PTR - count))
#define stack_clear(count) STACK_PTR -= count

#define stack_position(where) (STACK_PTR = call_frame->stk + where)
#define stack_calculate_sp() (STACK_PTR - call_frame->stk)
#define stack_back_position(count) (STACK_PTR - (count - 1))

#define stack_local(which) call_frame->stk[mcode->stack_size - which - 1]

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define JUMP_DEBUGGING \
  return MachineCode::debugger_interpreter_continue(state, mcode, call_frame, \
         stack_calculate_sp(), is, unwinds)

#define CHECK_AND_PUSH(val) \
   if(val == NULL) { goto exception; } \
   else { stack_push(val); if(mcode->debugging) JUMP_DEBUGGING; }

#define RUN_EXCEPTION() goto exception

#define SET_CALL_FLAGS(val) is.call_flags = (val)
#define CALL_FLAGS() is.call_flags

Object* MachineCode::interpreter(STATE,
                                 MachineCode* const mcode,
                                 InterpreterCallFrame* const call_frame)
{

#include "gen/instruction_locations.hpp"

  InterpreterState is;
  GCTokenImpl gct;

  register opcode* ip_ptr = mcode->opcodes;
  register Object** stack_ptr = call_frame->stk - 1;

  UnwindInfoSet unwinds;

continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH goto *insn_locations[*ip_ptr++];

#undef next_int
#undef store_ip
#undef flush_ip

#define next_int ((opcode)(*ip_ptr++))
#define store_ip(which) ip_ptr = mcode->opcodes + which
#define flush_ip() call_frame->set_ip(ip_ptr - mcode->opcodes)

#include "gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->raise_exception(exc.exception);
    return NULL;
  }

  // There is no reason to be here. Either the bytecode loop exits,
  // or it jumps to exception;
  rubinius::bug("Control flow error in interpreter");

  // If control finds it's way down here, there is an exception.
exception:
  VMThreadState* th = state->vm()->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);
      call_frame->set_ip(info.target_ip);
      store_ip(info.target_ip);
      goto continue_to_run;
    } else {
      call_frame->scope->flush_to_heap(state);
      return NULL;
    }

  case cBreak:
    // If we're trying to break to here, we're done!
    if(th->destination_scope() == call_frame->scope->on_heap()) {
      stack_push(th->raise_value());
      th->clear_break();
      goto continue_to_run;
      // Don't return here, because we want to loop back to the top
      // and keep running this method.
    }

    // Otherwise, fall through and run the unwinds
  case cReturn:
  case cCatchThrow:
  case cThreadKill:
    // Otherwise, we're doing a long return/break unwind through
    // here. We need to run ensure blocks.
    while(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      if(info.for_ensure()) {
        stack_position(info.stack_depth);
        call_frame->set_ip(info.target_ip);
        store_ip(info.target_ip);

        // Don't reset ep here, we're still handling the return/break.
        goto continue_to_run;
      }
    }

    // Ok, no ensures to run.
    if(th->raise_reason() == cReturn) {
      call_frame->scope->flush_to_heap(state);

      // If we're trying to return to here, we're done!
      if(th->destination_scope() == call_frame->scope->on_heap()) {
        Object* val = th->raise_value();
        th->clear_return();
        return val;
      } else {
        // Give control of this exception to the caller.
        return NULL;
      }

    } else { // Not for us!
      call_frame->scope->flush_to_heap(state);
      // Give control of this exception to the caller.
      return NULL;
    }

  case cExit:
    call_frame->scope->flush_to_heap(state);
    return NULL;
  default:
    break;
  } // switch

  rubinius::bug("Control flow error in interpreter");
  return NULL;
}

Object* MachineCode::uncommon_interpreter(STATE,
                                          MachineCode* const mcode,
                                          CallFrame* const call_frame,
                                          int32_t entry_ip,
                                          native_int sp,
                                          CallFrame* const method_call_frame,
                                          jit::RuntimeDataHolder* rd,
                                          UnwindInfoSet& thread_unwinds,
                                          bool force_deoptimize)
{

  MachineCode* mc = method_call_frame->compiled_code->machine_code();

  if(force_deoptimize || ++mc->uncommon_count > state->shared().config.jit_deoptimize_threshold) {
    if(state->shared().config.jit_uncommon_print) {
      std::cerr << "[[[ Deoptimizing uncommon method ]]]\n";
      call_frame->print_backtrace(state);

      std::cerr << "Method Call Frame:\n";
      method_call_frame->print_backtrace(state);
    }

    mc->uncommon_count = 0;
    mc->call_count = 1;
    mc->deoptimize(state, method_call_frame->compiled_code, rd);
  }

#include "gen/instruction_locations.hpp"

  opcode* stream = mcode->opcodes;
  InterpreterState is;
  GCTokenImpl gct;

  Object** stack_ptr = call_frame->stk + sp;

  UnwindInfoSet unwinds(thread_unwinds);
continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef store_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define store_ip(which) call_frame->set_ip(which)
#define flush_ip()

#include "gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->raise_exception(exc.exception);
    return NULL;
  }

  // No reason to be here!
  rubinius::bug("Control flow error in interpreter");

exception:
  VMThreadState* th = state->vm()->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);
      call_frame->set_ip(info.target_ip);
      store_ip(info.target_ip);
      goto continue_to_run;
    } else {
      call_frame->scope->flush_to_heap(state);
      return NULL;
    }

  case cBreak:
    // If we're trying to break to here, we're done!
    if(th->destination_scope() == call_frame->scope->on_heap()) {
      stack_push(th->raise_value());
      th->clear_break();
      goto continue_to_run;
      // Don't return here, because we want to loop back to the top
      // and keep running this method.
    }

    // Otherwise, fall through and run the unwinds
  case cReturn:
  case cCatchThrow:
  case cThreadKill:
    // Otherwise, we're doing a long return/break unwind through
    // here. We need to run ensure blocks.
    while(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      if(info.for_ensure()) {
        stack_position(info.stack_depth);
        call_frame->set_ip(info.target_ip);
        store_ip(info.target_ip);

        // Don't reset ep here, we're still handling the return/break.
        goto continue_to_run;
      }
    }

    // Ok, no ensures to run.
    if(th->raise_reason() == cReturn) {
      call_frame->scope->flush_to_heap(state);

      // If we're trying to return to here, we're done!
      if(th->destination_scope() == call_frame->scope->on_heap()) {
        Object* val = th->raise_value();
        th->clear_return();
        return val;
      } else {
        // Give control of this exception to the caller.
        return NULL;
      }

    } else { // It's cBreak thats not for us!
      call_frame->scope->flush_to_heap(state);
      // Give control of this exception to the caller.
      return NULL;
    }

  case cExit:
    call_frame->scope->flush_to_heap(state);
    return NULL;
  default:
    break;
  } // switch

  rubinius::bug("Control flow error in interpreter");
  return NULL;
}

#undef CHECK_AND_PUSH
#define CHECK_AND_PUSH(val) if(val == NULL) { goto exception; } \
   else { stack_push(val); }


/* The debugger interpreter loop is used to run a method when a breakpoint
 * has been set. It has additional overhead, since it needs to inspect
 * each opcode for the breakpoint flag. It is installed on the MachineCode when
 * a breakpoint is set on compiled method.
 */
Object* MachineCode::debugger_interpreter(STATE,
                                          MachineCode* const mcode,
                                          InterpreterCallFrame* const call_frame)
{

#include "gen/instruction_locations.hpp"

  opcode* stream = mcode->opcodes;
  InterpreterState is;
  GCTokenImpl gct;

  UnwindInfoSet unwinds;

  // TODO: ug, cut and paste of the whole interpreter above. Needs to be fast,
  // maybe could use a function template?
  //
  // The only thing different is the DISPATCH macro, to check for debugging
  // instructions.

  Object** stack_ptr = call_frame->stk - 1;

continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH \
    if(Object* bp = call_frame->find_breakpoint(state)) { \
      if(!Helpers::yield_debugger(state, gct, call_frame, bp)) goto exception; \
    } \
    goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef store_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define store_ip(which) call_frame->set_ip(which)
#define flush_ip()

#include "gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->raise_exception(exc.exception);
    return NULL;
  }

  // no reason to be here!
  rubinius::bug("Control flow error in interpreter");

  // If control finds it's way down here, there is an exception.
exception:
  VMThreadState* th = state->vm()->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);
      call_frame->set_ip(info.target_ip);
      store_ip(info.target_ip);
      goto continue_to_run;
    } else {
      call_frame->scope->flush_to_heap(state);
      return NULL;
    }

  case cBreak:
    // If we're trying to break to here, we're done!
    if(th->destination_scope() == call_frame->scope->on_heap()) {
      stack_push(th->raise_value());
      th->clear_break();
      goto continue_to_run;
      // Don't return here, because we want to loop back to the top
      // and keep running this method.
    }

    // Otherwise, fall through and run the unwinds
  case cReturn:
  case cCatchThrow:
  case cThreadKill:
    // Otherwise, we're doing a long return/break unwind through
    // here. We need to run ensure blocks.
    while(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);

      if(info.for_ensure()) {
        stack_position(info.stack_depth);
        call_frame->set_ip(info.target_ip);
        store_ip(info.target_ip);

        // Don't reset ep here, we're still handling the return/break.
        goto continue_to_run;
      }
    }

    // Ok, no ensures to run.
    if(th->raise_reason() == cReturn) {
      call_frame->scope->flush_to_heap(state);

      // If we're trying to return to here, we're done!
      if(th->destination_scope() == call_frame->scope->on_heap()) {
        Object* val = th->raise_value();
        th->clear_return();
        return val;
      } else {
        // Give control of this exception to the caller.
        return NULL;
      }

    } else { // It's cBreak thats not for us!
      call_frame->scope->flush_to_heap(state);
      // Give control of this exception to the caller.
      return NULL;
    }

  case cExit:
    call_frame->scope->flush_to_heap(state);
    return NULL;
  default:
    break;
  } // switch

  rubinius::bug("Control flow error in interpreter");
  return NULL;
}

Object* MachineCode::debugger_interpreter_continue(STATE,
                                                   MachineCode* const mcode,
                                                   CallFrame* const call_frame,
                                                   int sp,
                                                   InterpreterState& is,
                                                   UnwindInfoSet& thread_unwinds)
{

#include "gen/instruction_locations.hpp"

  GCTokenImpl gct;
  opcode* stream = mcode->opcodes;

  Object** stack_ptr = call_frame->stk + sp;

  UnwindInfoSet unwinds(thread_unwinds);
continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH \
    if(Object* bp = call_frame->find_breakpoint(state)) { \
      if(!Helpers::yield_debugger(state, gct, call_frame, bp)) goto exception; \
    } \
    goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef store_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define store_ip(which) call_frame->set_ip(which)
#define flush_ip()

#include "gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->raise_exception(exc.exception);
    return NULL;
  }

  // No reason to be here!
  rubinius::bug("Control flow error in interpreter");

exception:
  VMThreadState* th = state->vm()->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);
      call_frame->set_ip(info.target_ip);
      store_ip(info.target_ip);
      goto continue_to_run;
    } else {
      call_frame->scope->flush_to_heap(state);
      return NULL;
    }

  case cBreak:
    // If we're trying to break to here, we're done!
    if(th->destination_scope() == call_frame->scope->on_heap()) {
      stack_push(th->raise_value());
      th->clear_break();
      goto continue_to_run;
      // Don't return here, because we want to loop back to the top
      // and keep running this method.
    }

    // Otherwise, fall through and run the unwinds
  case cReturn:
  case cCatchThrow:
  case cThreadKill:
    // Otherwise, we're doing a long return/break unwind through
    // here. We need to run ensure blocks.
    while(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      if(info.for_ensure()) {
        stack_position(info.stack_depth);
        call_frame->set_ip(info.target_ip);
        store_ip(info.target_ip);

        // Don't reset ep here, we're still handling the return/break.
        goto continue_to_run;
      }
    }

    // Ok, no ensures to run.
    if(th->raise_reason() == cReturn) {
      call_frame->scope->flush_to_heap(state);

      // If we're trying to return to here, we're done!
      if(th->destination_scope() == call_frame->scope->on_heap()) {
        Object* val = th->raise_value();
        th->clear_return();
        return val;
      } else {
        // Give control of this exception to the caller.
        return NULL;
      }

    } else { // It's cBreak thats not for us!
      call_frame->scope->flush_to_heap(state);
      // Give control of this exception to the caller.
      return NULL;
    }

  case cExit:
    call_frame->scope->flush_to_heap(state);
    return NULL;
  default:
    break;
  } // switch

  rubinius::bug("Control flow error in interpreter");
  return NULL;
}


/* The tooling interpreter calls the registered tool for every instruction
 * executed.
 */
Object* MachineCode::tooling_interpreter(STATE,
                                         MachineCode* const mcode,
                                         InterpreterCallFrame* const call_frame)
{

#include "gen/instruction_locations.hpp"

  opcode* stream = mcode->opcodes;
  InterpreterState is;
  GCTokenImpl gct;

  UnwindInfoSet unwinds;

  Object** stack_ptr = call_frame->stk - 1;

continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH \
    state->shared().tool_broker()->at_ip(state, mcode, call_frame->ip()); \
    goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef store_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define store_ip(which) call_frame->set_ip(which)
#define flush_ip()

#include "gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->raise_exception(exc.exception);
    return NULL;
  }

  // no reason to be here!
  rubinius::bug("Control flow error in interpreter");

  // If control finds it's way down here, there is an exception.
exception:
  VMThreadState* th = state->vm()->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);
      call_frame->set_ip(info.target_ip);
      store_ip(info.target_ip);
      goto continue_to_run;
    } else {
      call_frame->scope->flush_to_heap(state);
      return NULL;
    }

  case cBreak:
    // If we're trying to break to here, we're done!
    if(th->destination_scope() == call_frame->scope->on_heap()) {
      stack_push(th->raise_value());
      th->clear_break();
      goto continue_to_run;
      // Don't return here, because we want to loop back to the top
      // and keep running this method.
    }

    // Otherwise, fall through and run the unwinds
  case cReturn:
  case cCatchThrow:
  case cThreadKill:
    // Otherwise, we're doing a long return/break unwind through
    // here. We need to run ensure blocks.
    while(unwinds.has_unwinds()) {
      UnwindInfo info = unwinds.pop();
      stack_position(info.stack_depth);

      if(info.for_ensure()) {
        stack_position(info.stack_depth);
        call_frame->set_ip(info.target_ip);
        store_ip(info.target_ip);

        // Don't reset ep here, we're still handling the return/break.
        goto continue_to_run;
      }
    }

    // Ok, no ensures to run.
    if(th->raise_reason() == cReturn) {
      call_frame->scope->flush_to_heap(state);

      // If we're trying to return to here, we're done!
      if(th->destination_scope() == call_frame->scope->on_heap()) {
        Object* val = th->raise_value();
        th->clear_return();
        return val;
      } else {
        // Give control of this exception to the caller.
        return NULL;
      }

    } else { // It's cBreak thats not for us!
      call_frame->scope->flush_to_heap(state);
      // Give control of this exception to the caller.
      return NULL;
    }

  case cExit:
    call_frame->scope->flush_to_heap(state);
    return NULL;
  default:
    break;
  } // switch

  rubinius::bug("Control flow error in interpreter");
  return NULL;
}

