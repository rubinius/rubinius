#include <functional>

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

  typedef std::function<void (STATE, CallFrame*)> InstructionTest;

#define lambda [] (STATE, CallFrame* call_frame)

  CompiledCode* setup_compiled_code(int arg_length) {
    CompiledCode* code = CompiledCode::create(state);
    CompiledCode::initialize(state, code);

    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    code->literals(state, tup);

    // Need a valid stream of instructions terminated with a return in order
    // to pass the byte verifier
    code->iseq(state, InstructionSequence::create(state, 2));
    code->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_nil));
    code->iseq()->opcodes()->put(state, 1, Fixnum::from(InstructionSequence::insn_ret));
    code->stack_size(state, Fixnum::from(10));
    code->total_args(state, Fixnum::from(arg_length));
    code->required_args(state, code->total_args());
    code->post_args(state, code->total_args());

    return code;
  }

  void interpreter(int stack, int vars, InstructionTest test) {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(stack);
    StackVariables* scope = ALLOCA_STACKVARIABLES(vars);
    setup_call_frame(call_frame, scope, stack);

    call_frame->compiled_code = setup_compiled_code(0);
    call_frame->machine_code = new MachineCode(state, call_frame->compiled_code);

    test(state, call_frame);
  }

  void test_add_scope() {
    InstructionTest test = lambda {
      stack_push(Module::create(state));

      instructions::add_scope(state, call_frame);

      TS_ASSERT(instance_of<LexicalScope>(call_frame->lexical_scope_));
    };

    interpreter(1, 0, test);
  }

  void test_allow_private() {
    InstructionTest test = lambda {
      stack_push(cNil);

      instructions::allow_private();

      // There is no effect to this instruction so this assertion merely
      // records that the call was made successfully.
      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_cast_array_nil() {
    InstructionTest test = lambda {
      stack_push(cNil);

      instructions::cast_array(state, call_frame);

      Array *ary = try_as<Array>(stack_pop());

      TS_ASSERT(ary);
      TS_ASSERT(kind_of<Array>(ary));
      TS_ASSERT_EQUALS(ary->size(), 0);
    };

    interpreter(1, 0, test);
  }

  void test_cast_array_tuple() {
    InstructionTest test = lambda {
      Tuple* tuple = Tuple::create(state, 0);

      stack_push(tuple);

      instructions::cast_array(state, call_frame);

      Array *ary = try_as<Array>(stack_pop());

      TS_ASSERT(ary);
      TS_ASSERT(kind_of<Array>(ary));
      TS_ASSERT_EQUALS(ary->size(), 0);
    };

    interpreter(1, 0, test);
  }

  void test_cast_array_array() {
    InstructionTest test = lambda {
      Array* original = Array::create(state, 0);

      stack_push(original);

      instructions::cast_array(state, call_frame);

      Array *ary = try_as<Array>(stack_pop());

      TS_ASSERT(ary);
      TS_ASSERT(kind_of<Array>(ary));
      TS_ASSERT_EQUALS(ary->size(), 0);
      TS_ASSERT_EQUALS(ary, original);
    };

    interpreter(1, 0, test);
  }

  void test_cast_array_non_array_no_to_ary() {
    InstructionTest test = lambda {
      stack_push(Fixnum::from(42));

      TS_ASSERT_THROWS(instructions::cast_array(state, call_frame),
          const RubyException &);
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_array_arg() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg_toary_null() {
    InstructionTest test = lambda {
      Object* obj = RespondToToAryReturnNull::create(state);
      Object* static_args[1] = { obj };
      Arguments args(state->symbol("blah"), 1, static_args);

      call_frame->arguments = &args;

      TS_ASSERT(!instructions::cast_for_multi_block_arg(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg_toary_array() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_multi_block_arg_multi_fields_not_lambda_single_non_array_arg_toary_non_array() {
    InstructionTest test = lambda {
      Object* obj = RespondToToAryReturnNonArray::create(state);
      Object* static_args[1] = { obj };
      Arguments args(state->symbol("blah"), 1, static_args);

      call_frame->arguments = &args;

      TS_ASSERT(!instructions::cast_for_multi_block_arg(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_multi_block_arg_multi_fields_is_lambda() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_single_block_arg_no_args() {
    InstructionTest test = lambda {
      Arguments args(state->symbol("blah"));
      call_frame->arguments = &args;

      instructions::cast_for_single_block_arg(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop(), cNil);
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_single_block_arg_single_field() {
    InstructionTest test = lambda {
      Object** static_arg = new Object*[1];
      static_arg[0] = Fixnum::from(42);
      Arguments args(state->symbol("blah"), 1, static_arg);

      call_frame->arguments = &args;

      instructions::cast_for_single_block_arg(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_single_block_arg_multi_field() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_splat_block_arg_no_args() {
    InstructionTest test = lambda {
      TS_ASSERT(!instructions::cast_for_splat_block_arg(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_splat_block_arg_single_non_array_arg() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_splat_block_arg_single_non_array_toary_arg_null() {
    InstructionTest test = lambda {
      Object* obj = RespondToToAryReturnNull::create(state);
      Object* static_args[1] = { obj };
      Arguments args(state->symbol("blah"), 1, static_args);

      call_frame->arguments = &args;

      TS_ASSERT(!instructions::cast_for_splat_block_arg(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_splat_block_arg_single_non_array_toary_arg_nonary() {
    InstructionTest test = lambda {
      Object* obj = RespondToToAryReturnFixnum::create(state);
      Object* static_args[1] = { obj };
      Arguments args(state->symbol("blah"), 1, static_args);

      call_frame->arguments = &args;

      TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
      TS_ASSERT(!instructions::cast_for_splat_block_arg(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_splat_block_arg_multi_non_array_args() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_for_splat_block_arg_single_array_arg() {
    InstructionTest test = lambda {
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
    };

    interpreter(1, 0, test);
  }

  void test_cast_multi_value_single_array() {
    InstructionTest test = lambda {
      Array* original = Array::create(state, 1);
      original->set(state, 0, Fixnum::from(42));
      stack_push(original);

      instructions::cast_multi_value(state, call_frame);

      Array *ary = try_as<Array>(stack_pop());

      TS_ASSERT(ary);
      TS_ASSERT(kind_of<Array>(ary));
      TS_ASSERT_EQUALS(ary->size(), 1);
      TS_ASSERT_EQUALS(ary, original);
    };

    interpreter(1, 0, test);
  }

  void test_cast_multi_value_single_arg_no_toary() {
    InstructionTest test = lambda {
      stack_push(Fixnum::from(42));

      instructions::cast_multi_value(state, call_frame);

      Array *ary = try_as<Array>(stack_pop());

      TS_ASSERT(ary);
      TS_ASSERT(kind_of<Array>(ary));
      TS_ASSERT_EQUALS(ary->size(), 1);
      TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_cast_multi_value_single_arg_toary_nil() {
    InstructionTest test = lambda {
      Object* obj = RespondToToAryReturnNull::create(state);
      stack_push(obj);

      TS_ASSERT(!instructions::cast_multi_value(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_check_frozen_raising() {
    InstructionTest test = lambda {
      Array* ary = Array::create(state, 0);
      ary->freeze(state);

      stack_push(ary);

      TS_ASSERT(!instructions::check_frozen(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_check_frozen_non_raising() {
    InstructionTest test = lambda {
      Array* ary = Array::create(state, 0);

      stack_push(ary);

      TS_ASSERT(instructions::check_frozen(state, call_frame));
      TS_ASSERT_EQUALS(stack_pop(), ary);
    };

    interpreter(1, 0, test);
  }

  void test_check_interrupts() {
    InstructionTest test = lambda {
      int checkpoints = state->vm()->metrics().machine.checkpoints;

      instructions::check_interrupts(state);

      TS_ASSERT_EQUALS(state->vm()->metrics().machine.checkpoints, checkpoints+1);
    };

    interpreter(1, 0, test);
  }

  void test_check_serial_ret_true() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      // set call frame so instruction will use it to process
      state->vm()->set_call_frame(call_frame);
      stack_push(recv);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t serial = reinterpret_cast<intptr_t>(0L);

      instructions::check_serial(state, call_frame, literal, serial);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(1, 1, test);
  }

  void test_check_serial_ret_false() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      state->vm()->set_call_frame(call_frame);
      stack_push(recv);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t serial = reinterpret_cast<intptr_t>(1L); // will cause mismatch

      instructions::check_serial(state, call_frame, literal, serial);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cFalse);
    };

    interpreter(1, 1, test);
  }

  void test_check_serial_private_ret_true() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      state->vm()->set_call_frame(call_frame);
      stack_push(recv);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t serial = reinterpret_cast<intptr_t>(0L);

      instructions::check_serial_private(state, call_frame, literal, serial);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_check_serial_private_ret_false() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      state->vm()->set_call_frame(call_frame);
      stack_push(recv);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t serial = reinterpret_cast<intptr_t>(1L);

      instructions::check_serial_private(state, call_frame, literal, serial);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cFalse);
    };

    interpreter(1, 0, test);
  }

  void test_clear_exception() {
    InstructionTest test = lambda {
      object_type type = ClassType;
      Object* obj = G(object);
      Exception::make_type_error(state, type, obj);

      TS_ASSERT(state->vm()->thread_state()->current_exception());

      instructions::clear_exception(state);

      TS_ASSERT_EQUALS(state->vm()->thread_state()->current_exception(), cNil);
    };

    interpreter(1, 0, test);
  }

  void test_create_block() {
    InstructionTest test = lambda {
      intptr_t literal = reinterpret_cast<intptr_t>(call_frame->compiled_code);
      state->vm()->set_call_frame(call_frame);

      instructions::create_block(state, call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<BlockEnvironment>(res));
    };

    interpreter(1, 0, test);
  }

  void test_dup() {
    InstructionTest test = lambda {
      stack_push(Fixnum::from(42));

      instructions::dup(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
      TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
    };

    interpreter(2, 0, test);
  }

  void test_dup_many() {
    InstructionTest test = lambda {
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
    };

    interpreter(4, 0, test);
  }

  void test_ensure_return() {
    InstructionTest test = lambda {
      stack_push(cNil);

      // TODO: instructions
      // instructions::ensure_return(state, call_frame);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_find_const_cached() {
    InstructionTest test = lambda {
      // lots of setup for scope
      Module* module = Module::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache and populate it with a value
      Symbol* name = state->symbol("ConstantVal");
      Object* value = Fixnum::from(96);
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);
      ConstantCache* cache = ConstantCache::create(state, empty_cache, value, module, scope);

      stack_push(module);
      intptr_t literal = reinterpret_cast<intptr_t>(cache);

      TS_ASSERT(instructions::find_const(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(96));
    };

    interpreter(1, 0, test);
  }

  void test_find_const_noncached_found_non_autoload() {
    InstructionTest test = lambda {
      Module* module = Module::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache
      Symbol* name = state->symbol("ConstantVal");
      Object* value = Fixnum::from(96);
      call_frame->compiled_code->machine_code(call_frame->machine_code);
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);

      // populate module with the constant
      module->set_const(state, name, value);

      stack_push(module);
      intptr_t literal = reinterpret_cast<intptr_t>(empty_cache);

      instructions::find_const(state, call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(96));
    };

    interpreter(1, 0, test);
  }

  void test_find_const_noncached_found_autoload() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t literal = reinterpret_cast<intptr_t>(cNil);

      // TODO: instructions
      // Need to understand autoload a whole lot better before I can tackle this
      // particular case
      // instructions::find_const(state, call_frame, literal);

      TS_ASSERT(literal);
    };

    interpreter(1, 0, test);
  }

  void test_find_const_noncached_notfound_const_missing() {
    InstructionTest test = lambda {
      Module* module = ReturnConst::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache and populate it with a value
      Symbol* name = state->symbol("ConstantVal");
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);

      stack_push(module);
      intptr_t literal = reinterpret_cast<intptr_t>(empty_cache);

      TS_ASSERT(instructions::find_const(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_goto() {
    InstructionTest test = lambda {
      stack_push(cNil);

      instructions::goto_(call_frame);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_equal_both_equal() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      stack_push(a);
      stack_push(a);

      TS_ASSERT(instructions::goto_if_equal(call_frame));
    };

    interpreter(2, 0, test);
  }

  void test_goto_if_equal_not_equal() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* b = Fixnum::from(71);
      stack_push(a);
      stack_push(b);

      TS_ASSERT(!instructions::goto_if_equal(call_frame));
    };

    interpreter(2, 0, test);
  }

  void test_goto_if_false_push_false() {
    InstructionTest test = lambda {
      stack_push(cFalse);

      TS_ASSERT(instructions::goto_if_false(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_false_push_true() {
    InstructionTest test = lambda {
      stack_push(cTrue);

      TS_ASSERT(!instructions::goto_if_false(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_nil_push_nil() {
    InstructionTest test = lambda {
      stack_push(cNil);

      TS_ASSERT(instructions::goto_if_nil(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_nil_push_non_nil() {
    InstructionTest test = lambda {
      stack_push(cTrue);

      TS_ASSERT(!instructions::goto_if_nil(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_not_equal_both_equal() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      stack_push(a);
      stack_push(a);

      TS_ASSERT(!instructions::goto_if_not_equal(call_frame));
    };

    interpreter(2, 0, test);
  }

  void test_goto_if_not_equal_both_not_equal() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* b = Fixnum::from(71);
      stack_push(a);
      stack_push(b);

      TS_ASSERT(instructions::goto_if_not_equal(call_frame));
    };

    interpreter(2, 0, test);
  }

  void test_goto_if_not_nil_nil_arg() {
    InstructionTest test = lambda {
      stack_push(cNil);

      TS_ASSERT(!instructions::goto_if_not_nil(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_not_nil_non_nil_arg() {
    InstructionTest test = lambda {
      stack_push(Fixnum::from(42));

      TS_ASSERT(instructions::goto_if_not_nil(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_not_undefined_arg_undefined() {
    InstructionTest test = lambda {
      stack_push(G(undefined));

      TS_ASSERT(!instructions::goto_if_not_undefined(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_not_undefined_arg_not_undefined() {
    InstructionTest test = lambda {
      stack_push(cNil);

      TS_ASSERT(instructions::goto_if_not_undefined(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_true_arg_true() {
    InstructionTest test = lambda {
      stack_push(cTrue);

      TS_ASSERT(instructions::goto_if_true(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_true_arg_false() {
    InstructionTest test = lambda {
      stack_push(cFalse);

      TS_ASSERT(!instructions::goto_if_true(call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_undefined_arg_undefined() {
    InstructionTest test = lambda {
      stack_push(G(undefined));

      TS_ASSERT(instructions::goto_if_undefined(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_goto_if_undefined_arg_defined() {
    InstructionTest test = lambda {
      stack_push(cNil);

      TS_ASSERT(!instructions::goto_if_undefined(state, call_frame));
    };

    interpreter(1, 0, test);
  }

  void test_instance_of_args_match() {
    InstructionTest test = lambda {
      Fixnum* a = Fixnum::from(42);
      stack_push(a->class_object(state));
      stack_push(a);

      instructions::instance_of(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(2, 0, test);
  }

  void test_instance_of_args_mismatch() {
    InstructionTest test = lambda {
      Fixnum* a = Fixnum::from(42);
      stack_push(cNil->class_object(state));
      stack_push(a);

      instructions::instance_of(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cFalse);
    };

    interpreter(2, 0, test);
  }

  void test_invoke_primitive_pass() {
    InstructionTest test = lambda {
      Class* sub = state->memory()->new_class<Class>(state, G(array));
      InvokePrimitive invoker = Primitives::get_invoke_stub(state, state->symbol("array_allocate"));
      intptr_t literal = reinterpret_cast<intptr_t>(invoker);
      intptr_t count = reinterpret_cast<intptr_t>(1L);

      stack_push(sub);

      TS_ASSERT(instructions::invoke_primitive(state, call_frame, literal, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
    };

    interpreter(1, 0, test);
  }

  void test_invoke_primitive_fail() {
    InstructionTest test = lambda {
      Class* sub = state->memory()->new_class<Class>(state, G(array));
      InvokePrimitive invoker = Primitives::get_invoke_stub(state, state->symbol("unknown_prim"));
      intptr_t literal = reinterpret_cast<intptr_t>(invoker);
      intptr_t count = reinterpret_cast<intptr_t>(1L);

      stack_push(sub);

      TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
      TS_ASSERT(!instructions::invoke_primitive(state, call_frame, literal, count));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_kind_of_arg_match() {
    InstructionTest test = lambda {
      Fixnum* a = Fixnum::from(42);
      stack_push(a->class_object(state));
      stack_push(a);

      instructions::kind_of(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(2, 0, test);
  }

  void test_kind_of_arg_mismatch() {
    InstructionTest test = lambda {
      Fixnum* a = Fixnum::from(42);
      stack_push(cNil->class_object(state));
      stack_push(a);

      instructions::kind_of(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cFalse);
    };

    interpreter(2, 0, test);
  }

  void test_make_array() {
    InstructionTest test = lambda {
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
    };

    interpreter(3, 0, test);
  }

  void test_move_down() {
    InstructionTest test = lambda {
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
    };

    interpreter(4, 0, test);
  }

  void test_noop() {
    InstructionTest test = lambda {
      instructions::noop();

      // no op does not do anything
      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_object_to_s_pass_string() {
    InstructionTest test = lambda {
      Object* obj = String::create(state, "test");

      stack_push(obj);
      intptr_t literal = reinterpret_cast<intptr_t>(cNil);

      TS_ASSERT(instructions::object_to_s(state, call_frame, literal));
    };

    interpreter(1, 0, test);
  }

  void test_object_to_s_pass_obj_ret_string() {
    InstructionTest test = lambda {
      Object* recv = RespondToToSReturnString::create(state);
      Symbol* sym_literal = state->symbol("to_s");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      // set call frame so instruction will use it to process
      state->vm()->set_call_frame(call_frame);
      stack_push(recv);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      instructions::object_to_s(state, call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<String>(res));
    };

    interpreter(1, 0, test);
  }

  void test_object_to_s_pass_obj_ret_ctrue() {
    InstructionTest test = lambda {
      Object* recv = RespondToToSReturnCTrue::create(state);
      Symbol* sym_literal = state->symbol("to_s");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      // set call frame so instruction will use it to process
      state->vm()->set_call_frame(call_frame);
      stack_push(recv);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      instructions::object_to_s(state, call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<String>(res));
    };

    interpreter(1, 0, test);
  }

  void test_passed_arg_no_args() {
    InstructionTest test = lambda {
      call_frame->arguments = nullptr;
      intptr_t index = reinterpret_cast<intptr_t>(0L);

      TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
      TS_ASSERT(!instructions::passed_arg(state, call_frame, index));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_passed_arg_two_args_matches() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* static_args[2] = { a, a };
      Arguments args(state->symbol("blah"), 2, static_args);

      call_frame->arguments = &args;

      intptr_t index = reinterpret_cast<intptr_t>(1L);

      TS_ASSERT(instructions::passed_arg(state, call_frame, index));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_passed_arg_one_arg_no_match() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* static_args[2] = { a, a };
      Arguments args(state->symbol("blah"), 2, static_args);

      call_frame->arguments = &args;

      intptr_t index = reinterpret_cast<intptr_t>(3L);

      TS_ASSERT(instructions::passed_arg(state, call_frame, index));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cFalse);
    };

    interpreter(1, 0, test);
  }

  void test_passed_blockarg_noarg() {
    InstructionTest test = lambda {
      intptr_t count = reinterpret_cast<intptr_t>(1L);

      TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
      TS_ASSERT(!instructions::passed_blockarg(state, call_frame, count));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_passed_blockarg_arg_count_matches() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* static_args[1] = { a };
      Arguments args(state->symbol("blah"), 1, static_args);

      call_frame->arguments = &args;
      intptr_t count = reinterpret_cast<intptr_t>(1L);

      TS_ASSERT(instructions::passed_blockarg(state, call_frame, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_passed_blockarg_arg_count_mismatch() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* static_args[2] = { a, a };
      Arguments args(state->symbol("blah"), 2, static_args);

      call_frame->arguments = &args;
      intptr_t count = reinterpret_cast<intptr_t>(1L);

      TS_ASSERT(instructions::passed_blockarg(state, call_frame, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cFalse);
    };

    interpreter(1, 0, test);
  }

  void test_pop() {
    InstructionTest test = lambda {
      Object** stack_ptr = STACK_PTR;
      stack_push(cNil);

      instructions::pop(call_frame);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_pop_many() {
    InstructionTest test = lambda {
      Object** stack_ptr = STACK_PTR;
      stack_push(cNil);
      stack_push(cNil);
      stack_push(cNil);
      intptr_t count = reinterpret_cast<intptr_t>(3L);

      instructions::pop_many(call_frame, count);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(3, 0, test);
  }

  void test_pop_unwind() {
    InstructionTest test = lambda {
      stack_push(cNil);

      // TODO: instructions
      // instructions::pop_unwind(state, call_frame);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_push_block() {
    InstructionTest test = lambda {
      call_frame->scope->set_block(BlockEnvironment::allocate(state));

      Object** stack_ptr = STACK_PTR;
      instructions::push_block(state, call_frame);

      // assert that the stack pointer advanced by one
      TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_push_block_arg_no_args() {
    InstructionTest test = lambda {
      TS_ASSERT_EQUALS(cNil, state->thread_state()->current_exception());
      TS_ASSERT(!instructions::push_block_arg(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_push_block_arg_good_arg() {
    InstructionTest test = lambda {
      Object* block = BlockEnvironment::allocate(state);
      Arguments args(state->symbol("blah"), cNil, block);

      call_frame->arguments = &args;

      Object** stack_ptr = STACK_PTR;

      TS_ASSERT(instructions::push_block_arg(state, call_frame));

      TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);
      TS_ASSERT_EQUALS(stack_pop()->class_object(state), BlockEnvironment::allocate(state)->class_object(state));
    };

    interpreter(1, 0, test);
  }

  void test_push_const_cached() {
    InstructionTest test = lambda {
      Module* module = Module::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache and populate it with a value
      Symbol* name = state->symbol("ConstantVal");
      Object* value = Fixnum::from(96);
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);
      ConstantCache* cache = ConstantCache::create(state, empty_cache, value, module, scope);

      intptr_t literal = reinterpret_cast<intptr_t>(cache);

      TS_ASSERT(instructions::push_const(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(96));
    };

    interpreter(1, 0, test);
  }

  void test_push_const_found_in_parent_chain() {
    InstructionTest test = lambda {
      state->vm()->set_call_frame(call_frame);

      Module* module = Module::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache
      Symbol* name = state->symbol("ConstantVal");
      Object* value = Fixnum::from(96);
      call_frame->compiled_code->machine_code(call_frame->machine_code);
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);

      // populate module with the constant
      module->set_const(state, name, value);

      intptr_t literal = reinterpret_cast<intptr_t>(empty_cache);

      TS_ASSERT(instructions::push_const(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(96));
    };

    interpreter(1, 0, test);
  }

  void test_push_const_found_autoload() {
    InstructionTest test = lambda {
      /* TODO: Need to understand more about how autoloader works
         before this can be tested properly.
      */
      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_push_const_const_missing() {
    InstructionTest test = lambda {
      state->vm()->set_call_frame(call_frame);

      Module* module = ReturnConst::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache and populate it with a value
      Symbol* name = state->symbol("ConstantVal");
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);

      intptr_t literal = reinterpret_cast<intptr_t>(empty_cache);

      TS_ASSERT(instructions::push_const(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_push_const_not_found() {
    InstructionTest test = lambda {
      state->vm()->set_call_frame(call_frame);

      Module* module = Module::create(state);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, module);
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      // create empty cache and populate it with a value
      Symbol* name = state->symbol("ConstantVal");
      ConstantCache* empty_cache = ConstantCache::empty(state, name, call_frame->compiled_code, 0);

      intptr_t literal = reinterpret_cast<intptr_t>(empty_cache);

      //TS_ASSERT(! instructions::push_const(state, call_frame, literal));
      // TODO: Need autoload to fail in order for const lookup to return false
      TS_ASSERT(literal);
    };

    interpreter(1, 0, test);
  }

  void test_push_cpath_top() {
    InstructionTest test = lambda {
      instructions::push_cpath_top(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop()->class_object(state), G(object)->class_object(state));
    };

    interpreter(1, 0, test);
  }

  void test_push_current_exception() {
    InstructionTest test = lambda {
      Exception::internal_error(state, "forced an exception for testing purposes");
      Exception* exc = state->vm()->thread_state()->current_exception();

      instructions::push_current_exception(state, call_frame);

      Exception* res = try_as<Exception>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, exc);
    };

    interpreter(1, 0, test);
  }

  void test_push_exception_state() {
    InstructionTest test = lambda {
      Exception::internal_error(state, "forced an exception for testing purposes");
      ThreadState* exc = state->vm()->thread_state()->state_as_object(state);

      instructions::push_exception_state(state, call_frame);

      ThreadState* res = try_as<ThreadState>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res->current_exception(), exc->current_exception());
    };

    interpreter(1, 0, test);
  }

  void test_push_false() {
    InstructionTest test = lambda {
      instructions::push_false(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), cFalse);
    };

    interpreter(1, 0, test);
  }

  void test_push_has_block_false() {
    InstructionTest test = lambda {
      instructions::push_has_block(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), cFalse);
    };

    interpreter(1, 0, test);
  }

  void test_push_has_block_true() {
    InstructionTest test = lambda {
      call_frame->scope->set_block(BlockEnvironment::allocate(state));

      instructions::push_has_block(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_push_int() {
    InstructionTest test = lambda {
      intptr_t number = reinterpret_cast<intptr_t>(Fixnum::from(42));

      instructions::push_int(call_frame, number);

      TS_ASSERT_EQUALS(stack_pop(), Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_push_ivar() {
    InstructionTest test = lambda {
      Symbol* sym = state->symbol("@blah");
      call_frame->scope->self()->set_ivar(state, sym, cTrue);
      intptr_t literal = reinterpret_cast<intptr_t>(sym);

      instructions::push_ivar(state, call_frame, literal);

      TS_ASSERT_EQUALS(stack_pop(), cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_push_literal() {
    InstructionTest test = lambda {
      Object* obj = String::create(state, "blah");

      intptr_t literal = reinterpret_cast<intptr_t>(obj);

      Object** stack_ptr = STACK_PTR;
      instructions::push_literal(call_frame, literal);

      TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, obj);
    };

    interpreter(1, 0, test);
  }

  void test_push_local_zero() {
    InstructionTest test = lambda {
      String* str = String::create(state, "blah");
      intptr_t local = 0;
      call_frame->scope->set_local(local, str);
      Object** stack_ptr = STACK_PTR;

      instructions::push_local(call_frame, local);

      TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, str);
    };

    interpreter(1, 1, test);
  }

  void test_push_local_one() {
    InstructionTest test = lambda {
      String* str0 = String::create(state, "blah");
      String* str1 = String::create(state, "blech");
      intptr_t local = 1;
      call_frame->scope->set_local(0, str0);
      call_frame->scope->set_local(1, str1);
      Object** stack_ptr = STACK_PTR;

      instructions::push_local(call_frame, local);

      TS_ASSERT_EQUALS(STACK_PTR, ++stack_ptr);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, str1);
    };

    interpreter(1, 1, test);
  }

  void test_push_local_depth_illegal_depth() {
    InstructionTest test = lambda {
      intptr_t depth = 0;
      intptr_t index = 0;

      TS_ASSERT(!instructions::push_local_depth(state, call_frame, depth, index));
      TS_ASSERT(kind_of<Exception>(state->vm()->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_push_local_depth_illegal_depth_no_parent() {
    InstructionTest test = lambda {
      intptr_t depth = 1;
      intptr_t index = 0;

      call_frame->scope->set_parent(nullptr);

      TS_ASSERT(!instructions::push_local_depth(state, call_frame, depth, index));
      TS_ASSERT(kind_of<Exception>(state->vm()->thread_state()->current_exception()));
    };

    interpreter(1, 1, test);
  }

  void test_push_local_depth_illegal_depth_no_parent2() {
    InstructionTest test = lambda {
      intptr_t depth = 2;
      intptr_t index = 0;

      VariableScope* parent = state->memory()->new_object<VariableScope>(state, G(variable_scope));
      call_frame->scope->initialize((Object*)call_frame->scope, nullptr, nullptr, 0);
      VariableScope::initialize(state, parent);

      call_frame->scope->set_parent(parent);
      parent->parent(nil<VariableScope>());

      TS_ASSERT(!instructions::push_local_depth(state, call_frame, depth, index));
      TS_ASSERT(kind_of<Exception>(state->vm()->thread_state()->current_exception()));
    };

    interpreter(1, 1, test);
  }

  void test_push_local_depth_bad_index() {
    InstructionTest test = lambda {
      intptr_t depth = 1;
      intptr_t index = 1;

      VariableScope* parent = state->memory()->new_object<VariableScope>(state, G(variable_scope));
      call_frame->scope->initialize((Object*)call_frame->scope, nullptr, nullptr, 0);
      VariableScope::initialize(state, parent);

      call_frame->scope->set_parent(parent);
      parent->parent(nil<VariableScope>());

      TS_ASSERT(!instructions::push_local_depth(state, call_frame, depth, index));
      TS_ASSERT(kind_of<Exception>(state->vm()->thread_state()->current_exception()));
    };

    interpreter(1, 1, test);
  }

  void test_push_local_depth_pass() {
    InstructionTest test = lambda {
      intptr_t depth = 1;
      intptr_t index = 0;

      // set a local at index
      String* local = String::create(state, "blah");
      Tuple* tup = Tuple::create(state, 1);
      tup->put(state, 0, local);

      VariableScope* parent = VariableScope::synthesize(state, nil<CompiledCode>(), nil<Module>(),
        nil<Object>(),
        state->memory()->new_object<VariableScope>(state, G(variable_scope)),
        nil<BlockEnvironment>(), tup);
        call_frame->scope->initialize((Object*)call_frame->scope, nullptr, nullptr, 0);

      call_frame->scope->set_parent(parent);
      parent->parent(nil<VariableScope>());

      TS_ASSERT(instructions::push_local_depth(state, call_frame, depth, index));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<String>(res));
    };

    interpreter(1, 1, test);
  }

  void test_push_memo_nil() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t literal = reinterpret_cast<intptr_t>(cTrue);

      instructions::push_memo(call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_push_memo_push_opcode() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t literal = reinterpret_cast<intptr_t>(cNil);

      // TODO: instructions store_literal not implemented
      // instructions::push_memo(call_frame, literal);

      TS_ASSERT(literal);
    };

    interpreter(1, 0, test);
  }

  void test_push_mirror() {
    InstructionTest test = lambda {
      instructions::push_mirror(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop(), G(mirror));
    };

    interpreter(1, 0, test);
  }

  void test_push_my_field() {
    InstructionTest test = lambda {
      Array* ary = Array::create(state, 3);
      ary->set(state, 0, Fixnum::from(42)); // set values so size returns 3
      ary->set(state, 1, Fixnum::from(71));
      ary->set(state, 2, Fixnum::from(96));

      call_frame->scope->initialize(ary, cNil, nil<Module>(), 0);
      intptr_t index = 0;

      instructions::push_my_field(state, call_frame, index);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(3));
    };

    interpreter(1, 0, test);
  }

  void test_push_my_offset() {
    InstructionTest test = lambda {
      // TODO: fix. thought this should work
      TS_ASSERT(true);
//      Array* ary = Array::create(state, 3);
//      ary->set(state, 0, Fixnum::from(42)); // set values so size returns 3
//      ary->set(state, 1, Fixnum::from(71));
//      ary->set(state, 2, Fixnum::from(96));
//
//      call_frame->scope->initialize(ary, cNil, nil<Module>(), 0);
//      intptr_t index = 0;
//
//      instructions::push_my_offset(call_frame, index);
//
//      Object* res = reinterpret_cast<Object*>(stack_pop());
//
//      TS_ASSERT(res);
//      TS_ASSERT_EQUALS(res, ary);
    };

    interpreter(1, 0, test);
  }

  void test_push_nil() {
    InstructionTest test = lambda {
      instructions::push_nil(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), cNil);
    };

    interpreter(1, 0, test);
  }

  void test_push_proc_no_args() {
    InstructionTest test = lambda {
      call_frame->arguments = nullptr;
      TS_ASSERT(!instructions::push_proc(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_push_proc_invalid_block_type() {
    InstructionTest test = lambda {
      Arguments args(state->symbol("blah"));
      args.set_block(Array::create(state, 1));
      call_frame->arguments = &args;

      TS_ASSERT(!instructions::push_proc(state, call_frame));
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_push_proc_valid_block_type() {
    InstructionTest test = lambda {
      Arguments args(state->symbol("blah"));
      args.set_block(BlockEnvironment::allocate(state));
      call_frame->arguments = &args;

      TS_ASSERT(instructions::push_proc(state, call_frame));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Proc>(res));
    };

    interpreter(1, 0, test);
  }

  void test_push_proc_no_block() {
    InstructionTest test = lambda {
      Arguments args(state->symbol("blah"));

      call_frame->arguments = &args;

      TS_ASSERT(instructions::push_proc(state, call_frame));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, cNil);
    };

    interpreter(1, 0, test);
  }

  void test_push_rubinius() {
    InstructionTest test = lambda {
      instructions::push_rubinius(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop(), G(rubinius));
    };

    interpreter(1, 0, test);
  }

  void test_push_scope() {
    InstructionTest test = lambda {
      instructions::push_scope(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), call_frame->lexical_scope());
    };

    interpreter(1, 0, test);
  }

  void test_push_self() {
    InstructionTest test = lambda {
      instructions::push_self(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), call_frame->self());
    };

    interpreter(1, 0, test);
  }

  void test_push_stack_local() {
    InstructionTest test = lambda {
      intptr_t which = 8;
      Array* ary = Array::create(state, 0);
      call_frame->stk[10 - which - 1] = ary;

      instructions::push_stack_local(call_frame, which);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, ary);
    };

    interpreter(1, 0, test);
  }

  void test_push_true() {
    InstructionTest test = lambda {
      instructions::push_true(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), cTrue);
    };

    interpreter(1, 0, test);
  }

  void test_push_type() {
    InstructionTest test = lambda {
      instructions::push_type(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop(), G(type));
    };

    interpreter(1, 0, test);
  }

  void test_push_undef() {
    InstructionTest test = lambda {
      instructions::push_undef(state, call_frame);

      TS_ASSERT_EQUALS(stack_pop(), G(undefined));
    };

    interpreter(1, 0, test);
  }

  void test_push_variables() {
    InstructionTest test = lambda {
      // assigns compiled_code->machine_code which is needed to initialize
      // several variables required by this instruction test
      call_frame->compiled_code->internalize(state);

      instructions::push_variables(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<VariableScope>(res));
    };

    interpreter(1, 1, test);
  }

  void test_raise_break() {
    InstructionTest test = lambda {
      stack_push(cNil);

      // TODO: instructions
      // instructions::raise_break(state, call_frame);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_raise_exc() {
    InstructionTest test = lambda {
      Exception* exc = Exception::create(state);
      stack_push(exc);

      instructions::raise_exc(state, call_frame);

      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_raise_return() {
    InstructionTest test = lambda {
      stack_push(cNil);

      // TODO: instructions
      // instructions::raise_return(state, call_frame);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_reraise() {
    InstructionTest test = lambda {
      Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
      state->raise_exception(exc);

      instructions::reraise(state, call_frame);

      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_restore_exception_state_nil() {
    InstructionTest test = lambda {
      Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
      state->raise_exception(exc);
      TS_ASSERT(kind_of<Exception>(state->thread_state()->current_exception()));

      stack_push(cNil);

      instructions::restore_exception_state(state, call_frame);

      TS_ASSERT_EQUALS(nil<Exception>(), state->thread_state()->current_exception());
    };

    interpreter(1, 0, test);
  }

  void test_restore_exception_state_thread_state() {
    InstructionTest test = lambda {
      ThreadState* ts = ThreadState::create(state);
      ThreadState::initialize(state, ts);
      ts->raise_reason(Fixnum::from(cFiberCancel));
      stack_push(ts);

      instructions::restore_exception_state(state, call_frame);

      TS_ASSERT_EQUALS(cFiberCancel, state->thread_state()->raise_reason());
    };

    interpreter(1, 0, test);
  }

  void test_ret_no_flush_to_heap() {
    InstructionTest test = lambda {
      Object* obj = Fixnum::from(42);
      call_frame->compiled_code->internalize(state);
      stack_push(obj);

      instructions::ret(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_ret_flush_to_heap() {
    InstructionTest test = lambda {
      Object* obj = Fixnum::from(42);
      call_frame->compiled_code->internalize(state);
      call_frame->promote_scope(state); // causes flush to heap
      stack_push(obj);

      instructions::ret(state, call_frame);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_rotate() {
    InstructionTest test = lambda {
      Object* a = cTrue;
      Object* b = cFalse;
      Object* c = Array::create(state, 0);

      stack_push(a);
      stack_push(b);
      stack_push(c);
      intptr_t count = 2;

      instructions::rotate(call_frame, count);

      TS_ASSERT_EQUALS(stack_pop(), b);
      TS_ASSERT_EQUALS(stack_pop(), c);
      TS_ASSERT_EQUALS(stack_pop(), a);
    };

    interpreter(1, 0, test);
  }

  void test_run_exception() {
    InstructionTest test = lambda {
      stack_push(cNil);

      // TODO: instructions
      // instructions::run_exception(state, call_frame);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_send_method_method_exists() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      stack_push(recv);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(instructions::send_method(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
    };

    interpreter(1, 0, test);
  }

  void test_send_method_method_does_not_exist() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_nothing");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT_THROWS(instructions::send_method(state, call_frame, literal),
          const RubyException &);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_method_execute_returns_null() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnNull::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(!instructions::send_method(state, call_frame, literal));

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_method_exist() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(method_arg);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(instructions::send_stack(state, call_frame, literal, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_execute_returns_null() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnNull::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(method_arg);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(!instructions::send_stack(state, call_frame, literal, count));

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_method_no_method_error() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_doesnotexist");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(method_arg);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT_THROWS(instructions::send_stack(state, call_frame, literal, count),
          const RubyException &);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 1, test);
  }

  void test_send_stack_with_block_method_exists() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;
      BlockEnvironment* block = BlockEnvironment::allocate(state);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(method_arg);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(instructions::send_stack_with_block(state, call_frame, literal, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_with_block_no_method_error() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_nomethod");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;
      BlockEnvironment* block = BlockEnvironment::allocate(state);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(method_arg);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT_THROWS(instructions::send_stack_with_block(state, call_frame, literal, count),
        const RubyException &);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_with_block_returns_null() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnNull::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;
      BlockEnvironment* block = BlockEnvironment::allocate(state);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(method_arg);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(!instructions::send_stack_with_block(state, call_frame, literal, count));

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_with_splat_method_exists() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      BlockEnvironment* block = BlockEnvironment::allocate(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(cFalse); // method arg
      stack_push(args);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(instructions::send_stack_with_splat(state, call_frame, literal, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_with_splat_no_method_error() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_nomethod");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      BlockEnvironment* block = BlockEnvironment::allocate(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(cFalse); // method arg
      stack_push(args);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT_THROWS(instructions::send_stack_with_splat(state, call_frame, literal, count),
        const RubyException &);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_stack_with_splat_execute_return_null() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnNull::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      BlockEnvironment* block = BlockEnvironment::allocate(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(recv);
      stack_push(cFalse); // method arg
      stack_push(args);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(!instructions::send_stack_with_splat(state, call_frame, literal, count));

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_super_stack_with_block_method_exists() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;
      BlockEnvironment* block = BlockEnvironment::allocate(state);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(method_arg);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(instructions::send_super_stack_with_block(state, call_frame, literal, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_super_stack_with_block_no_method_error() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_nomethod");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;
      BlockEnvironment* block = BlockEnvironment::allocate(state);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(method_arg);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT_THROWS(instructions::send_super_stack_with_block(state, call_frame, literal, count),
        const RubyException &);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_super_stack_with_block_return_null() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnNull::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      Object* method_arg = cTrue;
      BlockEnvironment* block = BlockEnvironment::allocate(state);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(method_arg);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(!instructions::send_super_stack_with_block(state, call_frame, literal, count));

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_super_stack_with_splat_method_exists() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      BlockEnvironment* block = BlockEnvironment::allocate(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(cFalse); // method arg
      stack_push(args);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(instructions::send_super_stack_with_splat(state, call_frame, literal, count));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_super_stack_with_splat_no_method_error() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnArray::create(state);
      Symbol* sym_literal = state->symbol("to_nomethod");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      BlockEnvironment* block = BlockEnvironment::allocate(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(cFalse); // method arg
      stack_push(args);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT_THROWS(instructions::send_super_stack_with_splat(state, call_frame, literal, count),
        const RubyException &);

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_super_stack_with_splat_return_null() {
    InstructionTest test = lambda {
      Object* recv = RespondToToAryReturnNull::create(state);
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      call_frame->scope->initialize(recv, nullptr, nullptr, 0);
      BlockEnvironment* block = BlockEnvironment::allocate(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      intptr_t literal = reinterpret_cast<intptr_t>(call_site);
      intptr_t count = 1;

      Object** stack_ptr = STACK_PTR;

      stack_push(cFalse); // method arg
      stack_push(args);
      stack_push(block);

      state->vm()->set_call_frame(call_frame);
      TS_ASSERT(!instructions::send_super_stack_with_splat(state, call_frame, literal, count));

      TS_ASSERT_EQUALS(STACK_PTR, stack_ptr);
    };

    interpreter(1, 0, test);
  }

  void test_send_vcall_method_exists() {
    InstructionTest test = lambda {
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      Object* recv = RespondToToAryReturnArray::create(state);

      call_frame->scope->initialize(recv, cNil, nil<Module>(), 0);
      state->vm()->set_call_frame(call_frame);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      TS_ASSERT(instructions::send_vcall(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT(kind_of<Array>(res));
    };

    interpreter(1, 1, test);
  }

  void test_send_vcall_no_method_error() {
    InstructionTest test = lambda {
      Symbol* sym_literal = state->symbol("to_nomethod");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      Object* recv = RespondToToAryReturnArray::create(state);

      call_frame->scope->initialize(recv, cNil, nil<Module>(), 0);
      state->vm()->set_call_frame(call_frame);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      TS_ASSERT_THROWS(instructions::send_vcall(state, call_frame, literal),
        const RubyException &);
    };

    interpreter(1, 1, test);
  }

  void test_send_vcall_return_null() {
    InstructionTest test = lambda {
      Symbol* sym_literal = state->symbol("to_ary");
      CallSite* call_site = CallSite::create(state, sym_literal, 0);
      Object* recv = RespondToToAryReturnNull::create(state);

      call_frame->scope->initialize(recv, cNil, nil<Module>(), 0);
      state->vm()->set_call_frame(call_frame);
      intptr_t literal = reinterpret_cast<intptr_t>(call_site);

      TS_ASSERT(!instructions::send_vcall(state, call_frame, literal));
    };

    interpreter(1, 1, test);
  }

  void test_set_call_flags() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t flags = 0;

      // TODO: instructions
      // instructions::set_call_flags(call_frame);

      TS_ASSERT(!flags);
    };

    interpreter(1, 0, test);
  }

  void test_set_const() {
    InstructionTest test = lambda {
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, Module::create(state));
      scope->parent(state, call_frame->lexical_scope());
      call_frame->lexical_scope_ = scope;

      stack_push(Fixnum::from(42));
      intptr_t literal = reinterpret_cast<intptr_t>(state->symbol("ConstantVal"));

      instructions::set_const(state, call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_set_const_at() {
    InstructionTest test = lambda {
      stack_push(Module::create(state));
      stack_push(Fixnum::from(42));
      intptr_t literal = reinterpret_cast<intptr_t>(state->symbol("ConstantVal"));

      instructions::set_const_at(state, call_frame, literal);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_set_ivar() {
    InstructionTest test = lambda {
      call_frame->scope->initialize(Array::create(state, 0), nullptr, nullptr, 0);
      stack_push(Fixnum::from(42));
      intptr_t literal = reinterpret_cast<intptr_t>(state->symbol("@blah"));

      TS_ASSERT(instructions::set_ivar(state, call_frame, literal));

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_set_ivar_frozen_obj_raises() {
    InstructionTest test = lambda {
      Array* ary = Array::create(state, 0);
      ary->freeze(state);
      call_frame->scope->initialize(ary, nullptr, nullptr, 0);
      stack_push(Fixnum::from(42));
      intptr_t literal = reinterpret_cast<intptr_t>(state->symbol("@blah"));

      TS_ASSERT_EQUALS(cNil, (state->vm()->thread_state()->current_exception()));
      TS_ASSERT(!instructions::set_ivar(state, call_frame, literal));
      TS_ASSERT(kind_of<Exception>(state->vm()->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_set_local() {
    InstructionTest test = lambda {
      stack_push(Fixnum::from(42));
      intptr_t local = 0;

      instructions::set_local(call_frame, local);

      Object* res = reinterpret_cast<Object*>(stack_pop());

      TS_ASSERT(res);
      TS_ASSERT_EQUALS(res, Fixnum::from(42));
      TS_ASSERT_EQUALS(call_frame->scope->get_local(local), Fixnum::from(42));
    };

    interpreter(1, 1, test);
  }

  void test_set_local_depth() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t depth = 0;
      intptr_t index = 0;

      // TODO: instructions
      // instructions::set_local_depth(state, call_frame, depth, index);

      TS_ASSERT(!depth);
      TS_ASSERT(!index);
    };

    interpreter(1, 0, test);
  }

  void test_set_stack_local() {
    InstructionTest test = lambda {
      stack_push(Fixnum::from(42));
      intptr_t which = 6;

      stack_local(which) = Fixnum::from(0);

      TS_ASSERT_EQUALS(stack_local(which), Fixnum::from(0));
      instructions::set_stack_local(call_frame, which);

      TS_ASSERT_EQUALS(stack_local(which), Fixnum::from(42));
    };

    interpreter(1, 0, test);
  }

  void test_setup_unwind() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t ip = 0;
      intptr_t type = 0;

      // TODO: instructions
      // instructions::setup_unwind(call_frame, ip, type);

      TS_ASSERT(!ip);
      TS_ASSERT(!type);
    };

    interpreter(1, 0, test);
  }

  void test_shift_array_empty() {
    InstructionTest test = lambda {
      Array* original = Array::create(state, 0);
      stack_push(original);

      instructions::shift_array(state, call_frame);

      Object* shifted_value = stack_pop();
      Array* ary = try_as<Array>(stack_pop());

      TS_ASSERT(shifted_value);
      TS_ASSERT_EQUALS(shifted_value, cNil);
      TS_ASSERT(ary);
      TS_ASSERT_EQUALS(ary->size(), 0);
    };

    interpreter(2, 0, test);
  }

  void test_shift_array_has_elements() {
    InstructionTest test = lambda {
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
    };

    interpreter(2, 0, test);
  }

  void test_store_my_field() {
    InstructionTest test = lambda {
      Array* ary = Array::create(state, 3);
      ary->set(state, 0, Fixnum::from(42)); // set values so size returns 3
      ary->set(state, 1, Fixnum::from(71));
      ary->set(state, 2, Fixnum::from(96));

      call_frame->scope->initialize(ary, cNil, nil<Module>(), 0);
      stack_push(Fixnum::from(42));
      intptr_t index = 0;

      TS_ASSERT(instructions::store_my_field(state, call_frame, index));
    };

    interpreter(1, 0, test);
  }

  void test_string_append() {
    InstructionTest test = lambda {
      String* final_string = String::create(state, "rubinius");
      String* s1 = String::create(state, "rubini");
      String* s2 = String::create(state, "us");
      stack_push(s2);
      stack_push(s1);

      instructions::string_append(state, call_frame);

      String* string = try_as<String>(stack_pop());

      TS_ASSERT(string);
      TS_ASSERT_EQUALS(string->equal(state, final_string), cTrue);
    };

    interpreter(2, 0, test);
  }

  void test_string_build() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t count = 2;

      // TODO: instructions
      // instructions::string_build(state, call_frame, count);

      TS_ASSERT(count);
    };

    interpreter(1, 0, test);
  }

  void test_string_dup() {
    InstructionTest test = lambda {
      String* str = String::create(state, "blah");
      stack_push(str);

      TS_ASSERT(instructions::string_dup(state, call_frame));

      String* dup = (String*) stack_pop();
      TS_ASSERT_EQUALS(dup->data(), str->data());
    };

    interpreter(1, 0, test);
  }

  void test_swap() {
    InstructionTest test = lambda {
      Object* a = Fixnum::from(42);
      Object* b = Fixnum::from(71);
      stack_push(a);
      stack_push(b);

      instructions::swap(call_frame);

      TS_ASSERT_EQUALS(stack_pop(), a);
      TS_ASSERT_EQUALS(stack_pop(), b);
    };

    interpreter(2, 0, test);
  }

  void test_yield_debugger() {
    InstructionTest test = lambda {
      stack_push(cNil);

      // TODO: instructions
      // instructions::yield_debugger(state);

      TS_ASSERT(true);
    };

    interpreter(1, 0, test);
  }

  void test_yield_splat() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t count = 1;

      // TODO: instructions
      // instructions::yield_splat(state, call_frame, count);

      TS_ASSERT(count);
    };

    interpreter(1, 0, test);
  }

//  void test_yield_stack_passed_block() {
//    InstructionTest test = lambda {
//      BlockEnvironment* block = BlockEnvironment::allocate(state);
//      BlockEnvironment::initialize(state, block);
//      call_frame->scope->set_block(block);
//
//      LexicalScope* scope = LexicalScope::create(state);
//      scope->module(state, Module::create(state));
//      scope->parent(state, call_frame->lexical_scope());
//      call_frame->lexical_scope_ = scope;
//
//      Object* block_arg = Fixnum::from(42);
//      stack_push(block_arg);
//      intptr_t count = 1;
//
//      TS_ASSERT(instructions::yield_stack(state, call_frame, count));
//
//      TS_ASSERT(count);
//    };
//
//    interpreter(1, 1, test);
//  }

  void test_yield_stack_passed_proc() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t count = 1;

      // TODO: instructions
      // instructions::yield_stack(state, call_frame, count);

      TS_ASSERT(count);
    };

    interpreter(1, 0, test);
  }

  void test_yield_stack_nil_raises() {
    InstructionTest test = lambda {
      intptr_t count = 0;

      TS_ASSERT(!instructions::yield_stack(state, call_frame, count));

      TS_ASSERT(kind_of<Exception>(state->vm()->thread_state()->current_exception()));
    };

    interpreter(1, 0, test);
  }

  void test_yield_stack_passed_method() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t count = 1;

      // TODO: instructions
      // instructions::yield_stack(state, call_frame, count);

      TS_ASSERT(count);
    };

    interpreter(1, 0, test);
  }

  void test_zsuper() {
    InstructionTest test = lambda {
      stack_push(cNil);
      intptr_t literal = reinterpret_cast<intptr_t>(cNil);

      // TODO: instructions
      // instructions::zsuper(state, call_frame, literal);

      TS_ASSERT(literal);
    };

    interpreter(1, 0, test);
  }
};
