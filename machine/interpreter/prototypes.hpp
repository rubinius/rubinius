#ifndef RBX_INTERPRETER_PROTOTYPES_HPP
#define RBX_INTERPRETER_PROTOTYPES_HPP

namespace rubinius {
extern "C" {

intptr_t int_add_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_allow_private(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_cast_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_cast_for_multi_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_cast_for_single_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_cast_for_splat_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_cast_multi_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_check_frozen(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_check_interrupts(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_check_serial(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_check_serial_private(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_clear_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_create_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_dup_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_ensure_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_find_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_not_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_not_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_not_undefined(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_goto_if_undefined(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_instance_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_invoke_primitive(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_kind_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_make_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_move_down(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_noop(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_object_to_s(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_passed_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_passed_blockarg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_pop(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_pop_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_pop_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_cpath_top(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_current_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_has_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_literal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_memo(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_mirror(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_my_field(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_my_offset(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_proc(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_rubinius(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_self(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_stack_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_undef(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_push_variables(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_raise_break(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_raise_exc(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_raise_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_reraise(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_restore_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_rotate(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_stack_with_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_stack_with_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_super_stack_with_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_super_stack_with_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_send_vcall(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_call_flags(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_const_at(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_set_stack_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_setup_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_shift_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_store_my_field(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_string_append(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_string_build(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_string_dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_swap(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_yield_debugger(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_yield_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_yield_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
intptr_t int_zsuper(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

}
}

#endif
