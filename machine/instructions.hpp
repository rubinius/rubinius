#ifndef RBX_INSTRUCTIONS_HPP
#define RBX_INSTRUCTIONS_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "machine_code.hpp"

#include "instructions/data.hpp"

namespace rubinius {
  namespace instructions {
    using namespace rubinius;

    typedef enum {
      cExceptionRescue,
      cExceptionEnsure,
      cExceptionBreak,
      cExceptionReturn,
      cExceptionUnwind,
    } ExceptionContinuation;

    typedef intptr_t (*Instruction)(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

#define CALL_FLAG_CONCAT 2

#define argument(n)       opcodes[call_frame->ip() + n + 1]

#define store_literal(f, x)  (f->machine_code->opcodes[f->ip()+1] = (x))

#define STACK_PTR call_frame->stack_ptr_

/* We have to use the local here we need to evaluate val before we alter the
 * stack. The reason is evaluating val might throw an exception. The old code
 * used an undefined behavior, this forces the order.
 */
#define stack_push(val) ({ Object* __stack_v = (val); *++STACK_PTR = __stack_v; })
#define stack_pop() (*STACK_PTR--)
#define stack_set_top(val) *STACK_PTR = (val)

#define stack_top() (*STACK_PTR)
#define stack_back(count) (*(STACK_PTR - count))
#define stack_clear(count) STACK_PTR -= count

#define stack_position(where) (STACK_PTR = call_frame->stk + where)
#define stack_calculate_sp() (STACK_PTR - call_frame->stk)
#define stack_back_position(count) (STACK_PTR - (count - 1))

#define stack_local(which) call_frame->stk[call_frame->machine_code->stack_size - which - 1]

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define CHECK_AND_PUSH(val) \
  if(val == NULL) { \
    return false; \
  } else { \
    stack_push(val); \
    return true; \
  }

#define interp_assert(code) if(!(code)) { \
  Exception::internal_error(state, "assertion failed: " #code); \
}

#define SET_CALL_FLAGS(val) call_frame->is->call_flags = (val)
#define CALL_FLAGS()        call_frame->is->call_flags

    inline void add_scope(STATE, CallFrame* call_frame);
    inline void allow_private();
    inline bool cast_array(STATE, CallFrame* call_frame);
    inline bool cast_for_multi_block_arg(STATE, CallFrame* call_frame);
    inline bool cast_for_single_block_arg(STATE, CallFrame* call_frame);
    inline bool cast_for_splat_block_arg(STATE, CallFrame* call_frame);
    inline bool cast_multi_value(STATE, CallFrame* call_frame);
    inline bool check_frozen(STATE, CallFrame* call_frame);
    inline void check_interrupts(STATE);
    inline void check_serial(STATE, CallFrame* call_frame, intptr_t literal, intptr_t serial);
    inline void check_serial_private(STATE, CallFrame* call_frame, intptr_t literal, intptr_t serial);
    inline void clear_exception(STATE);
    inline bool create_block(STATE, CallFrame* call_frame, intptr_t literal);
    inline void dup(CallFrame* call_frame);
    inline void dup_many(CallFrame* call_frame, intptr_t count);
    inline intptr_t ensure_return(STATE, CallFrame* call_frame);
    inline bool find_const(STATE, CallFrame* call_frame, intptr_t literal);
    inline void goto_(CallFrame* call_frame);
    inline bool goto_if_equal(CallFrame* call_frame);
    inline bool goto_if_false(CallFrame* call_frame);
    inline bool goto_if_nil(CallFrame* call_frame);
    inline bool goto_if_not_equal(CallFrame* call_frame);
    inline bool goto_if_not_nil(CallFrame* call_frame);
    inline bool goto_if_not_undefined(STATE, CallFrame* call_frame);
    inline bool goto_if_true(CallFrame* call_frame);
    inline bool goto_if_undefined(STATE, CallFrame* call_frame);
    inline void instance_of(STATE, CallFrame* call_frame);
    inline bool invoke_primitive(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count);
    inline void kind_of(STATE, CallFrame* call_frame);
    inline void make_array(STATE, CallFrame* call_frame, intptr_t count);
    inline void move_down(CallFrame* call_frame, intptr_t positions);
    inline void noop();
    inline bool object_to_s(STATE, CallFrame* call_frame, intptr_t literal);
    inline bool passed_arg(STATE, CallFrame* call_frame, intptr_t index);
    inline bool passed_blockarg(STATE, CallFrame* call_frame, intptr_t count);
    inline void pop(CallFrame* call_frame);
    inline void pop_many(CallFrame* call_frame, intptr_t count);
    inline void pop_unwind(STATE, CallFrame* call_frame);
    inline void push_block(STATE, CallFrame* call_frame);
    inline bool push_block_arg(STATE, CallFrame* call_frame);
    inline bool push_const(STATE, CallFrame* call_frame, intptr_t literal);
    inline void push_cpath_top(STATE, CallFrame* call_frame);
    inline void push_current_exception(STATE, CallFrame* call_frame);
    inline void push_exception_state(STATE, CallFrame* call_frame);
    inline void push_false(CallFrame* call_frame);
    inline void push_has_block(CallFrame* call_frame);
    inline void push_int(CallFrame* call_frame, intptr_t number);
    inline bool push_ivar(STATE, CallFrame* call_frame, intptr_t literal);
    inline void push_literal(CallFrame* call_frame, intptr_t literal);
    inline void push_local(CallFrame* call_frame, intptr_t local);
    inline bool push_local_depth(STATE, CallFrame* call_frame, intptr_t depth, intptr_t index);
    inline void push_memo(CallFrame* call_frame, intptr_t literal);
    inline void push_mirror(STATE, CallFrame* call_frame);
    inline void push_my_field(STATE, CallFrame* call_frame, intptr_t index);
    inline void push_my_offset(CallFrame* call_frame, intptr_t index);
    inline void push_nil(CallFrame* call_frame);
    inline bool push_proc(STATE, CallFrame* call_frame);
    inline void push_rubinius(STATE, CallFrame* call_frame);
    inline void push_scope(CallFrame* call_frame);
    inline void push_self(CallFrame* call_frame);
    inline void push_stack_local(CallFrame* call_frame, intptr_t which);
    inline void push_true(CallFrame* call_frame);
    inline void push_type(STATE, CallFrame* call_frame);
    inline void push_undef(STATE, CallFrame* call_frame);
    inline void push_variables(STATE, CallFrame* call_frame);
    inline intptr_t raise_break(STATE, CallFrame* call_frame);
    inline intptr_t raise_exc(STATE, CallFrame* call_frame);
    inline intptr_t raise_return(STATE, CallFrame* call_frame);
    inline intptr_t reraise(STATE, CallFrame* call_frame);
    inline void restore_exception_state(STATE, CallFrame* call_frame);
    inline intptr_t ret(STATE, CallFrame* call_frame);
    inline void rotate(CallFrame* call_frame, intptr_t count);
    inline ExceptionContinuation unwind(STATE, CallFrame* call_frame);
    inline bool send_method(STATE, CallFrame* call_frame, intptr_t literal);
    inline bool send_stack(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count);
    inline bool send_stack_with_block(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count);
    inline bool send_stack_with_splat(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count);
    inline bool send_super_stack_with_block(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count);
    inline bool send_super_stack_with_splat(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count);
    inline bool send_vcall(STATE, CallFrame* call_frame, intptr_t literal);
    inline void set_call_flags(CallFrame* call_frame, intptr_t flags);
    inline void set_const(STATE, CallFrame* call_frame, intptr_t literal);
    inline void set_const_at(STATE, CallFrame* call_frame, intptr_t literal);
    inline bool set_ivar(STATE, CallFrame* call_frame, intptr_t literal);
    inline void set_local(CallFrame* call_frame, intptr_t local);
    inline bool set_local_depth(STATE, CallFrame* call_frame, intptr_t depth, intptr_t index);
    inline void set_stack_local(CallFrame* call_frame, intptr_t which);
    inline void setup_unwind(CallFrame* call_frame, intptr_t ip, intptr_t type);
    inline void shift_array(STATE, CallFrame* call_frame);
    inline bool store_my_field(STATE, CallFrame* call_frame, intptr_t index);
    inline void string_append(STATE, CallFrame* call_frame);
    inline bool string_build(STATE, CallFrame* call_frame, intptr_t count);
    inline bool string_dup(STATE, CallFrame* call_frame);
    inline void swap(CallFrame* call_frame);
    inline void yield_debugger(STATE);
    inline bool yield_splat(STATE, CallFrame* call_frame, intptr_t count);
    inline bool yield_stack(STATE, CallFrame* call_frame, intptr_t count);
    inline bool zsuper(STATE, CallFrame* call_frame, intptr_t literal);
    inline void push_file(STATE, CallFrame* call_frame);

    // PEG instructions
    inline bool p_any(STATE, CallFrame* call_frame, const intptr_t n);
    inline void p_call(STATE, CallFrame* call_frame);
    inline bool p_char(STATE, CallFrame* call_frame, const intptr_t chr);
    inline bool p_char_set(STATE, CallFrame* call_frame, const intptr_t chr_set);
    inline void p_choice(STATE, CallFrame* call_frame, const intptr_t ip, const intptr_t index);
    inline void p_commit(STATE, CallFrame* call_frame);
    inline void p_commit_back(STATE, CallFrame* call_frame);
    inline void p_commit_partial(STATE, CallFrame* call_frame);
    inline intptr_t p_end(STATE, CallFrame* call_frame);
    inline intptr_t p_fail(STATE, CallFrame* call_frame);
    inline void p_fail_twice(STATE, CallFrame* call_frame);
    inline void p_jump(STATE, CallFrame* call_frame);
    inline intptr_t p_return(STATE, CallFrame* call_frame);
    inline bool p_span(STATE, CallFrame* call_frame, const intptr_t chr_set);
    inline bool p_test_any(STATE, CallFrame* call_frame, const intptr_t n);
    inline bool p_test_char(STATE, CallFrame* call_frame, const intptr_t chr);
    inline bool p_test_char_set(STATE, CallFrame* call_frame, const intptr_t chr_set);
    inline void p_init(STATE, CallFrame* call_frame, const intptr_t subject, const intptr_t captures);

    // Instrumentation instructions
    inline void m_bytes(STATE, CallFrame* call_frame, intptr_t value, const intptr_t r0);
    inline void m_counter(STATE, CallFrame* call_frame, intptr_t value);
    inline void m_sum(STATE, CallFrame* call_frame, intptr_t value, const intptr_t r0);
    inline void m_value(STATE, CallFrame* call_frame, intptr_t value, const intptr_t r0);
    inline void m_time_stamp(STATE, CallFrame* call_frame, intptr_t value, const intptr_t flag);
    inline void m_timer_start(STATE, CallFrame* call_frame, intptr_t timer);
    inline void m_timer_stop(STATE, CallFrame* call_frame, const intptr_t ip, const intptr_t flag);
  }
}

#endif
