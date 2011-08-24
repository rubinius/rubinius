#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/autoload.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/proc.hpp"
#include "builtin/thread.hpp"
#include "builtin/system.hpp"
#include "builtin/global_cache_entry.hpp"
#include "builtin/location.hpp"

#include "call_frame.hpp"

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "instructions.hpp"
#include "configuration.hpp"

#include "helpers.hpp"
#include "inline_cache.hpp"

#include "vm/gen/instruction_defines.hpp"

#define interp_assert(code) if(!(code)) { Exception::internal_error(state, call_frame, "assertion failed: " #code); RUN_EXCEPTION(); }

using namespace rubinius;

#define STACK_PTR stack_ptr

/** We have to use the local here we need to evaluate val before we alter
 * the stack. The reason is evaluating val might throw an exception. The
 * old code used an undefined behavior, this forces the order. */
#define stack_push(val) ({ Object* __stack_v = (val); *++STACK_PTR = __stack_v; })
#define stack_pop() (*STACK_PTR--)
#define stack_set_top(val) *STACK_PTR = (val)

#define USE_JUMP_TABLE

#define stack_top() (*STACK_PTR)
#define stack_back(count) (*(STACK_PTR - count))
#define stack_clear(count) STACK_PTR -= count

#define stack_position(where) (STACK_PTR = call_frame->stk + where)
#define stack_calculate_sp() (STACK_PTR - call_frame->stk)
#define stack_back_position(count) (STACK_PTR - (count - 1))

#define stack_local(which) call_frame->stk[vmm->stack_size - which - 1]

#define next_int ((opcode)(stream[call_frame->inc_ip()]))

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define CHECK_EXCEPTION(val) if(val == NULL) { goto exception; }

#define JUMP_DEBUGGING \
  return VMMethod::debugger_interpreter_continue(state, vmm, call_frame, \
         stack_calculate_sp(), is, current_unwind, unwinds)

#define CHECK_AND_PUSH(val) \
   if(val == NULL) { goto exception; } \
   else { stack_push(val); if(vmm->debugging) JUMP_DEBUGGING; }

#define RUN_EXCEPTION() goto exception

#define SET_CALL_FLAGS(val) is.call_flags = (val)
#define CALL_FLAGS() is.call_flags

#define SET_ALLOW_PRIVATE(val) is.allow_private = (val)
#define ALLOW_PRIVATE() is.allow_private

Object* VMMethod::interpreter(STATE,
                              VMMethod* const vmm,
                              InterpreterCallFrame* const call_frame)
{

#include "vm/gen/instruction_locations.hpp"

  if(unlikely(state == 0)) {
    VMMethod::instructions = const_cast<void**>(insn_locations);
    return NULL;
  }

  InterpreterState is;

#ifdef X86_ESI_SPEEDUP
  register void** ip_ptr asm ("esi") = vmm->addresses;
#else
  register void** ip_ptr = vmm->addresses;
#endif

  Object** stack_ptr = call_frame->stk - 1;

  int current_unwind = 0;
  UnwindInfo unwinds[kMaxUnwindInfos];

continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH goto **ip_ptr++

#undef next_int
#define next_int ((opcode)(*ip_ptr++))

#define cache_ip(which) ip_ptr = vmm->addresses + which
#define flush_ip() call_frame->calculate_ip(ip_ptr)

#include "vm/gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->thread_state()->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->thread_state()->raise_exception(exc.exception);
    return NULL;
  }

  // There is no reason to be here. Either the bytecode loop exits,
  // or it jumps to exception;
  abort();

  // If control finds it's way down here, there is an exception.
exception:
  ThreadState* th = state->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(current_unwind > 0) {
      UnwindInfo* info = &unwinds[--current_unwind];
      stack_position(info->stack_depth);
      call_frame->set_ip(info->target_ip);
      cache_ip(info->target_ip);
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

  std::cout << "bug!\n";
  call_frame->print_backtrace(state);
  abort();
  return NULL;
}

Object* VMMethod::uncommon_interpreter(STATE,
                                       VMMethod* const vmm,
                                       CallFrame* const call_frame,
                                       int32_t entry_ip,
                                       native_int sp,
                                       CallFrame* const method_call_frame,
                                       int32_t unwind_count,
                                       int32_t* input_unwinds)
{

  VMMethod* method_vmm = method_call_frame->cm->backend_method();

  if(++method_vmm->uncommon_count > state->shared.config.jit_deoptimize_threshold) {
    if(state->shared.config.jit_show_uncommon) {
      std::cerr << "[[[ Deoptimizing uncommon method ]]]\n";
      call_frame->print_backtrace(state);

      std::cerr << "Method Call Frame:\n";
      method_call_frame->print_backtrace(state);
    }

    method_vmm->uncommon_count = 0;
    method_vmm->deoptimize(state, method_call_frame->cm);
  }

#include "vm/gen/instruction_locations.hpp"

  opcode* stream = vmm->opcodes;
  InterpreterState is;

  Object** stack_ptr = call_frame->stk + sp;

  int current_unwind = unwind_count;
  UnwindInfo unwinds[kMaxUnwindInfos];

  for(int i = 0, j = 0; j < unwind_count; i += 3, j++) {
    UnwindInfo& uw = unwinds[j];
    uw.target_ip = input_unwinds[i];
    uw.stack_depth = input_unwinds[i + 1];
    uw.type = (UnwindType)input_unwinds[i + 2];
  }

continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef cache_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define cache_ip(which)
#define flush_ip()

#include "vm/gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->thread_state()->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->thread_state()->raise_exception(exc.exception);
    return NULL;
  }

  // No reason to be here!
  abort();

exception:
  ThreadState* th = state->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(current_unwind > 0) {
      UnwindInfo* info = &unwinds[--current_unwind];
      stack_position(info->stack_depth);
      call_frame->set_ip(info->target_ip);
      cache_ip(info->target_ip);
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

  std::cout << "bug!\n";
  call_frame->print_backtrace(state);
  abort();
  return NULL;
}

#undef CHECK_AND_PUSH
#define CHECK_AND_PUSH(val) if(val == NULL) { goto exception; } \
   else { stack_push(val); }


/* The debugger interpreter loop is used to run a method when a breakpoint
 * has been set. It has additional overhead, since it needs to inspect
 * each opcode for the breakpoint flag. It is installed on the VMMethod when
 * a breakpoint is set on compiled method.
 */
Object* VMMethod::debugger_interpreter(STATE,
                                       VMMethod* const vmm,
                                       InterpreterCallFrame* const call_frame)
{

#include "vm/gen/instruction_locations.hpp"

  opcode* stream = vmm->opcodes;
  InterpreterState is;

  int current_unwind = 0;
  UnwindInfo unwinds[kMaxUnwindInfos];

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
      if(!Helpers::yield_debugger(state, call_frame, bp)) goto exception; \
    } \
    goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef cache_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define cache_ip(which)
#define flush_ip()

#include "vm/gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->thread_state()->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->thread_state()->raise_exception(exc.exception);
    return NULL;
  }

  // no reason to be here!
  abort();

  // If control finds it's way down here, there is an exception.
exception:
  ThreadState* th = state->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(current_unwind > 0) {
      UnwindInfo* info = &unwinds[--current_unwind];
      stack_position(info->stack_depth);
      call_frame->set_ip(info->target_ip);
      cache_ip(info->target_ip);
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

  std::cout << "bug!\n";
  call_frame->print_backtrace(state);
  abort();
  return NULL;
}

Object* VMMethod::debugger_interpreter_continue(STATE,
                                       VMMethod* const vmm,
                                       CallFrame* const call_frame,
                                       int sp,
                                       InterpreterState& is,
                                       int current_unwind,
                                       UnwindInfo* unwinds)
{

#include "vm/gen/instruction_locations.hpp"

  opcode* stream = vmm->opcodes;

  Object** stack_ptr = call_frame->stk + sp;

continue_to_run:
  try {

#undef DISPATCH
#define DISPATCH \
    if(Object* bp = call_frame->find_breakpoint(state)) { \
      if(!Helpers::yield_debugger(state, call_frame, bp)) goto exception; \
    } \
    goto *insn_locations[stream[call_frame->inc_ip()]];

#undef next_int
#undef cache_ip
#undef flush_ip

#define next_int ((opcode)(stream[call_frame->inc_ip()]))
#define cache_ip(which)
#define flush_ip()

#include "vm/gen/instruction_implementations.hpp"

  } catch(TypeError& e) {
    flush_ip();
    Exception* exc =
      Exception::make_type_error(state, e.type, e.object, e.reason);
    exc->locations(state, Location::from_call_stack(state, call_frame));

    state->thread_state()->raise_exception(exc);
    call_frame->scope->flush_to_heap(state);
    return NULL;
  } catch(const RubyException& exc) {
    exc.exception->locations(state,
          Location::from_call_stack(state, call_frame));
    state->thread_state()->raise_exception(exc.exception);
    return NULL;
  }

  // No reason to be here!
  abort();

exception:
  ThreadState* th = state->thread_state();
  //
  switch(th->raise_reason()) {
  case cException:
    if(current_unwind > 0) {
      UnwindInfo* info = &unwinds[--current_unwind];
      stack_position(info->stack_depth);
      call_frame->set_ip(info->target_ip);
      cache_ip(info->target_ip);
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

  std::cout << "bug!\n";
  call_frame->print_backtrace(state);
  abort();
  return NULL;
}
