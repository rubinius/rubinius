#ifndef RBX_INTERPRETER_PROTOTYPES_HPP
#define RBX_INTERPRETER_PROTOTYPES_HPP

namespace rubinius {
  namespace interpreter {
    using namespace rubinius;

    intptr_t add_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t allow_private(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t cast_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t cast_for_multi_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t cast_for_single_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t cast_for_splat_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t cast_multi_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t check_frozen(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t check_interrupts(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t check_serial(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t check_serial_private(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t clear_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t create_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t dup_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t ensure_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t find_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_not_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_not_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_not_undefined(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_if_undefined(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t instance_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t invoke_primitive(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t kind_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t make_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t move_down(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t noop(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t object_to_s(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t passed_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t passed_blockarg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t pop(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t pop_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t pop_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_cpath_top(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_current_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_has_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_literal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_memo(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_mirror(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_my_field(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_my_offset(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_proc(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_rubinius(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_self(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_stack_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_undef(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_variables(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t raise_break(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t raise_exc(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t raise_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t reraise(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t restore_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t rotate(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_stack_with_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_stack_with_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_super_stack_with_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_super_stack_with_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t send_vcall(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_call_flags(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_const_at(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t set_stack_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t setup_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t shift_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t store_my_field(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t string_append(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t string_build(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t string_dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t swap(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t yield_debugger(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t yield_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t yield_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t zsuper(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t push_file(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // PEG instructions
    intptr_t p_any(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_call(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_char(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_char_set(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_choice(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_commit(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_commit_back(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_commit_partial(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_end(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_fail(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_fail_twice(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_jump(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_span(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_test_any(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_test_char(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_test_char_set(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t p_init(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Instrumentation instructions
    intptr_t m_bytes(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_counter(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_sum(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_time_stamp(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_timer_start(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_timer_stop(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
  }
}

#endif
