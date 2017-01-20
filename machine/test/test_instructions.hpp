#include "machine/test/test.hpp"

#include "call_frame.hpp"
#include "object_utils.hpp"

#include "instructions/add_scope.hpp"
#include "instructions/allow_private.hpp"
#include "instructions/cast_array.hpp"
#include "instructions/cast_for_multi_block_arg.hpp"
#include "instructions/cast_for_single_block_arg.hpp"
#include "instructions/cast_for_splat_block_arg.hpp"
#include "instructions/cast_multi_value.hpp"
#include "instructions/check_frozen.hpp"
#include "instructions/check_interrupts.hpp"
#include "instructions/check_serial.hpp"
#include "instructions/check_serial_private.hpp"
#include "instructions/clear_exception.hpp"
#include "instructions/create_block.hpp"
#include "instructions/dup.hpp"
#include "instructions/dup_many.hpp"
#include "instructions/ensure_return.hpp"
#include "instructions/find_const.hpp"
#include "instructions/goto.hpp"
#include "instructions/goto_if_equal.hpp"
#include "instructions/goto_if_false.hpp"
#include "instructions/goto_if_nil.hpp"
#include "instructions/goto_if_not_equal.hpp"
#include "instructions/goto_if_not_nil.hpp"
#include "instructions/goto_if_not_undefined.hpp"
#include "instructions/goto_if_true.hpp"
#include "instructions/goto_if_undefined.hpp"
#include "instructions/instance_of.hpp"
#include "instructions/invoke_primitive.hpp"
#include "instructions/kind_of.hpp"
#include "instructions/make_array.hpp"
#include "instructions/move_down.hpp"
#include "instructions/noop.hpp"
#include "instructions/object_to_s.hpp"
#include "instructions/passed_arg.hpp"
#include "instructions/passed_blockarg.hpp"
#include "instructions/pop.hpp"
#include "instructions/pop_many.hpp"
#include "instructions/pop_unwind.hpp"
#include "instructions/push_block.hpp"
#include "instructions/push_block_arg.hpp"
#include "instructions/push_const.hpp"
#include "instructions/push_cpath_top.hpp"
#include "instructions/push_current_exception.hpp"
#include "instructions/push_exception_state.hpp"
#include "instructions/push_false.hpp"
#include "instructions/push_has_block.hpp"
#include "instructions/push_int.hpp"
#include "instructions/push_ivar.hpp"
#include "instructions/push_literal.hpp"
#include "instructions/push_local.hpp"
#include "instructions/push_local_depth.hpp"
#include "instructions/push_memo.hpp"
#include "instructions/push_mirror.hpp"
#include "instructions/push_my_field.hpp"
#include "instructions/push_my_offset.hpp"
#include "instructions/push_nil.hpp"
#include "instructions/push_proc.hpp"
#include "instructions/push_rubinius.hpp"
#include "instructions/push_scope.hpp"
#include "instructions/push_self.hpp"
#include "instructions/push_stack_local.hpp"
#include "instructions/push_true.hpp"
#include "instructions/push_type.hpp"
#include "instructions/push_undef.hpp"
#include "instructions/push_variables.hpp"
#include "instructions/raise_break.hpp"
#include "instructions/raise_exc.hpp"
#include "instructions/raise_return.hpp"
#include "instructions/reraise.hpp"
#include "instructions/restore_exception_state.hpp"
#include "instructions/ret.hpp"
#include "instructions/rotate.hpp"
#include "instructions/run_exception.hpp"
#include "instructions/send_method.hpp"
#include "instructions/send_stack.hpp"
#include "instructions/send_stack_with_block.hpp"
#include "instructions/send_stack_with_splat.hpp"
#include "instructions/send_super_stack_with_block.hpp"
#include "instructions/send_super_stack_with_splat.hpp"
#include "instructions/send_vcall.hpp"
#include "instructions/set_call_flags.hpp"
#include "instructions/set_const.hpp"
#include "instructions/set_const_at.hpp"
#include "instructions/set_ivar.hpp"
#include "instructions/set_local.hpp"
#include "instructions/set_local_depth.hpp"
#include "instructions/set_stack_local.hpp"
#include "instructions/setup_unwind.hpp"
#include "instructions/shift_array.hpp"
#include "instructions/store_my_field.hpp"
#include "instructions/string_append.hpp"
#include "instructions/string_build.hpp"
#include "instructions/string_dup.hpp"
#include "instructions/swap.hpp"
#include "instructions/yield_debugger.hpp"
#include "instructions/yield_splat.hpp"
#include "instructions/yield_stack.hpp"
#include "instructions/zsuper.hpp"

class TestInstructions : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_add_scope() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(Module::create(state));

    instructions::add_scope(state, call_frame);

    TS_ASSERT(instance_of<LexicalScope>(call_frame->lexical_scope_));
  }

  void test_allow_private() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    instructions::allow_private();

    // There is no effect to this instruction so this assertion merely records
    // that the call was made successfully.
    TS_ASSERT(true);
  }

  void test_cast_array_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    instructions::cast_array(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 0);
  }

  void test_cast_array_tuple() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Tuple* tuple = Tuple::create(state, 0);

    stack_push(tuple);

    instructions::cast_array(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 0);
  }

  void test_cast_array_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Array* original = Array::create(state, 0);

    stack_push(original);

    instructions::cast_array(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 0);
    TS_ASSERT_EQUALS(ary, original);
  }

  void test_cast_array_non_array_no_to_ary() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(Fixnum::from(42));

    TS_ASSERT_THROWS(instructions::cast_array(state, call_frame),
        const RubyException &);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_array_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_args = new Object*[1];
    Array* original = Array::create(state, 1);
    original->set(state, 0, Fixnum::from(42));
    static_args[0] = original;

    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_multi_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary, original);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_args = new Object*[1];
    static_args[0] = Fixnum::from(42);
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_multi_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg_toary_null() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* obj = RespondToToAryReturnNull::create(state);
    Object* static_args[1] = { obj };
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    TS_ASSERT(!instructions::cast_for_multi_block_arg(state, call_frame));
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg_toary_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* obj = RespondToToAryReturnArray::create(state);
    Object* static_args[1] = { obj };
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_multi_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg_toary_non_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* obj = RespondToToAryReturnNonArray::create(state);
    Object* static_args[1] = { obj };
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    TS_ASSERT(!instructions::cast_for_multi_block_arg(state, call_frame));
    TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
  }

  void test_cast_for_multi_block_arg_multi_fields_is_lambda() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);
    call_frame->flags |= CallFrame::cIsLambda;

    Object** static_args = new Object*[2];
    static_args[0] = Fixnum::from(42);
    static_args[1] = Fixnum::from(71);
    Arguments args(state->symbol("blah"), 2, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_multi_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
    TS_ASSERT_EQUALS(ary->get(state, 1), Fixnum::from(71));
  }

  void test_cast_for_single_block_arg_no_args() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);
    Arguments args(state->symbol("blah"));
    call_frame->arguments = &args;

    instructions::cast_for_single_block_arg(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop(), cNil);
  }

  void test_cast_for_single_block_arg_single_field() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_arg = new Object*[1];
    static_arg[0] = Fixnum::from(42);
    Arguments args(state->symbol("blah"), 1, static_arg);

    call_frame->arguments = &args;

    instructions::cast_for_single_block_arg(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
  }

  void test_cast_for_single_block_arg_multi_field() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_args = new Object*[2];
    static_args[0] = Fixnum::from(42);
    static_args[1] = Fixnum::from(71);
    Arguments args(state->symbol("blah"), 2, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_single_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
    TS_ASSERT_EQUALS(ary->get(state, 1), Fixnum::from(71));
  }

  void test_cast_for_splat_block_arg_no_args() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    TS_ASSERT(!instructions::cast_for_splat_block_arg(state, call_frame));
    TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
  }

  void test_cast_for_splat_block_arg_single_non_array_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_args = new Object*[1];
    static_args[0] = Fixnum::from(42);
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_splat_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
  }

  void test_cast_for_splat_block_arg_single_non_array_toary_arg_null() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* obj = RespondToToAryReturnNull::create(state);
    Object* static_args[1] = { obj };
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    TS_ASSERT(!instructions::cast_for_splat_block_arg(state, call_frame));
  }

  void test_cast_for_splat_block_arg_single_non_array_toary_arg_nonary() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* obj = RespondToToAryReturnFixnum::create(state);
    Object* static_args[1] = { obj };
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
    TS_ASSERT(!instructions::cast_for_splat_block_arg(state, call_frame));
    TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
  }

  void test_cast_for_splat_block_arg_multi_non_array_args() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_args = new Object*[2];
    static_args[0] = Fixnum::from(42);
    static_args[1] = Fixnum::from(71);
    Arguments args(state->symbol("blah"), 2, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_splat_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
    TS_ASSERT_EQUALS(ary->get(state, 1), Fixnum::from(71));
  }

  void test_cast_for_splat_block_arg_single_array_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** static_args = new Object*[2];
    Array* original = Array::create(state, 1);
    original->set(state, 0, Fixnum::from(42));
    static_args[0] = original;
    Arguments args(state->symbol("blah"), 1, static_args);

    call_frame->arguments = &args;

    instructions::cast_for_splat_block_arg(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), original);
  }

  void test_cast_multi_value_single_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Array* original = Array::create(state, 1);
    original->set(state, 0, Fixnum::from(42));
    stack_push(original);

    instructions::cast_multi_value(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary, original);
  }

  void test_cast_multi_value_single_arg_no_toary() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(Fixnum::from(42));

    instructions::cast_multi_value(state, call_frame);

    Array *ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT(kind_of<Array>(ary));
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
  }

  void test_cast_multi_value_single_arg_toary_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* obj = RespondToToAryReturnNull::create(state);
    stack_push(obj);

    TS_ASSERT(!instructions::cast_multi_value(state, call_frame));
  }

  void test_check_frozen_raising() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Array* ary = Array::create(state, 0);
    ary->freeze(state);

    stack_push(ary);

    TS_ASSERT(!instructions::check_frozen(state, call_frame));
    TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
  }

  void test_check_frozen_non_raising() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Array* ary = Array::create(state, 0);

    stack_push(ary);

    TS_ASSERT(instructions::check_frozen(state, call_frame));

    TS_ASSERT_EQUALS(stack_pop(), ary);
  }

  void test_check_interrupts() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    int checkpoints = state->vm()->metrics().machine.checkpoints;

    instructions::check_interrupts(state);

    TS_ASSERT_EQUALS(state->vm()->metrics().machine.checkpoints, checkpoints+1);
  }

  void test_check_serial_ret_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(1);
    setup_call_frame(call_frame, scope, 1);

    Object* recv = RespondToToAryReturnArray::create(state);
    Symbol* sym_literal = state->symbol("to_ary");
    CallSite* call_site = CallSite::create(state, sym_literal, 0);
    scope->initialize(recv, nullptr, nullptr, 0);

    // set call frame so instruction will use it to process
    state->vm()->set_call_frame(call_frame);
    stack_push(recv);
    intptr_t literal = reinterpret_cast<intptr_t>(call_site);
    intptr_t serial = reinterpret_cast<intptr_t>(0L);

    instructions::check_serial(state, call_frame, literal, serial);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cTrue);
  }

  void test_check_serial_ret_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(1);
    setup_call_frame(call_frame, scope, 1);

    Object* recv = RespondToToAryReturnArray::create(state);
    Symbol* sym_literal = state->symbol("to_ary");
    CallSite* call_site = CallSite::create(state, sym_literal, 0);
    scope->initialize(recv, nullptr, nullptr, 0);

    state->vm()->set_call_frame(call_frame);
    stack_push(recv);
    intptr_t literal = reinterpret_cast<intptr_t>(call_site);
    intptr_t serial = reinterpret_cast<intptr_t>(1L); // will cause mismatch

    instructions::check_serial(state, call_frame, literal, serial);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cFalse);
  }

  void test_check_serial_private_ret_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* recv = RespondToToAryReturnArray::create(state);
    Symbol* sym_literal = state->symbol("to_ary");
    CallSite* call_site = CallSite::create(state, sym_literal, 0);
    scope->initialize(recv, nullptr, nullptr, 0);

    state->vm()->set_call_frame(call_frame);
    stack_push(recv);
    intptr_t literal = reinterpret_cast<intptr_t>(call_site);
    intptr_t serial = reinterpret_cast<intptr_t>(0L);

    instructions::check_serial_private(state, call_frame, literal, serial);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cTrue);
  }

  void test_check_serial_private_ret_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* recv = RespondToToAryReturnArray::create(state);
    Symbol* sym_literal = state->symbol("to_ary");
    CallSite* call_site = CallSite::create(state, sym_literal, 0);
    scope->initialize(recv, nullptr, nullptr, 0);

    state->vm()->set_call_frame(call_frame);
    stack_push(recv);
    intptr_t literal = reinterpret_cast<intptr_t>(call_site);
    intptr_t serial = reinterpret_cast<intptr_t>(1L);

    instructions::check_serial_private(state, call_frame, literal, serial);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cFalse);
  }

  void test_clear_exception() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    object_type type = ClassType;
    Object* obj = G(object);
    Exception::make_type_error(state, type, obj);

    TS_ASSERT(state->vm()->thread_state()->current_exception());

    instructions::clear_exception(state);

    TS_ASSERT_EQUALS(state->vm()->thread_state()->current_exception(), cNil);
  }

  void test_create_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    CompiledCode* code = setup_compiled_code(0);
    call_frame->compiled_code = code;
    intptr_t literal = reinterpret_cast<intptr_t>(code);
    state->vm()->set_call_frame(call_frame);

    instructions::create_block(state, call_frame, literal);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT(kind_of<BlockEnvironment>(res));
  }

  void test_dup() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(Fixnum::from(42));

    instructions::dup(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
    TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
  }

  void test_dup_many() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(4);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* a = Fixnum::from(42);
    Object* b = Fixnum::from(71);
    stack_push(a);
    stack_push(b);
    intptr_t count = reinterpret_cast<intptr_t>(2L);

    instructions::dup_many(call_frame, count);

    TS_ASSERT_EQUALS(stack_pop(), b);
    TS_ASSERT_EQUALS(stack_pop(), a);
    TS_ASSERT_EQUALS(stack_pop(), b);
    TS_ASSERT_EQUALS(stack_pop(), a);
  }

  void test_ensure_return() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::ensure_return(state, call_frame);

    TS_ASSERT(true);
  }

  void test_find_const() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::find_const(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_goto() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::goto_(call_frame);

    TS_ASSERT(true);
  }

  void test_goto_if_equal_both_equal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* a = Fixnum::from(42);
    stack_push(a);
    stack_push(a);

    TS_ASSERT(instructions::goto_if_equal(call_frame));
  }

  void test_goto_if_equal_not_equal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* a = Fixnum::from(42);
    Object* b = Fixnum::from(71);
    stack_push(a);
    stack_push(b);

    TS_ASSERT(!instructions::goto_if_equal(call_frame));
  }

  void test_goto_if_false_push_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cFalse);

    TS_ASSERT(instructions::goto_if_false(call_frame));
  }

  void test_goto_if_false_push_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cTrue);

    TS_ASSERT(!instructions::goto_if_false(call_frame));
  }

  void test_goto_if_nil_push_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    TS_ASSERT(instructions::goto_if_nil(call_frame));
  }

  void test_goto_if_nil_push_non_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cTrue);

    TS_ASSERT(!instructions::goto_if_nil(call_frame));
  }

  void test_goto_if_not_equal_both_equal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* a = Fixnum::from(42);
    stack_push(a);
    stack_push(a);

    TS_ASSERT(!instructions::goto_if_not_equal(call_frame));
  }

  void test_goto_if_not_equal_both_not_equal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* a = Fixnum::from(42);
    Object* b = Fixnum::from(71);
    stack_push(a);
    stack_push(b);

    TS_ASSERT(instructions::goto_if_not_equal(call_frame));
  }

  void test_goto_if_not_nil_nil_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    TS_ASSERT(!instructions::goto_if_not_nil(call_frame));
  }

  void test_goto_if_not_nil_non_nil_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(Fixnum::from(42));

    TS_ASSERT(instructions::goto_if_not_nil(call_frame));
  }

  void test_goto_if_not_undefined_arg_undefined() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(G(undefined));

    TS_ASSERT(!instructions::goto_if_not_undefined(state, call_frame));
  }

  void test_goto_if_not_undefined_arg_not_undefined() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    TS_ASSERT(instructions::goto_if_not_undefined(state, call_frame));
  }

  void test_goto_if_true_arg_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cTrue);

    TS_ASSERT(instructions::goto_if_true(call_frame));
  }

  void test_goto_if_true_arg_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cFalse);

    TS_ASSERT(!instructions::goto_if_true(call_frame));
  }

  void test_goto_if_undefined_arg_undefined() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(G(undefined));

    TS_ASSERT(instructions::goto_if_undefined(state, call_frame));
  }

  void test_goto_if_undefined_arg_defined() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    TS_ASSERT(!instructions::goto_if_undefined(state, call_frame));
  }

  void test_instance_of_args_match() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Fixnum* a = Fixnum::from(42);
    stack_push(a->class_object(state));
    stack_push(a);

    instructions::instance_of(state, call_frame);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cTrue);
  }

  void test_instance_of_args_mismatch() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Fixnum* a = Fixnum::from(42);
    stack_push(cNil->class_object(state));
    stack_push(a);

    instructions::instance_of(state, call_frame);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cFalse);
  }

  void test_invoke_primitive() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);
    intptr_t count = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::invoke_primitive(state, call_frame, literal, count);

    TS_ASSERT(literal);
    TS_ASSERT(count);
  }

  void test_kind_of_arg_match() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Fixnum* a = Fixnum::from(42);
    stack_push(a->class_object(state));
    stack_push(a);

    instructions::kind_of(state, call_frame);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cTrue);
  }

  void test_kind_of_arg_mismatch() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Fixnum* a = Fixnum::from(42);
    stack_push(cNil->class_object(state));
    stack_push(a);

    instructions::kind_of(state, call_frame);

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cFalse);
  }

  void test_make_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(3);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Fixnum* a = Fixnum::from(42);
    Fixnum* b = Fixnum::from(71);
    stack_push(a);
    stack_push(cNil);
    stack_push(b);
    intptr_t count = 3;

    instructions::make_array(state, call_frame, count);

    Array* ary = try_as<Array>(stack_pop());

    TS_ASSERT(ary);
    TS_ASSERT_EQUALS(ary->size(), 3);
    TS_ASSERT_EQUALS(ary->get(state, 0), a);
    TS_ASSERT_EQUALS(ary->get(state, 1), cNil);
    TS_ASSERT_EQUALS(ary->get(state, 2), b);
  }

  void test_move_down() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(4);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Fixnum* a = Fixnum::from(0);
    Fixnum* b = Fixnum::from(1);
    Fixnum* c = Fixnum::from(2);
    Fixnum* d = Fixnum::from(3);
    stack_push(a);
    stack_push(b);
    stack_push(c);
    stack_push(d);
    intptr_t positions = reinterpret_cast<intptr_t>(2L);

    instructions::move_down(call_frame, positions);

    TS_ASSERT_EQUALS(stack_pop(), c);
    TS_ASSERT_EQUALS(stack_pop(), b);
    TS_ASSERT_EQUALS(stack_pop(), d);
    TS_ASSERT_EQUALS(stack_pop(), a);
  }

  void test_noop() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::noop();

    // no op does not do anything
    TS_ASSERT(true);
  }

  void test_object_to_s() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::object_to_s(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_passed_arg_no_args() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    call_frame->arguments = nullptr;
    intptr_t index = reinterpret_cast<intptr_t>(0L);

    TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
    TS_ASSERT(!instructions::passed_arg(state, call_frame, index));
    TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
  }

  void test_passed_arg_two_args_matches() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    // initialize some complex structures to mock
    CompiledCode* code = setup_compiled_code(0);
    MachineCode* mcode = new MachineCode(state, code);

    Object* a = Fixnum::from(42);
    Object* static_args[2] = { a, a };
    Arguments args(state->symbol("blah"), 2, static_args);

    call_frame->arguments = &args;
    call_frame->machine_code = mcode;

    intptr_t index = reinterpret_cast<intptr_t>(1L);

    TS_ASSERT(instructions::passed_arg(state, call_frame, index));

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cTrue);
  }

  void test_passed_arg_one_arg_no_match() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    // initialize some complex structures to mock
    CompiledCode* code = setup_compiled_code(0);
    MachineCode* mcode = new MachineCode(state, code);

    Object* a = Fixnum::from(42);
    Object* static_args[2] = { a, a };
    Arguments args(state->symbol("blah"), 2, static_args);

    call_frame->arguments = &args;
    call_frame->machine_code = mcode;

    intptr_t index = reinterpret_cast<intptr_t>(3L);

    TS_ASSERT(instructions::passed_arg(state, call_frame, index));

    Object* res = reinterpret_cast<Object*>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, cFalse);
  }

  void test_passed_blockarg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t count = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::passed_blockarg(state, call_frame, count);

    TS_ASSERT(count);
  }

  void test_pop() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** stack_ptr = STACK_PTR;
    stack_push(cNil);

    instructions::pop(call_frame);

    TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
  }

  void test_pop_many() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(3);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object** stack_ptr = STACK_PTR;
    stack_push(cNil);
    stack_push(cNil);
    stack_push(cNil);
    intptr_t count = reinterpret_cast<intptr_t>(3L);

    instructions::pop_many(call_frame, count);

    TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
  }

  void test_pop_unwind() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::pop_unwind(state, call_frame);

    TS_ASSERT(true);
  }

  void test_push_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    scope->set_block(BlockEnvironment::allocate(state));
    call_frame->scope = scope;

    Object** stack_ptr = STACK_PTR;
    instructions::push_block(state, call_frame);

    // assert that the stack pointer advanced by one
    TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);
  }

  void test_push_block_arg_no_args() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
    TS_ASSERT(!instructions::push_block_arg(state, call_frame));
    TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
  }

  void test_push_block_arg_good_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* block = BlockEnvironment::allocate(state);
    Arguments args(state->symbol("blah"), cNil, block);

    call_frame->arguments = &args;

    Object** stack_ptr = STACK_PTR;

    TS_ASSERT(instructions::push_block_arg(state, call_frame));

    TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);
    TS_ASSERT_EQUALS(stack_pop()->class_object(state), BlockEnvironment::allocate(state)->class_object(state));
  }

  void test_push_const() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::push_const(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_push_cpath_top() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_cpath_top(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop()->class_object(state), G(object)->class_object(state));
  }

  void test_push_current_exception() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Exception::internal_error(state, "forced an exception for testing purposes");
    Exception* exc = state->vm()->thread_state()->current_exception();

    instructions::push_current_exception(state, call_frame);

    Exception* res = try_as<Exception>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res, exc);
  }

  void test_push_exception_state() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Exception::internal_error(state, "forced an exception for testing purposes");
    ThreadState* exc = state->vm()->thread_state()->state_as_object(state);

    instructions::push_exception_state(state, call_frame);

    ThreadState* res = try_as<ThreadState>(stack_pop());

    TS_ASSERT(res);
    TS_ASSERT_EQUALS(res->current_exception(), exc->current_exception());
  }

  void test_push_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_false(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), cFalse);
  }

  void test_push_has_block_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_has_block(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), cFalse);
  }

  void test_push_has_block_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    scope->set_block(BlockEnvironment::allocate(state));
    call_frame->scope = scope;

    instructions::push_has_block(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), cTrue);
  }

  void test_push_int() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t number = reinterpret_cast<intptr_t>(Fixnum::from(42));

    instructions::push_int(call_frame, number);

    TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
  }

  void test_push_ivar() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Symbol* sym = state->symbol("@blah");
    scope->self()->set_ivar(state, sym, cTrue);
    intptr_t literal = reinterpret_cast<intptr_t>(sym);

    instructions::push_ivar(state, call_frame, literal);

    TS_ASSERT_EQUALS(stack_pop(), cTrue);
  }

  void test_push_literal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::push_literal(call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_push_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t local = 0;

    // TODO: instructions
    // instructions::push_local(call_frame, local);

    TS_ASSERT(!local);
  }

  void test_push_local_depth() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t depth = 0;
    intptr_t index = 0;

    // TODO: instructions
    // instructions::push_local_depth(state, call_frame, depth, index);

    TS_ASSERT(!depth);
    TS_ASSERT(!index);
  }

  void test_push_memo() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::push_memo(call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_push_mirror() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_mirror(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop(), G(mirror));
  }

  void test_push_my_field() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t index = 0;

    // TODO: instructions
    // instructions::push_my_field(state, call_frame, index);

    TS_ASSERT(!index);
  }

  void test_push_my_offset() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t index = 0;

    // TODO: instructions
    // instructions::push_my_offset(call_frame, index);

    TS_ASSERT(!index);
  }

  void test_push_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_nil(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), cNil);
  }

  void test_push_proc() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::push_proc(state, call_frame);

    TS_ASSERT(true);
  }

  void test_push_rubinius() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_rubinius(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop(), G(rubinius));
  }

  void test_push_scope() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_scope(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), call_frame->lexical_scope());
  }

  void test_push_self() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_self(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), call_frame->self());
  }

  void test_push_stack_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t which = 0;

    // TODO: instructions
    // instructions::push_stack_local(call_frame, which);

    TS_ASSERT(!which);
  }

  void test_push_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_true(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), cTrue);
  }

  void test_push_type() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_type(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop(), G(type));
  }

  void test_push_undef() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    instructions::push_undef(state, call_frame);

    TS_ASSERT_EQUALS(stack_pop(), G(undefined));
  }

  void test_push_variables() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::push_variables(state, call_frame);

    TS_ASSERT(true);
  }

  void test_raise_break() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::raise_break(state, call_frame);

    TS_ASSERT(true);
  }

  void test_raise_exc() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::raise_exc(state, call_frame);

    TS_ASSERT(true);
  }

  void test_raise_return() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::raise_return(state, call_frame);

    TS_ASSERT(true);
  }

  void test_reraise() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::reraise(state, call_frame);

    TS_ASSERT(true);
  }

  void test_restore_exception_state() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::restore_exception_state(state, call_frame);

    TS_ASSERT(true);
  }

  void test_ret() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::ret(state, call_frame);

    TS_ASSERT(true);
  }

  void test_rotate() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t count = 2;

    // TODO: instructions
    // instructions::rotate(call_frame, count);

    TS_ASSERT(count);
  }

  void test_run_exception() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::run_exception(state, call_frame);

    TS_ASSERT(true);
  }

  void test_send_method() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::send_method(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_send_stack() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::send_stack(state, call_frame, literal, count);

    TS_ASSERT(literal);
    TS_ASSERT(count);
  }

  void test_send_stack_with_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::send_stack_with_block(state, call_frame, literal, count);

    TS_ASSERT(literal);
    TS_ASSERT(count);
  }

  void test_send_stack_with_splat() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::send_stack_with_splat(state, call_frame, literal, count);

    TS_ASSERT(literal);
    TS_ASSERT(count);
  }

  void test_send_super_stack_with_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::send_super_stack_with_block(state, call_frame, literal, count);

    TS_ASSERT(literal);
    TS_ASSERT(count);
  }

  void test_send_super_stack_with_splat() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::send_super_stack_with_splat(state, call_frame, literal, count);

    TS_ASSERT(literal);
    TS_ASSERT(count);
  }

  void test_send_vcall() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::send_vcall(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_set_call_flags() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t flags = 0;

    // TODO: instructions
    // instructions::set_call_flags(call_frame);

    TS_ASSERT(!flags);
  }

  void test_set_const() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::set_const(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_set_const_at() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::set_const_at(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_set_ivar() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::set_ivar(state, call_frame, literal);

    TS_ASSERT(literal);
  }

  void test_set_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t local = 0;

    // TODO: instructions
    // instructions::set_local(call_frame, local);

    TS_ASSERT(!local);
  }

  void test_set_local_depth() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t depth = 0;
    intptr_t index = 0;

    // TODO: instructions
    // instructions::set_local_depth(state, call_frame, depth, index);

    TS_ASSERT(!depth);
    TS_ASSERT(!index);
  }

  void test_set_stack_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t which = 0;

    // TODO: instructions
    // instructions::set_stack_local(call_frame, which);

    TS_ASSERT(!which);
  }

  void test_setup_unwind() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t ip = 0;
    intptr_t type = 0;

    // TODO: instructions
    // instructions::setup_unwind(call_frame, ip, type);

    TS_ASSERT(!ip);
    TS_ASSERT(!type);
  }

  void test_shift_array_empty() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Array* original = Array::create(state, 0);
    stack_push(original);

    instructions::shift_array(state, call_frame);

    Object* shifted_value = stack_pop();
    Array* ary = try_as<Array>(stack_pop());

    TS_ASSERT(shifted_value);
    TS_ASSERT_EQUALS(shifted_value, cNil);
    TS_ASSERT(ary);
    TS_ASSERT_EQUALS(ary->size(), 0);
  }

  void test_shift_array_has_elements() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Array* original = Array::create(state, 2);
    original->set(state, 0, Fixnum::from(42));
    original->set(state, 1, Fixnum::from(71));
    stack_push(original);

    instructions::shift_array(state, call_frame);

    Object* shifted_value = stack_pop();
    Array* ary = try_as<Array>(stack_pop());

    TS_ASSERT(shifted_value);
    TS_ASSERT_EQUALS(shifted_value, Fixnum::from(42));
    TS_ASSERT(ary);
    TS_ASSERT_EQUALS(ary->size(), 1);
  }

  void test_store_my_field() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t index = 0;

    // TODO: instructions
    // instructions::store_my_field(state, call_frame, index);

    TS_ASSERT(!index);
  }

  void test_string_append() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    String* final_string = String::create(state, "rubinius");
    String* s1 = String::create(state, "rubini");
    String* s2 = String::create(state, "us");
    stack_push(s2);
    stack_push(s1);

    instructions::string_append(state, call_frame);

    String* string = try_as<String>(stack_pop());

    TS_ASSERT(string);
    TS_ASSERT_EQUALS(string->equal(state, final_string), cTrue);
  }

  void test_string_build() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t count = 2;

    // TODO: instructions
    // instructions::string_build(state, call_frame, count);

    TS_ASSERT(count);
  }

  void test_string_dup() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    String* str = String::create(state, "blah");
    stack_push(str);

    TS_ASSERT(instructions::string_dup(state, call_frame));

    String* dup = (String*) stack_pop();
    TS_ASSERT_EQUALS(dup->data(), str->data());
  }

  void test_swap() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(2);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    Object* a = Fixnum::from(42);
    Object* b = Fixnum::from(71);
    stack_push(a);
    stack_push(b);

    instructions::swap(call_frame);

    TS_ASSERT_EQUALS(stack_pop(), a);
    TS_ASSERT_EQUALS(stack_pop(), b);
  }

  void test_yield_debugger() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);

    // TODO: instructions
    // instructions::yield_debugger(state);

    TS_ASSERT(true);
  }

  void test_yield_splat() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::yield_splat(state, call_frame, count);

    TS_ASSERT(count);
  }

  void test_yield_stack() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t count = 1;

    // TODO: instructions
    // instructions::yield_stack(state, call_frame, count);

    TS_ASSERT(count);
  }

  void test_zsuper() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    stack_push(cNil);
    intptr_t literal = reinterpret_cast<intptr_t>(cNil);

    // TODO: instructions
    // instructions::zsuper(state, call_frame, literal);

    TS_ASSERT(literal);
  }
};
