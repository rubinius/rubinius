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
    intptr_t goto_past(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t goto_future(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
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
    intptr_t push_tagged_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
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

    // Function instructions
    intptr_t call_send(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t call(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t call_0(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

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
    intptr_t m_log(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t m_debug(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Branching instructions
    intptr_t b_if_serial(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t b_if_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t b_if_eint(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t b_if_float(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t b_if(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Register movement instructions
    intptr_t r_load_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_literal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_float(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_float(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_ref_addr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_byte_addr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_ref(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_ref(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_byte(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_byte(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_handle(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_handle(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_store_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_index(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_self(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_neg1(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_0(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_1(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_2(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_bool(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_m_binops(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_load_f_binops(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_copy(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_refcnt_inc(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t r_refcnt_dec(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Native signed integer instructions
    intptr_t n_iadd(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_isub(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_imul(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_idiv(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_imod(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ineg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_iinc(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_idec(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ibits(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_isize(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_inot(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_iand(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ior(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ixor(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ishl(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ishr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_iadd_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_isub_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_imul_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_idiv_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_imod_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_idivmod(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ipow_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ineg_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ishl_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ishr_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_icmp(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ieq(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ine(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ilt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ile(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_igt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ige(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ipopcnt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_istr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_iflt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    intptr_t n_promote(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_demote(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Native signed extended integer instructions
    intptr_t n_eadd(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_esub(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_emul(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ediv(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_emod(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_edivmod(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_epow(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eneg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ebits(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_esize(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_enot(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eand(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eor(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_exor(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eshl(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eshr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_epopcnt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ecmp(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eeq(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ene(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_elt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ele(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_egt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ege(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_estr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_eflt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Native double floating point instructions
    intptr_t n_dadd(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dsub(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dmul(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ddiv(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dmod(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_ddivmod(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dpow(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dneg(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dcmp(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_deq(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dne(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dlt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dle(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dgt(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dge(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dstr(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dinf(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dnan(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dclass(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t n_dsign(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Code execution instructions
    intptr_t e_cache_method_p(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_cache_function_p(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_cache_value_p(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_cache_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_cache_function(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_cache_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_resolve_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_resolve_receiver_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_resolve_function(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_resolve_scope_constant(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_resolve_path_constant(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_signature(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_check_signature(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_invoke_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t e_invoke_function(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    // Assertion instructions
    intptr_t a_instance(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_kind(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_receiver_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_function(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_not_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_less(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_less_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_greater(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
    intptr_t a_greater_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]);
  }
}

#endif
