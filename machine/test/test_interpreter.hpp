#include "machine/test/test.hpp"

#include "call_frame.hpp"
#include "object_utils.hpp"

#include "interpreter.hpp"

class TestInterpreter : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_interpreter_add_scope() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_add_scope.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    stack_push(Module::create(state));

    interpreter::add_scope(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_add_scope.width);
  }

  void test_interpreter_allow_private() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_allow_private.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    interpreter::allow_private(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_allow_private.width);
  }

  /* The cast_array instruction has multiple paths that are tested in the
   * instruction tests. Here we only test that the interpreter advances. This
   * obviously relies on knowledge that all paths through the instruction
   * result in the interpreter advancing. But we have this knowledge, so
   * multiple tests of the instruction via the interpreter don't add anything
   * to the multiple instruction tests we already have.
   */
  void test_interpreter_cast_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_cast_array.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    stack_push(cNil);

    interpreter::cast_array(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_cast_array.width);
  }

  void test_interpreter_cast_for_multi_block_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_cast_for_multi_block_arg.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::cast_for_multi_block_arg(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_cast_for_multi_block_arg.width);
  }

  void test_interpreter_cast_for_single_block_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_cast_for_single_block_arg.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::cast_for_single_block_arg(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_cast_for_single_block_arg.width);
  }

  void test_interpreter_cast_for_splat_block_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_cast_for_splat_block_arg.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::cast_for_splat_block_arg(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_cast_for_splat_block_arg.width);
  }

  void test_interpreter_cast_multi_value() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_cast_multi_value.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::cast_multi_value(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_cast_multi_value.width);
  }

  void test_interpreter_check_frozen_non_raising() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_check_frozen.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    Array* ary = Array::create(state, 0);

    stack_push(ary);

    interpreter::check_frozen(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_check_frozen.width);
  }

  void test_interpreter_check_interrupts() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_check_interrupts.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::check_interrupts(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_check_interrupts.width);
  }

  void test_interpreter_check_serial() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_check_serial.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // serial
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::check_serial(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_check_serial.width);
  }

  void test_interpreter_check_serial_private() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_check_serial_private.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // serial
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::check_serial_private(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_check_serial_private.width);
  }

  void test_interpreter_clear_exception() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_clear_exception.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::clear_exception(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_clear_exception.width);
  }

  void test_interpreter_create_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_create_block.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::create_block(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_create_block.width);
  }

  void test_interpreter_dup() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_dup.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::dup(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_dup.width);
  }

  void test_interpreter_dup_many() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_dup_many.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::dup_many(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_dup_many.width);
  }

  void test_interpreter_ensure_return() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_ensure_return.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::ensure_return(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_ensure_return.width);
  }

  void test_interpreter_find_const() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_find_const.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::find_const(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_find_const.width);
  }

  void test_interpreter_goto() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto.width);
  }

  void test_interpreter_goto_if_equal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_equal.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_equal(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_equal.width);
  }

  void test_interpreter_goto_if_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_false.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_false(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_false.width);
  }

  void test_interpreter_goto_if_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_nil.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_nil(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_nil.width);
  }

  void test_interpreter_goto_if_not_equal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_not_equal.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_not_equal(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_not_equal.width);
  }

  void test_interpreter_goto_if_not_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_not_nil.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_not_nil(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_not_nil.width);
  }

  void test_interpreter_goto_if_not_undefined() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_not_undefined.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_not_undefined(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_not_undefined.width);
  }

  void test_interpreter_goto_if_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_true.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_true(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_true.width);
  }

  void test_interpreter_goto_if_undefined() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_goto_if_undefined.interpreter_address);
    opcodes[1] = 0; // location
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::goto_if_undefined(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_goto_if_undefined.width);
  }

  void test_interpreter_instance_of() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_instance_of.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::instance_of(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_instance_of.width);
  }

  void test_interpreter_invoke_primitive() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_invoke_primitive.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // count
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::invoke_primitive(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_invoke_primitive.width);
  }

  void test_interpreter_kind_of() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_kind_of.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::kind_of(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_kind_of.width);
  }

  void test_interpreter_make_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_make_array.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::make_array(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_make_array.width);
  }

  void test_interpreter_move_down() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_move_down.interpreter_address);
    opcodes[1] = 0; // positions
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::move_down(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_move_down.width);
  }

  void test_interpreter_noop() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_noop.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::noop(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_noop.width);
  }

  void test_interpreter_object_to_s() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_object_to_s.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::object_to_s(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_object_to_s.width);
  }

  void test_interpreter_passed_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_passed_arg.interpreter_address);
    opcodes[1] = 0; // index
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::passed_arg(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_passed_arg.width);
  }

  void test_interpreter_passed_blockarg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_passed_blockarg.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::passed_blockarg(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_passed_blockarg.width);
  }

  void test_interpreter_pop() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_pop.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::pop(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_pop.width);
  }

  void test_interpreter_pop_many() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_pop_many.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::pop_many(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_pop_many.width);
  }

  void test_interpreter_pop_unwind() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_pop_unwind.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::pop_unwind(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_pop_unwind.width);
  }

  void test_interpreter_push_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_block.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_block(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_block.width);
  }

  void test_interpreter_push_block_arg() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_block_arg.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_block_arg(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_block_arg.width);
  }

  void test_interpreter_push_const() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_const.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_const(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_const.width);
  }

  void test_interpreter_push_cpath_top() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_cpath_top.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_cpath_top(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_cpath_top.width);
  }

  void test_interpreter_push_current_exception() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_current_exception.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_current_exception(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_current_exception.width);
  }

  void test_interpreter_push_exception_state() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_exception_state.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_exception_state(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_exception_state.width);
  }

  void test_interpreter_push_false() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_false.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    intptr_t value = interpreter::push_false(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(value, reinterpret_cast<intptr_t>(cFalse));
    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_false.width);
  }

  void test_interpreter_push_has_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_has_block.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_has_block(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_has_block.width);
  }

  void test_interpreter_push_int() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_int.interpreter_address);
    opcodes[1] = 84; // number
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    intptr_t value = interpreter::push_int(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(value, 84);
    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_int.width);
  }

  void test_interpreter_push_ivar() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_ivar.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_ivar(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_ivar.width);
  }

  void test_interpreter_push_literal() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_literal.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_literal(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_literal.width);
  }

  void test_interpreter_push_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_local.interpreter_address);
    opcodes[1] = 0; // local
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_local(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_local.width);
  }

  void test_interpreter_push_local_depth() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_local_depth.interpreter_address);
    opcodes[1] = 0; // depth
    opcodes[2] = 0; // index
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_local_depth(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_local_depth.width);
  }

  void test_interpreter_push_memo() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_memo.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_memo(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_memo.width);
  }

  void test_interpreter_push_mirror() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_mirror.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_mirror(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_mirror.width);
  }

  void test_interpreter_push_my_field() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_my_field.interpreter_address);
    opcodes[1] = 0; // index
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_my_field(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_my_field.width);
  }

  void test_interpreter_push_my_offset() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_my_offset.interpreter_address);
    opcodes[1] = 0; // index
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_my_offset(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_my_offset.width);
  }

  void test_interpreter_push_nil() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_nil.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    intptr_t value = interpreter::push_nil(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(value, reinterpret_cast<intptr_t>(cNil));
    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_nil.width);
  }

  void test_interpreter_push_proc() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_proc.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_proc(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_proc.width);
  }

  void test_interpreter_push_rubinius() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_rubinius.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_rubinius(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_rubinius.width);
  }

  void test_interpreter_push_scope() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_scope.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_scope(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_scope.width);
  }

  void test_interpreter_push_self() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_self.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_self(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_self.width);
  }

  void test_interpreter_push_stack_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_stack_local.interpreter_address);
    opcodes[1] = 0; // which
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_stack_local(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_stack_local.width);
  }

  void test_interpreter_push_true() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_true.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    intptr_t value = interpreter::push_true(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(value, reinterpret_cast<intptr_t>(cTrue));
    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_true.width);
  }

  void test_interpreter_push_type() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_type.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_type(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_type.width);
  }

  void test_interpreter_push_undef() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_undef.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    intptr_t value = interpreter::push_undef(state, call_frame, opcodes);

    TS_ASSERT_EQUALS(value, reinterpret_cast<intptr_t>(G(undefined)));
    TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_undef.width);
  }

  void test_interpreter_push_variables() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_push_variables.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::push_variables(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_push_variables.width);
  }

  void test_interpreter_raise_break() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_raise_break.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::raise_break(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_raise_break.width);
  }

  void test_interpreter_raise_exc() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_raise_exc.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::raise_exc(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_raise_exc.width);
  }

  void test_interpreter_raise_return() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_raise_return.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::raise_return(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_raise_return.width);
  }

  void test_interpreter_reraise() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_reraise.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::reraise(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_reraise.width);
  }

  void test_interpreter_restore_exception_state() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_restore_exception_state.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::restore_exception_state(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_restore_exception_state.width);
  }

  void test_interpreter_ret() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::ret(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_ret.width);
  }

  void test_interpreter_rotate() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_rotate.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::rotate(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_rotate.width);
  }

  void test_interpreter_run_exception() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_run_exception.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::run_exception(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_run_exception.width);
  }

  void test_interpreter_send_method() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_method.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_method(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_method.width);
  }

  void test_interpreter_send_stack() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_stack.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // count
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_stack(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_stack.width);
  }

  void test_interpreter_send_stack_with_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_stack_with_block.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // count
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_stack_with_block(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_stack_with_block.width);
  }

  void test_interpreter_send_stack_with_splat() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_stack_with_splat.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // count
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_stack_with_splat(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_stack_with_splat.width);
  }

  void test_interpreter_send_super_stack_with_block() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_super_stack_with_block.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // count
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_super_stack_with_block(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_super_stack_with_block.width);
  }

  void test_interpreter_send_super_stack_with_splat() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_super_stack_with_splat.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] = 0; // count
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_super_stack_with_splat(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_super_stack_with_splat.width);
  }

  void test_interpreter_send_vcall() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_send_vcall.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::send_vcall(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_send_vcall.width);
  }

  void test_interpreter_set_call_flags() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_call_flags.interpreter_address);
    opcodes[1] = 0; // flags
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_call_flags(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_call_flags.width);
  }

  void test_interpreter_set_const() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_const.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_const(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_const.width);
  }

  void test_interpreter_set_const_at() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_const_at.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_const_at(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_const_at.width);
  }

  void test_interpreter_set_ivar() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_ivar.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_ivar(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_ivar.width);
  }

  void test_interpreter_set_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_local.interpreter_address);
    opcodes[1] = 0; // local
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_local(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_local.width);
  }

  void test_interpreter_set_local_depth() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_local_depth.interpreter_address);
    opcodes[1] = 0; // depth
    opcodes[2] = 0; // index
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_local_depth(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_local_depth.width);
  }

  void test_interpreter_set_stack_local() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_set_stack_local.interpreter_address);
    opcodes[1] = 0; // which
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::set_stack_local(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_set_stack_local.width);
  }

  void test_interpreter_setup_unwind() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[4];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_setup_unwind.interpreter_address);
    opcodes[1] = 0; // ip
    opcodes[2] = 0; // type
    opcodes[3] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::setup_unwind(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_setup_unwind.width);
  }

  void test_interpreter_shift_array() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_shift_array.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::shift_array(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_shift_array.width);
  }

  void test_interpreter_store_my_field() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_store_my_field.interpreter_address);
    opcodes[1] = 0; // index
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::store_my_field(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_store_my_field.width);
  }

  void test_interpreter_string_append() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_string_append.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::string_append(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_string_append.width);
  }

  void test_interpreter_string_build() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_string_build.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::string_build(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_string_build.width);
  }

  void test_interpreter_string_dup() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_string_dup.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::string_dup(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_string_dup.width);
  }

  void test_interpreter_swap() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_swap.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::swap(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_swap.width);
  }

  void test_interpreter_yield_debugger() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[2];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_yield_debugger.interpreter_address);
    opcodes[1] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::yield_debugger(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_yield_debugger.width);
  }

  void test_interpreter_yield_splat() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_yield_splat.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::yield_splat(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_yield_splat.width);
  }

  void test_interpreter_yield_stack() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_yield_stack.interpreter_address);
    opcodes[1] = 0; // count
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::yield_stack(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_yield_stack.width);
  }

  void test_interpreter_zsuper() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[3];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_zsuper.interpreter_address);
    opcodes[1] = 0; // literal
    opcodes[2] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::zsuper(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_zsuper.width);
  }
};
