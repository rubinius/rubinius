#ifndef RBX_INSTRUCTIONS_HPP
#define RBX_INSTRUCTIONS_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "machine_code.hpp"

#include "instructions/data.hpp"

namespace rubinius {
  inline bool MemoryHeader::integer_p() const {
    return fixnum_p() || (reference_p() && try_as<Bignum>(this));
  }

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
#define REGISTERS call_frame->registers

#define REG(r)  (reinterpret_cast<intptr_t*>(REGISTERS)[r])
#define RVAL(r) (reinterpret_cast<Object**>(REGISTERS)[r])
#define RFLT(r)  (reinterpret_cast<double*>(REGISTERS)[r])

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

#define fixnums_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM_MASK == TAG_FIXNUM)

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

#define CF        CallFrame* call_frame
#define COUNT     const intptr_t count
#define LITERAL   const intptr_t literal
#define OBJECT    const intptr_t object
#define LOCAL     const intptr_t local
#define DEPTH     const intptr_t depth
#define INDEX     const intptr_t index
#define IP        const intptr_t IP
#define R0        const intptr_t r0
#define R1        const intptr_t r1
#define R2        const intptr_t r2

    inline void add_scope(STATE, CF);
    inline void allow_private();
    inline bool cast_array(STATE, CF);
    inline bool cast_for_multi_block_arg(STATE, CF);
    inline bool cast_for_single_block_arg(STATE, CF);
    inline bool cast_for_splat_block_arg(STATE, CF);
    inline bool cast_multi_value(STATE, CF);
    inline bool check_frozen(STATE, CF);
    inline void check_interrupts(STATE);
    inline void check_serial(STATE, CF, LITERAL, intptr_t serial);
    inline void check_serial_private(STATE, CF, LITERAL, intptr_t serial);
    inline void clear_exception(STATE);
    inline bool create_block(STATE, CF, LITERAL);
    inline void dup(CF);
    inline void dup_many(CF, COUNT);
    inline intptr_t ensure_return(STATE, CF);
    inline bool find_const(STATE, CF, LITERAL);
    inline void goto_(CF);
    inline void goto_past(STATE, CF);
    inline void goto_future(CF);
    inline bool goto_if_equal(CF);
    inline bool goto_if_false(CF);
    inline bool goto_if_nil(CF);
    inline bool goto_if_not_equal(CF);
    inline bool goto_if_not_nil(CF);
    inline bool goto_if_not_undefined(STATE, CF);
    inline bool goto_if_true(CF);
    inline bool goto_if_undefined(STATE, CF);
    inline void instance_of(STATE, CF);
    inline bool invoke_primitive(STATE, CF, LITERAL, COUNT);
    inline void kind_of(STATE, CF);
    inline void make_array(STATE, CF, COUNT);
    inline void move_down(CF, intptr_t positions);
    inline void noop();
    inline bool object_to_s(STATE, CF, LITERAL);
    inline bool passed_arg(STATE, CF, INDEX);
    inline bool passed_blockarg(STATE, CF, COUNT);
    inline void pop(CF);
    inline void pop_many(CF, COUNT);
    inline void pop_unwind(STATE, CF);
    inline void push_block(STATE, CF);
    inline bool push_block_arg(STATE, CF);
    inline bool push_const(STATE, CF, LITERAL);
    inline void push_cpath_top(STATE, CF);
    inline void push_current_exception(STATE, CF);
    inline void push_exception_state(STATE, CF);
    inline void push_false(CF);
    inline void push_has_block(CF);
    inline void push_int(CF, intptr_t number);
    inline bool push_ivar(STATE, CF, LITERAL);
    inline void push_literal(CF, LITERAL);
    inline void push_local(CF, LOCAL);
    inline bool push_local_depth(STATE, CF, DEPTH, INDEX);
    inline void push_memo(CF, LITERAL);
    inline void push_mirror(STATE, CF);
    inline void push_my_field(STATE, CF, INDEX);
    inline void push_my_offset(CF, INDEX);
    inline void push_nil(CF);
    inline void push_tagged_nil(CF, intptr_t nil);
    inline bool push_proc(STATE, CF);
    inline void push_rubinius(STATE, CF);
    inline void push_scope(CF);
    inline void push_self(CF);
    inline void push_stack_local(CF, intptr_t which);
    inline void push_true(CF);
    inline void push_type(STATE, CF);
    inline void push_undef(STATE, CF);
    inline void push_variables(STATE, CF);
    inline intptr_t raise_break(STATE, CF);
    inline intptr_t raise_exc(STATE, CF);
    inline intptr_t raise_return(STATE, CF);
    inline intptr_t reraise(STATE, CF);
    inline void restore_exception_state(STATE, CF);
    inline intptr_t ret(STATE, CF);
    inline void rotate(CF, COUNT);
    inline ExceptionContinuation unwind(STATE, CF);
    inline bool send_method(STATE, CF, LITERAL);
    inline bool send_stack(STATE, CF, LITERAL, COUNT);
    inline bool send_stack_with_block(STATE, CF, LITERAL, COUNT);
    inline bool send_stack_with_splat(STATE, CF, LITERAL, COUNT);
    inline bool send_super_stack_with_block(STATE, CF, LITERAL, COUNT);
    inline bool send_super_stack_with_splat(STATE, CF, LITERAL, COUNT);
    inline bool send_vcall(STATE, CF, LITERAL);
    inline void set_call_flags(CF, intptr_t flags);
    inline void set_const(STATE, CF, LITERAL);
    inline void set_const_at(STATE, CF, LITERAL);
    inline bool set_ivar(STATE, CF, LITERAL);
    inline void set_local(CF, LOCAL);
    inline bool set_local_depth(STATE, CF, DEPTH, INDEX);
    inline void set_stack_local(CF, intptr_t which);
    inline void setup_unwind(CF, IP, intptr_t type);
    inline void shift_array(STATE, CF);
    inline bool store_my_field(STATE, CF, INDEX);
    inline void string_append(STATE, CF);
    inline bool string_build(STATE, CF, COUNT);
    inline bool string_dup(STATE, CF);
    inline void swap(CF);
    inline void yield_debugger(STATE);
    inline bool yield_splat(STATE, CF, COUNT);
    inline bool yield_stack(STATE, CF, COUNT);
    inline bool zsuper(STATE, CF, LITERAL);
    inline void push_file(STATE, CF);

    // Function instructions
    inline bool call_send(STATE, CF, LITERAL, COUNT);
    inline bool call(STATE, CF, LITERAL, COUNT);
    inline bool call_0(STATE, CF, LITERAL);

    // PEG instructions
    inline bool p_any(STATE, CF, const intptr_t n);
    inline void p_call(STATE, CF);
    inline bool p_char(STATE, CF, const intptr_t chr);
    inline bool p_char_set(STATE, CF, const intptr_t chr_set);
    inline void p_choice(STATE, CF, IP, INDEX);
    inline void p_commit(STATE, CF);
    inline void p_commit_back(STATE, CF);
    inline void p_commit_partial(STATE, CF);
    inline intptr_t p_end(STATE, CF);
    inline intptr_t p_fail(STATE, CF);
    inline void p_fail_twice(STATE, CF);
    inline void p_jump(STATE, CF);
    inline intptr_t p_return(STATE, CF);
    inline bool p_span(STATE, CF, const intptr_t chr_set);
    inline bool p_test_any(STATE, CF, const intptr_t n);
    inline bool p_test_char(STATE, CF, const intptr_t chr);
    inline bool p_test_char_set(STATE, CF, const intptr_t chr_set);
    inline void p_init(STATE, CF, const intptr_t subject, const intptr_t captures);

    // Instrumentation instructions
    inline void m_bytes(STATE, CF, OBJECT, R0);
    inline void m_counter(STATE, CF, OBJECT);
    inline void m_sum(STATE, CF, OBJECT, R0);
    inline void m_value(STATE, CF, OBJECT, R0);
    inline void m_time_stamp(STATE, CF, OBJECT, const intptr_t flag);
    inline void m_timer_start(STATE, CF, intptr_t timer);
    inline void m_timer_stop(STATE, CF, IP, const intptr_t flag);
    inline void m_log(STATE, CF, R0);
    inline void m_debug(STATE, CF);

    // Branching instructions
    inline bool b_if_serial(CF, LITERAL, R0);
    inline bool b_if_int(CF, R0, R1);
    inline bool b_if_eint(CF, R0, R1);
    inline bool b_if_float(CF, R0, R1);
    inline bool b_if(CF, R0);

    // Register movement instructions
    inline void r_load_local(CF, R0, LOCAL);
    inline void r_store_local(CF, R0, LOCAL);
    inline void r_load_local_depth(CF, R0, LOCAL, DEPTH);
    inline void r_store_local_depth(CF, R0, LOCAL, DEPTH);
    inline void r_load_stack(CF, R0);
    inline void r_store_stack(CF, R0);
    inline void r_load_literal(CF, R0, LITERAL);
    inline void r_load_int(CF, R0, R1);
    inline void r_store_int(STATE, CF, R0, R1);
    inline void r_load_int(CF, R0, R1);
    inline void r_store_int(STATE, CF, R0, R1);
    inline void r_load_float(CF, R0, R1);
    inline void r_store_float(STATE, CF, R0, R1);
    inline void r_load_ref_addr(STATE, CF, R0, R1, R2);
    inline void r_load_byte_addr(STATE, CF, R0, R1, R2);
    inline void r_load_ref(CF, R0, R1, R2);
    inline void r_store_ref(CF, R0, R1, R2);
    inline void r_load_byte(CF, R0, R1, R2);
    inline void r_store_byte(CF, R0, R1, R2);
    inline void r_load_handle(STATE, CF, R0, R1);
    inline void r_store_handle(STATE, CF, R0, R1);
    inline void r_load_ivar(STATE, CF, R0, R1, LITERAL);
    inline void r_store_ivar(STATE, CF, R0, R1, LITERAL);
    inline void r_load_index(STATE, CF, R0, R1, LITERAL);
    inline void r_load_self(CF, R0);
    inline void r_load_neg1(CF, R0);
    inline void r_load_0(CF, R0);
    inline void r_load_1(CF, R0);
    inline void r_load_2(CF, R0);
    inline void r_load_nil(CF, R0, OBJECT);
    inline void r_load_false(CF, R0);
    inline void r_load_true(CF, R0);
    inline void r_load_bool(CF, R0, R1);
    inline void r_load_m_binops(CF, R0, R1);
    inline void r_load_f_binops(CF, R0, R1);
    inline void r_copy(CF, R0, R1);
    inline intptr_t r_ret(STATE, CF, R0);
    inline void r_refcnt_inc(STATE, CF, R0);
    inline void r_refcnt_dec(STATE, CF, R0);

    // Native signed integer instructions
    inline void n_iadd(CF, R0, R1, R2);
    inline void n_isub(CF, R0, R1, R2);
    inline void n_imul(CF, R0, R1, R2);
    inline void n_idiv(STATE, CF, R0, R1, R2);
    inline void n_imod(CF, R0, R1, R2);
    inline void n_ineg(CF, R0, R1);
    inline void n_inot(CF, R0, R1);
    inline void n_iinc(CF, R0, R1);
    inline void n_idec(CF, R0, R1);
    inline void n_ibits(CF, R0, R1);
    inline void n_isize(CF, R0, R1);
    inline void n_iand(CF, R0, R1, R2);
    inline void n_ior(CF, R0, R1, R2);
    inline void n_ixor(CF, R0, R1, R2);
    inline void n_ishl(CF, R0, R1, R2);
    inline void n_ishr(CF, R0, R1, R2);
    inline void n_iadd_o(STATE, CF, R0, R1, R2);
    inline void n_isub_o(STATE, CF, R0, R1, R2);
    inline void n_imul_o(STATE, CF, R0, R1, R2);
    inline void n_idiv_o(STATE, CF, R0, R1, R2);
    inline void n_idivmod(STATE, CF, R0, R1, R2);
    inline void n_ipow_o(STATE, CF, R0, R1, R2);
    inline void n_imod_o(STATE, CF, R0, R1, R2);
    inline void n_ineg_o(STATE, CF, R0, R1);
    inline void n_ishl_o(STATE, CF, R0, R1, R2);
    inline void n_ishr_o(STATE, CF, R0, R1, R2);
    inline void n_icmp(CF, R0, R1, R2);
    inline void n_ieq(CF, R0, R1, R2);
    inline void n_ine(CF, R0, R1, R2);
    inline void n_ilt(CF, R0, R1, R2);
    inline void n_ile(CF, R0, R1, R2);
    inline void n_igt(CF, R0, R1, R2);
    inline void n_ige(CF, R0, R1, R2);
    inline void n_ipopcnt(CF, R0, R1);
    inline void n_istr(STATE, CF, R0, R1, R2);
    inline void n_iflt(CF, R0, R1);

    inline void n_promote(STATE, CF, R0, R1, R2);
    inline void n_demote(STATE, CF, R0, R1, R2);

    // Native signed extended integer instructions
    inline void n_eadd(STATE, CF, R0, R1, R2);
    inline void n_esub(STATE, CF, R0, R1, R2);
    inline void n_emul(STATE, CF, R0, R1, R2);
    inline void n_ediv(STATE, CF, R0, R1, R2);
    inline void n_emod(STATE, CF, R0, R1, R2);
    inline void n_edivmod(STATE, CF, R0, R1, R2);
    inline void n_epow(STATE, CF, R0, R1, R2);
    inline void n_eneg(STATE, CF, R0, R1);
    inline void n_ebits(STATE, CF, R0, R1);
    inline void n_esize(STATE, CF, R0, R1);
    inline void n_enot(STATE, CF, R0, R1);
    inline void n_eand(STATE, CF, R0, R1, R2);
    inline void n_eor(STATE, CF, R0, R1, R2);
    inline void n_exor(STATE, CF, R0, R1, R2);
    inline void n_eshl(STATE, CF, R0, R1, R2);
    inline void n_eshr(STATE, CF, R0, R1, R2);
    inline void n_epopcnt(STATE, CF, R0, R1);
    inline void n_ecmp(STATE, CF, R0, R1, R2);
    inline void n_eeq(STATE, CF, R0, R1, R2);
    inline void n_ene(STATE, CF, R0, R1, R2);
    inline void n_elt(STATE, CF, R0, R1, R2);
    inline void n_ele(STATE, CF, R0, R1, R2);
    inline void n_egt(STATE, CF, R0, R1, R2);
    inline void n_ege(STATE, CF, R0, R1, R2);
    inline void n_estr(STATE, CF, R0, R1, R2);
    inline void n_eflt(STATE, CF, R0, R1);

    // Native double floating point instructions
    inline void n_dadd(CF, R0, R1, R2);
    inline void n_dsub(CF, R0, R1, R2);
    inline void n_dmul(CF, R0, R1, R2);
    inline void n_ddiv(STATE, CF, R0, R1, R2);
    inline void n_dmod(STATE, CF, R0, R1, R2);
    inline void n_ddivmod(STATE, CF, R0, R1, R2);
    inline void n_dpow(STATE, CF, R0, R1, R2);
    inline void n_dneg(CF, R0, R1);
    inline void n_dcmp(CF, R0, R1, R2);
    inline void n_deq(CF, R0, R1, R2);
    inline void n_dne(CF, R0, R1, R2);
    inline void n_dlt(CF, R0, R1, R2);
    inline void n_dle(CF, R0, R1, R2);
    inline void n_dgt(CF, R0, R1, R2);
    inline void n_dge(CF, R0, R1, R2);
    inline void n_dstr(STATE, CF, R0, R1, R2);
    inline void n_dinf(CF, R0, R1);
    inline void n_dnan(CF, R0, R1);

    // Code execution instructions
    inline void e_cache_method_p(STATE, CF, R0, R1);
    inline void e_cache_function_p(STATE, CF, R0, R1);
    inline void e_cache_value_p(STATE, CF, R0, R1);
    inline void e_cache_method(STATE, CF, R0, R1);
    inline void e_cache_function(STATE, CF, R0, R1);
    inline void e_cache_value(STATE, CF, R0, R1);
    inline void e_resolve_method(STATE, CF, R0, R1);
    inline void e_resolve_receiver_method(STATE, CF, R0, R1);
    inline void e_resolve_function(STATE, CF, R0, R1);
    inline void e_resolve_scope_constant(STATE, CF, R0, R1);
    inline void e_resolve_path_constant(STATE, CF, R0, R1);
    inline void e_signature(STATE, CF, R0, R1);
    inline void e_check_signature(STATE, CF, R0, R1);
    inline void e_invoke_method(STATE, CF, R0, R1);
    inline void e_invoke_function(STATE, CF, R0, R1);

    // Assertion instructions
    inline void a_instance(STATE, CF, R0, R1);
    inline void a_kind(STATE, CF, R0, R1);
    inline void a_method(STATE, CF, R0, R1);
    inline void a_receiver_method(STATE, CF, R0, R1);
    inline void a_type(STATE, CF, R0, R1);
    inline void a_function(STATE, CF, R0, R1);
    inline void a_equal(STATE, CF, R0, R1);
    inline void a_not_equal(STATE, CF, R0, R1);
    inline void a_less(STATE, CF, R0, R1);
    inline void a_less_equal(STATE, CF, R0, R1);
    inline void a_greater(STATE, CF, R0, R1);
    inline void a_greater_equal(STATE, CF, R0, R1);
  }
}

#endif
