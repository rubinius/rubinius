# === Rubinius VM Instructions ===
# Keep this file in alphabetical order or suffer the scorn of your peers.
# When you add an instruction here, you MUST also add it to the master
# list in kernel/common/iseq.rb

# ==== Writing an instruction test ====
# The test harness for the VM instructions may require some explanation.
# Tests in this file are read by vm/codegen/instructions_gen.rb and written
# out into vm/test/test_instructions.hpp using the strings returned by the
# test_* methods in this file.
#
# It sets up various things for us, including:
# * Some C macros for test-writing convenience.
# * A CompiledMethod instance that will be 'running' in the test.
# * A Task with that CompiledMethod in its active context.
# * A zeroed-out stream of opcodes.
# The last three are available in tests as "cm", "task", and "stream".
#
# When the test body is reached, the test generator sets the first item
# in the opcode stream to the opcode we are testing.
# You will see various tests that set additional items in the stream after
# the opcode.  This is necessary to test opcodes that take arguments.
#
# The 'run()' macro is simply a shortcut for: task->execute_stream(stream).
# You can call run() as many times as you need in a single test. Each time,
# the VM will execute those instructions and leave the stream unmodified.
# If your second run() needs new arguments, you will need to set them by hand.
# WARNING: Make sure you pay attention to the stack contents if you are writing
# multiple scenarios in one test. run() does not reset the stack.
#
# Other useful functions:
# * task->push(val) pushes an object onto the stack
# * task->pop() pops the top item from the stack
# * task->stack_at(7) returns the 8th item in the stack
#
# Tests should be placed immediately after the instruction they are testing.
# Keep this file alphabetized by opcode name.

class Instructions

  # HACK dup'd in lib/compiler/generator.rb
  CALL_FLAG_PRIVATE = 1
  CALL_FLAG_CONCAT  = 2

  # [Operation]
  #   Add a new StaticScope entry for the given Module
  # [Format]
  #   \add_scope
  # [Stack Before]
  #   * #<Module>
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Create a new StaticScope object for the given Module on the stack.
  #   This scope is chained off the current scope of the method.
  #
  #   This also sets the scope of the current CompiledMethod to the new
  #   StaticScope
  #

  def add_scope
    <<-CODE
    Object* obj = stack_pop();
    Module* mod = as<Module>(obj);
    StaticScope* scope = StaticScope::create(state);
    scope->module(state, mod);
    scope->parent(state, call_frame->static_scope());
    call_frame->cm->scope(state, scope);
    call_frame->static_scope_ = scope;
    CODE
  end

  def test_add_scope
    <<-CODE
    StaticScope* scope = task->active()->cm()->scope();
    task->push(G(string));
    run();
    TS_ASSERT_DIFFERS(scope, task->active()->cm()->scope());
    TS_ASSERT_EQUALS(G(string), task->active()->cm()->scope()->module());
    CODE
  end

  def allow_private
    <<-CODE
    SET_ALLOW_PRIVATE(true);
    CODE
  end

  # [Operation]
  #   Convert a tuple to an array, or wrap an object within an array
  # [Format]
  #   \cast_array
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * array
  #   * ...
  # [Description]
  #   Removes the object on the top of the stack, and:
  #
  #   If the input is a tuple, a new array object is created based on the
  #   tuple data.
  #
  #   If the input is an array, it is unmodified.
  #
  #   If the input is any other type, that type is wrapped within a new array
  #   of length one.
  #
  #   The resulting array is then pushed back onto the stack.

  def cast_array
    <<-CODE
    Object* t1 = stack_pop();
    if(kind_of<Tuple>(t1)) {
      t1 = Array::from_tuple(state, as<Tuple>(t1));
    } else if(!kind_of<Array>(t1)) {
      Object* recv = G(array);
      Arguments args(recv, 1, &t1);
      Dispatch dis(G(sym_coerce_into_array));

      t1 = dis.send(state, call_frame, args);
    }
    stack_push(t1);
    CODE
  end

  def test_cast_array
    <<-CODE
    task->push(Qtrue);
    run();

    Array* ary = as<Array>(task->pop());
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);

    task->push(Qfalse);
    run();

    ary = as<Array>(task->pop());
    TS_ASSERT_EQUALS(ary->get(state, 0), Qfalse);

    Array* custom = Array::create(state, 1);
    task->push(custom);
    run();

    TS_ASSERT_EQUALS(task->stack_top(), custom);
    CODE
  end

  # [Operation]
  #   Evil twin for cast_array. The difference is how it handles nil.
  #   Cast_for_splat_block_arg converts nil to [], cast_array
  #   converts nil to [nil]
  # [Format]
  #   \cast_for_splat_block_arg
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * array
  #   * ...
  # [Description]
  #   Removes the object on the top of the stack, and:
  #
  #   If the input is nil, a new empty array object is created.
  #
  #   If the input is a tuple, a new array object is created based on the
  #   tuple data.
  #
  #   If the input is an array, it is unmodified.
  #
  #   If the input is any other type, that type is wrapped within a new array
  #   of length one.
  #
  #   The resulting array is then pushed back onto the stack.

  def cast_for_splat_block_arg
    <<-CODE
    Array* ary = Array::create(state, args.total());
    for(size_t i = 0; i < args.total(); i++) {
      ary->set(state, i, args.get_argument(i));
    }

    stack_push(ary);
    CODE
  end

  def test_cast_for_splat_block_arg
    <<-CODE
    task->push(Qnil);
    run();

    Array* ary = as<Array>(task->pop());
    TS_ASSERT_EQUALS(ary->num_fields(), 0);

    task->push(Qtrue);
    run();

    Array* ary = as<Array>(task->pop());
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);

    task->push(Qfalse);
    run();

    ary = as<Array>(task->pop());
    TS_ASSERT_EQUALS(ary->get(state, 0), Qfalse);

    Array* custom = Array::create(state, 1);
    task->push(custom);
    run();

    TS_ASSERT_EQUALS(task->stack_top(), custom);
    CODE
  end

  # [Operation]
  #   Converts a block argument single-valued tuple into multiple arguments if
  #   the arg is an array
  # [Format]
  #   \cast_for_multi_block_arg
  # [Stack Before]
  #   * tuple[array[el1,el2,...,eln]]
  #   * ...
  # [Stack After]
  #   * tuple[el1,el2,...,eln]
  #   * ...
  # [Description]
  #   If the tuple on the top of the stack has only a single element, and that
  #   element is an array, a new tuple is created containing the contents of
  #   the array, and this new tuple is used to update the top of the stack.
  # [Example]
  #   <code>
  #     [[1,2,3]].each do |i,j,k|
  #       # do something
  #     end
  #   </code>

  def cast_for_multi_block_arg
    <<-CODE
    /* If there is only one argument and that thing is an array... */
    if(args.total() == 1 && kind_of<Array>(args.get_argument(0))) {
      stack_push(args.get_argument(0));
    } else {
      Array* ary = Array::create(state, args.total());
      for(size_t i = 0; i < args.total(); i++) {
        ary->set(state, i, args.get_argument(i));
      }
      stack_push(ary);
    }
    CODE
  end

  def test_cast_for_multi_block_arg
    <<-CODE
    Tuple* tup = Tuple::from(state, 2, Qtrue, Qfalse);
    task->push(tup);
    run();

    TS_ASSERT_EQUALS(task->stack_top(), tup);

    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Fixnum::from(1));
    tup = Tuple::from(state, 1, ary);
    task->active()->set_top(tup);
    run();

    tup = as<Tuple>(task->stack_top());
    TS_ASSERT_EQUALS(tup->at(state, 0), Fixnum::from(1));
    CODE
  end

  # [Operation]
  #   Converts the item on the top of the stack into an argument for a block
  #   taking one arg
  # [Format]
  #   \cast_for_single_block_arg
  # [Stack Before]
  #   * arg
  #   * ...
  # [Stack After]
  #   * block_arg
  #   * ...
  # [Description]
  #   The item on the top of the stack is popped, and:
  #
  #   If it has no fields, the result is nil
  #
  #   If the item contains a single field, the result is the value in the
  #   first field
  #
  #   If the item is a tuple, the result is an array created from the tuple.
  #
  #   The result is then pushed onto the stack.

  def cast_for_single_block_arg
    <<-CODE
    int k = args.total();
    if(k == 0) {
      stack_push(Qnil);
    } else if(k == 1) {
      stack_push(args.get_argument(0));
    } else {
      Array* ary = Array::create(state, k);
      for(int i = 0; i < k; i++) {
        ary->set(state, i, args.get_argument(i));
      }
      stack_push(ary);
    }
    CODE
  end

  def test_cast_for_single_block_arg
    <<-CODE
    Tuple* tup = Tuple::create(state, 0);
    task->push(tup);
    run();

    TS_ASSERT_EQUALS(task->pop(), Qnil);

    tup = Tuple::from(state, 1, Qtrue);
    task->push(tup);
    run();

    TS_ASSERT_EQUALS(task->pop(), Qtrue);

    tup = Tuple::from(state, 2, Qtrue, Qfalse);
    task->push(tup);
    run();

    Array* ary = as<Array>(task->stack_top());
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qfalse);
    CODE
  end

  # [Operation]
  #   Checks if the specified method serial number matches an expected value
  # [Format]
  #   \check_serial sendsite serial
  # [Stack Before]
  #   * obj
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Pops an object off the stack, and determines whether the serial number
  #   of the method identified by the literal +method+ is the same as the
  #   expected value +serial+. The result is pushed back on the stack as the
  #   value +true+ or +false+.
  # [Notes]
  #   This opcode is typically used to determine at runtime whether an
  #   optimisation can be performed. At compile time, two code paths are
  #   generated: a slow, but guaranteed correct path, and a fast path that
  #   uses certain optimisations. The serial number check is then performed at
  #   runtime to determine which code path is executed.
  #
  #   For example, a method such as Fixnum#times can be optimised at compile
  #   time, but we can't know until runtime whether or not the Fixnum#times
  #   method has been overridden. The serial number check is used to determine
  #   each time the code is executed, whether or not the standard Fixnum#times
  #   has been overridden. It leverages the serial number field on a
  #   CompiledMethod, is initialised to either 0 (for kernel land methods) or
  #   1 (for user land methods).

  def check_serial(index, serial)
    <<-CODE
    Object* recv = stack_pop();
    SendSite* ss = as<SendSite>(call_frame->cm->literals()->at(state, index));

    if(ss->check_serial(state, call_frame, recv, serial)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def test_check_serial
    <<-CODE
      Fixnum* s = Fixnum::from(100);
      Symbol* sym = String::create(state, "to_s")->to_sym(state);
      SendSite* ss = SendSite::create(state, sym);
      call_frame->cm->literals()->put(state, 0, ss);

      TS_ASSERT_EQUALS(Qnil, cm->serial());
      task->add_method(G(fixnum_class), sym, cm);
      TS_ASSERT_EQUALS(Fixnum::from(0), cm->serial());

      task->push(s);
      stream[1] = (opcode)0;
      stream[2] = (opcode)0;

      run();
      TS_ASSERT_EQUALS(Qtrue, task->pop());

      task->push(s);
      TS_ASSERT_EQUALS(Fixnum::from(0), cm->serial());
      stream[1] = (opcode)0;
      stream[2] = (opcode)1;
      run();
      TS_ASSERT_EQUALS(Qfalse, task->pop());
    CODE
  end

  # [Operation]
  #   Clears any exceptions from the current execution context
  # [Format]
  #   \clear_exception
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Clears any exceptions from the current execution context. The stack is
  #   untouched by this opcode.

  def clear_exception
    <<-CODE
    // Don't allow this code to clear non-exception raises
    if(state->thread_state()->raise_reason() == cException) {
      state->thread_state()->clear_exception();
    }
    CODE
  end

  def test_clear_exception
    <<-CODE
    task->exception(state, Exception::create(state));
    run();
    TS_ASSERT_EQUALS(task->exception(), Qnil);
    CODE
  end

  # [Operation]
  #   Creates a block
  # [Format]
  #   \create_block literal
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * block_env
  #   * ...
  # [Description]
  #   Takes a +compiled_method+ out of the literals tuple, and converts it
  #   into a block environment +block_env+, which is then pushed back onto the
  #   stack.

  def create_block(index)
    <<-CODE
    Object* _lit = call_frame->cm->literals()->at(state, index);
    CompiledMethod* cm = as<CompiledMethod>(_lit);

    call_frame->promote_scope(state);

    // TODO: We don't need to be doing this everytime.
    cm->scope(state, call_frame->static_scope());

    Object* be = BlockEnvironment::under_call_frame(state, cm, vmm, call_frame, index);

    stack_push(be);
    CODE
  end

  # [Operation]
  #   Duplicate the top item on the stack
  # [Format]
  #   \dup_top
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * value
  #   * value
  #   * ...
  # [Description]
  #   Duplicate the top value on the operand stack and push the duplicated
  #   value onto the operand stack.

  def dup_top
    <<-CODE
    Object* t1 = stack_top();
    stack_push(t1);
    CODE
  end

  def test_dup_top
    <<-CODE
    task->push(Qtrue);

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 1);
    TS_ASSERT_EQUALS(task->stack_at(1), Qtrue);
    CODE
  end

  # [Operation]
  #   Finds a constant
  # [Format]
  #   \find_const constant
  # [Stack Before]
  #   * parent
  #   * ...
  # [Stack After]
  #   * const
  #   * ...
  # [Description]
  #   Pops the module or \class +ns+ off the stack, and searches within it's
  #   namespace for the constant identified by the literal +constant+. If
  #   found, it is pushed onto the stack; otherwise, nothing is pushed onto
  #   the stack, and a NameError exception is raised.
  # [Example]
  #   <code>
  #     str = "abc"
  #     enum = Enumerable::Enumerator(str, :each_byte)
  #   </code>

  def find_const(index)
    <<-CODE
    bool found;
    Module* under = as<Module>(stack_pop());
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    Object* res = Helpers::const_get(state, under, sym, &found);
    if(!found) {
      res = Helpers::const_missing(state, under, sym, call_frame);
    } else if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, call_frame);
    }

    HANDLE_EXCEPTION(res);

    stack_push(res);
    CODE
  end

  def test_find_const
    <<-CODE
    Symbol* name = state->symbol("Number");
    G(true_class)->set_const(state, name, Fixnum::from(3));

    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    task->push(G(true_class));

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(3));
    CODE
  end

  # [Operation]
  #   Unconditionally jump execution to the position specified by the label
  # [Format]
  #   \goto label
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Moves the instruction pointer to the instruction following the specified
  #   label without disturbing the stack.
  # [See Also]
  #   * goto_if_true
  #   * goto_if_false
  #   * goto_if_defined

  def goto(location)
    <<-CODE
    call_frame->set_ip(location);
    DISPATCH;
    CODE
  end

  def test_goto
    <<-CODE
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->current_ip(), 15);
    CODE
  end

  # [Operation]
  #   Jump execution to the position specified by the label if the top of the
  #   stack is not undefined.
  # [Format]
  #   \goto_if_defined label
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Remove the top item on the stack, and if it is not the special value
  #   +undefined+, jump to the instruction following the specified label;
  #   otherwise, continue.
  # [See Also]
  #   * goto
  #   * goto_if_true
  #   * goto_if_false

  def goto_if_defined(location)
    <<-CODE
    Object* t1 = stack_pop();
    if(t1 != Qundef) {
      call_frame->set_ip(location);
    }
    CODE
  end

  def test_goto_if_defined
    <<-CODE
    task->push(Qundef);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    TS_ASSERT_EQUALS(task->current_ip(), 0);

    task->push(Qtrue);
    run();
    TS_ASSERT_EQUALS(task->current_ip(), 15);
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    CODE
  end

  # [Operation]
  #   Jump execution to the position specified by the label if the top of the
  #   stack evaluates to false.
  # [Format]
  #   \goto_if_false label
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Remove the top item on the stack, and if +nil+ or +false+, jump to the
  #   instruction following the specified label; otherwise, continue.
  # [See Also]
  #   * goto
  #   * goto_if_false
  #   * goto_if_defined

  def goto_if_false(location)
    <<-CODE
    Object* t1 = stack_pop();
    if(!RTEST(t1)) {
      call_frame->set_ip(location);
      DISPATCH;
    }
    CODE
  end

  def test_goto_if_false
    <<-CODE
    task->push(Qtrue);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    TS_ASSERT_EQUALS(task->current_ip(), 0);

    task->push(Qfalse);
    run();
    TS_ASSERT_EQUALS(task->current_ip(), 15);
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    CODE
  end

  # [Operation]
  #   Jump execution to the position specified by the label if the top of the
  #   stack evaluates to true.
  # [Format]
  #   \goto_if_true label
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Remove the top item on the stack, and if not +nil+ or +false+, jump to
  #   the instruction following the specified label; otherwise, continue.
  # [See Also]
  #   * goto
  #   * goto_if_false
  #   * goto_if_defined

  def goto_if_true(location)
    <<-CODE
    Object* t1 = stack_pop();
    if(RTEST(t1)) {
      call_frame->set_ip(location);
      DISPATCH;
    }
    CODE
  end

  def test_goto_if_true
    <<-CODE
    task->push(Qfalse);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    TS_ASSERT_EQUALS(task->current_ip(), 0);

    task->push(Qtrue);
    run();
    TS_ASSERT_EQUALS(task->current_ip(), 15);
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    CODE
  end

  # [Operation]
  #   Evaluate if +object+ is an instance of +class+
  # [Format]
  #   \instance_of
  # [Stack Before]
  #   * object
  #   * class
  #   * ...
  # [Stack After]
  #   * result
  #   * ...
  # [Description]
  #   If the object is an instance of +\class+ return the special value +true+,
  #   otherwise return +false+. This check is different than +kind_of+ in that
  #   it does not evaluate superclass relationships. Instance-specific
  #   subtyping via metaclasses are ignored in computing the parent \class.
  # [See Also]
  #   * kind_of

  def instance_of
    <<-CODE
    Object* t1 = stack_pop();
    Class* cls = as<Class>(stack_pop());
    if(t1->class_object(state) == cls) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def test_instance_of
    <<-CODE
    Class* parent = state->new_class("Parent", G(object));
    Class* child =  state->new_class("Child", parent);

    Object* objp = state->new_object<Object>(parent);
    Object* objc = state->new_object<Object>(child);

    /* standard */
    task->push(parent);
    task->push(objp);
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());

    /* subclass */
    task->push(parent);
    task->push(objc);
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());

    /* superclass */
    task->push(child);
    task->push(objp);
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());
    CODE
  end

  # [Operation]
  #   Return true if value is nil, otherwise false
  # [Format]
  #   \is_nil
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * result
  #   * ...
  # [Description]
  #   Consume the value on the stack, and put the special values true or false
  #   depending on whether the consumed value was the special value nil

  def is_nil
    <<-CODE
    Object* t1 = stack_pop();
    stack_push(t1 == Qnil ? Qtrue : Qfalse);
    CODE
  end

  def test_is_nil
    <<-CODE
    task->push(Qnil);
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());
    task->push(String::create(state, "no"));
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());
    task->push(Integer::from(state, 0));
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());
    CODE
  end

  # [Operation]
  #   Evaluate if +object+ is an instance of +\class+ or of an ancestor of
  #   +\class+.
  # [Format]
  #   \kind_of
  # [Stack Before]
  #   * object
  #   * class
  #   * ...
  # [Stack After]
  #   * result
  #   * ...
  # [Description]
  #   Evaluate if the object is created by +\class+, its parent, or a further
  #   ancestor. This differs from +instance_of+ in that the \class heirarchy
  #   will be evaluated rather than just the \class object given.
  # [See Also]
  #   * instance_of

  def kind_of
    <<-CODE
    Object* t1 = stack_pop();
    Object* mod = stack_pop();
    if(t1->kind_of_p(state, mod)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def test_kind_of
    <<-CODE
    Class* parent = state->new_class("Parent", G(object));
    Class* child =  state->new_class("Child", parent);

    Object* objp = state->new_object<Object>(parent);
    Object* objc = state->new_object<Object>(child);

    /* standard */
    task->push(parent);
    task->push(objp);
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());

    /* subclass */
    task->push(parent);
    task->push(objc);
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());

    /* superclass */
    task->push(child);
    task->push(objp);
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());
    CODE
  end

  # [Operation]
  #   Create an array and populate with items on the stack
  # [Format]
  #   \make_array argc
  # [Stack Before]
  #   * valueN
  #   * ...
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * [value1, value2, ..., valueN]
  #   * ...
  # [Description]
  #   Creates a new array, populating its contents with the number of items
  #   (+argc+) specified in the opcode. The contents of the new array are
  #   taken from the stack, with the top item on the stack becoming the last
  #   item in the array. The resulting array is added back to the stack.

  def make_array(count)
    <<-CODE
    Object* t2;
    Array* ary = Array::create(state, count);
    int j = count - 1;
    for(; j >= 0; j--) {
      t2 = stack_pop();
      ary->set(state, j, t2);
    }

    stack_push(ary);
    CODE
  end

  def test_make_array
    <<-CODE
    task->push(Qtrue);
    task->push(Qfalse);

    stream[1] = 2;
    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    Array* ary = as<Array>(task->stack_top());
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qfalse);
    CODE
  end

  # [Operation]
  #   Pushes 0 onto the stack
  # [Format]
  #   \meta_push_0
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * 0
  #   * ...
  # [Description]
  #   Fast push of 0 (zero) onto the stack. This is an optimisation applied
  #   whenever a literal 0 is encountered by the compiler. It is equivalent to
  #   'push 0', but faster because there is no need to lookup the literal
  #   value in the literals tuple.

  def meta_push_0
    <<-CODE
    stack_push(Fixnum::from(0));
    CODE
  end

  def test_meta_push_0
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT(kind_of<Fixnum>(task->stack_top()));
    TS_ASSERT_EQUALS(as<Integer>(task->stack_top())->to_native(), 0);
    CODE
  end

  # [Operation]
  #   Pushes 1 onto the stack
  # [Format]
  #   \meta_push_1
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * 1
  #   * ...
  # [Description]
  #   Fast push of 1 (one) onto the stack. This is an optimisation applied
  #   whenever a literal 1 is encountered by the compiler. It is equivalent to
  #   'push 1', but faster because there is no need to lookup the literal
  #   value in the literals tuple.

  def meta_push_1
    <<-CODE
    stack_push(Fixnum::from(1));
    CODE
  end

  def test_meta_push_1
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT(kind_of<Fixnum>(task->stack_top()));
    TS_ASSERT_EQUALS(as<Integer>(task->stack_top())->to_native(), 1);
    CODE
  end

  # [Operation]
  #   Pushes 2 onto the stack
  # [Format]
  #   \meta_push_2
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * 2
  #   * ...
  # [Description]
  #   Fast push of 2 (two) onto the stack. This is an optimisation applied
  #   whenever a literal 2 is encountered by the compiler. It is equivalent to
  #   'push 2', but faster because there is no need to lookup the literal
  #   value in the literals tuple.

  def meta_push_2
    <<-CODE
    stack_push(Fixnum::from(2));
    CODE
  end

  def test_meta_push_2
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT(kind_of<Fixnum>(task->stack_top()));
    TS_ASSERT_EQUALS(as<Integer>(task->stack_top())->to_native(), 2);
    CODE
  end

  # [Operation]
  #   Pushes -1 onto the stack
  # [Format]
  #   \meta_push_neg_1
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * -1
  #   * ...
  # [Description]
  #   Fast push of -1 (negative 1) onto the stack. This is an optimisation
  #   applied whenever a literal -1 is encountered by the compiler. It is
  #   equivalent to 'push -1', but faster because there is no need to lookup
  #   the literal value in the literals tuple.

  def meta_push_neg_1
    <<-CODE
    stack_push(Fixnum::from(-1));
    CODE
  end

  def test_meta_push_neg_1
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT(kind_of<Fixnum>(task->stack_top()));
    TS_ASSERT_EQUALS(as<Integer>(task->stack_top())->to_native(), -1);
    CODE
  end

  # [Operation]
  #   Used for non-dynamic 'yield' calls and for simple calls with static args
  # [Format]
  #   \meta_send_call count
  # [Stack Before]
  #   * argN
  #   * ...
  #   * arg1
  #   * receiver
  # [Stack After]
  #   * retval
  # [Description]
  #   Simplified call instruction used for yields and basic calls

  def meta_send_call(count)
    <<-CODE
    Object* t1 = stack_back(count);
    Object* ret;

    Arguments out_args(Qnil, count, stack_back_position(count));

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      ret = env->call(state, call_frame, out_args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      ret = proc->call(state, call_frame, out_args);
    } else {
      ret = send_slowly(state, vmm, call_frame, G(sym_call),
        stack_back_position(count + 1), count);
    }

    stack_clear(count + 1);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def yield_stack(count)
    <<-CODE
    Object* t1 = call_frame->top_scope()->block();
    Object* ret;
    Arguments args(t1, count, stack_back_position(count));

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      ret = env->call(state, call_frame, args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      ret = proc->yield(state, call_frame, args);
    } else if(t1->nil_p()) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "no block given");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      ret = NULL;
    } else {
      Dispatch dis(G(sym_call));
      ret = dis.send(state, call_frame, args);
    }

    stack_clear(count);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def yield_splat(count)
    <<-CODE
    Object* ary = stack_pop();
    Object* t1 = call_frame->top_scope()->block();

    Arguments args(t1, count, stack_back_position(count));

    if(!ary->nil_p()) {
      args.append(state, as<Array>(ary));
    }

    Object* ret;
    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      ret = env->call(state, call_frame, args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      ret = proc->yield(state, call_frame, args);
    } else if(t1->nil_p()) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "no block given");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      ret = NULL;
    } else {
      Dispatch dis(G(sym_call));
      ret = dis.send(state, call_frame, args);
    }

    stack_clear(count);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end


  def test_meta_send_call
    <<-CODE
    CompiledMethod* block_method = CompiledMethod::create(state);
    block_method->iseq(state, InstructionSequence::create(state, 2));
    block_method->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_true));
    block_method->iseq()->opcodes()->put(state, 1, Fixnum::from(InstructionSequence::insn_ret));
    block_method->total_args(state, Fixnum::from(2));
    block_method->required_args(state, Fixnum::from(2));
    block_method->stack_size(state, Fixnum::from(10));
    block_method->formalize(state);

    call_frame->cm->literals()->put(state, 0, block_method);

    /* Run the create_block instruction, since that is how BlockEnvs are created */
    stream[0] = InstructionSequence::insn_create_block;
    stream[1] = (opcode)0; /* Create a BlockEnv from index 0 */

    run();

    BlockEnvironment* block = as<BlockEnvironment>(task->pop());

    stream[0] = InstructionSequence::insn_meta_send_call;
    stream[1] = (opcode)2;

    task->push(block);
    task->push(Qnil); /* arg1 */
    task->push(Qfalse); /* arg2 */

    run();

    Tuple* args = as<Tuple>(task->pop());
    TS_ASSERT_EQUALS(Qnil, args->at(state, 0));
    TS_ASSERT_EQUALS(Qfalse, args->at(state, 1));

    TS_ASSERT_EQUALS(Fixnum::from(InstructionSequence::insn_push_true), task->active()->cm()->iseq()->opcodes()->at(state, 0));
    TS_ASSERT_EQUALS(Fixnum::from(InstructionSequence::insn_ret), task->active()->cm()->iseq()->opcodes()->at(state, 1));
    CODE
  end

  # [Operation]
  #   Implementation of == optimised for fixnums and symbols
  # [Format]
  #   \meta_send_op_equal
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of (+value1+ == +value2+). If +value1+ and +value2+ are both fixnums or
  #   both symbols, the comparison is done directly; otherwise, the == method
  #   is called on +value1+, passing +value2+ as the argument.

  def meta_send_op_equal
    <<-CODE
    Object* t1 = stack_back(1);
    Object* t2 = stack_back(0);
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qtrue : Qfalse);
    } else {
      Object* ret = send_slowly(state, vmm, call_frame, G(sym_equal),
        stack_back_position(2), 1);
      stack_clear(2);

      HANDLE_EXCEPTION(ret);
      stack_push(ret);
    }
    CODE
  end

  def test_meta_send_op_equal
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(two);
    task->push(one);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Qfalse);

    CODE
  end

  # [Operation]
  #   Implementation of > optimised for fixnums
  # [Format]
  #   \meta_send_op_gt
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of (+value1+ > +value2+). If +value1+ and +value2+ are both fixnums, the
  #   comparison is done directly; otherwise, the > method is called on
  #   +value1+, passing +value2+ as the argument.

  def meta_send_op_gt
    <<-CODE
    Object* t1 = stack_back(1);
    Object* t2 = stack_back(0);
    if(both_fixnum_p(t1, t2)) {
      native_int j = as<Integer>(t1)->to_native();
      native_int k = as<Integer>(t2)->to_native();
      stack_pop();
      stack_set_top((j > k) ? Qtrue : Qfalse);
    } else {
      Object* ret = send_slowly(state, vmm, call_frame, G(sym_gt),
        stack_back_position(2), 1);
      stack_clear(2);

      HANDLE_EXCEPTION(ret);
      stack_push(ret);
    }
    CODE
  end

  def test_meta_send_op_gt
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(one);
    task->push(two);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Qfalse);

    CODE
  end

  # [Operation]
  #   Implementation of < optimised for fixnums
  # [Format]
  #   \meta_send_op_lt
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of (+value1+ < +value2+). If +value1+ and +value2+ are both fixnums, the
  #   comparison is done directly; otherwise, the < method is called on
  #   +value1+, passing +value2+ as the argument.

  def meta_send_op_lt
    <<-CODE
    Object* t1 = stack_back(1);
    Object* t2 = stack_back(0);
    if(both_fixnum_p(t1, t2)) {
      native_int j = as<Integer>(t1)->to_native();
      native_int k = as<Integer>(t2)->to_native();
      stack_pop();
      stack_set_top((j < k) ? Qtrue : Qfalse);
    } else {
      Object* ret = send_slowly(state, vmm, call_frame, G(sym_lt),
        stack_back_position(2), 1);
      stack_clear(2);

      HANDLE_EXCEPTION(ret);
      stack_push(ret);
    }
    CODE
  end

  def test_meta_send_op_lt
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(one);
    task->push(two);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);

    CODE
  end

  # [Operation]
  #   Implementation of - optimised for fixnums
  # [Format]
  #   \meta_send_op_minus
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * value1 - value2
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of (+value1+ - +value2+). If +value1+ and +value2+ are both fixnums, the
  #   subtraction is done directly via the fixnum_sub primitive; otherwise,
  #   the - method is called on +value1+, passing +value2+ as the argument.

  def meta_send_op_minus
    <<-CODE
    Object* left =  stack_back(1);
    Object* right = stack_back(0);

    if(both_fixnum_p(left, right)) {
      stack_pop();
      stack_set_top(((Fixnum*)(left))->sub(state, (Fixnum*)(right)));
    } else {
      Object* ret = send_slowly(state, vmm, call_frame, G(sym_minus),
        stack_back_position(2), 1);
      stack_clear(2);

      HANDLE_EXCEPTION(ret);
      stack_push(ret);
    }
    CODE
  end

  def test_meta_send_op_minus
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(two);
    task->push(one);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(1));

    CODE
  end

  # [Operation]
  #   Implementation of + optimised for fixnums
  # [Format]
  #   \meta_send_op_plus
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * value1 + value2
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of (+value1+ + +value2+). If +value1+ and +value2+ are both fixnums, the
  #   addition is done directly via the fixnum_add primitive; otherwise, the +
  #   method is called on +value1+, passing +value2+ as the argument.

  def meta_send_op_plus
    <<-CODE
    Object* left =  stack_back(1);
    Object* right = stack_back(0);

    if(both_fixnum_p(left, right)) {
      stack_pop();
      stack_pop();
      Object* res = ((Fixnum*)(left))->add(state, (Fixnum*)(right));
      stack_push(res);
    } else {
      Object* ret = send_slowly(state, vmm, call_frame, G(sym_plus),
        stack_back_position(2), 1);
      stack_clear(2);

      HANDLE_EXCEPTION(ret);
      stack_push(ret);
    }
    CODE
  end

  def test_meta_send_op_plus
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(one);
    task->push(two);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(3));

    CODE
  end

  # [Operation]
  #   Implementation of === (triple \equal) optimised for fixnums and symbols
  # [Format]
  #   \meta_send_op_tequal
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of (+value1+ === +value2+). If +value1+ and +value2+ are both fixnums or
  #   both symbols, the comparison is done directly; otherwise, the === method
  #   is called on +value1+, passing +value2+ as the argument.
  # [Notes]
  #   Exactly like equal, except calls === if it can't handle it directly.

  def meta_send_op_tequal
    <<-CODE
    Object* t1 = stack_back(1);
    Object* t2 = stack_back(0);
    /* If both are fixnums, or both are symbols, compare the ops directly. */
    if((t1->fixnum_p() && t2->fixnum_p()) || (t1->symbol_p() && t2->symbol_p())) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qtrue : Qfalse);
    } else {
      Object* ret = send_slowly(state, vmm, call_frame, G(sym_tequal),
        stack_back_position(2), 1);
      stack_clear(2);

      HANDLE_EXCEPTION(ret);
      stack_push(ret);
    }
    CODE
  end

  def test_meta_send_op_tequal
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(two);
    task->push(one);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Qfalse);

    CODE
  end

  # [Operation]
  #   Performs a no-op, i.e. does nothing.
  # [Format]
  #   \noop
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   The classic no-op operator; performs no actions, and does not modify the
  #   stack.
  #
  #   To consume an item from the stack, but otherwise do nothing, use
  #   pop.
  # [See Also]
  #   * pop

  def noop
    <<-CODE
    CODE
  end

  def test_noop
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    CODE
  end

  # [Operation]
  #   Test to determine whether an argument was passed
  # [Format]
  #   \passed_arg index
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Checks if the number of arguments passed to a method is greater than the
  #   specified argument index +index+ (0-based), and pushes the result of the
  #   test onto the stack.

  def passed_arg(index)
    <<-CODE
    if(index < (int)args.total()) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def test_passed_arg
    <<-CODE
    task->active()->args = 0;
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->pop(), Qfalse);

    task->active()->args = 1;

    run();

    TS_ASSERT_EQUALS(task->pop(), Qtrue);
    CODE
  end


  # [Operation]
  #   Test to determine whether a block argument was passed
  # [Format]
  #   \passed_blockarg count
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Checks if a block was passed to a method, and pushes the result of the
  #   test onto the stack.

  def passed_blockarg(count)
    <<-CODE
    if(count == (int)args.total()) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def test_passed_blockarg
    <<-CODE
    task->blockargs = 0;
    stream[1] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->pop(), Qfalse);

    task->blockargs = 1;

    run();

    TS_ASSERT_EQUALS(task->pop(), Qtrue);
    CODE
  end


  # [Operation]
  #   Pop an item off the stack and discard
  # [Format]
  #   \pop
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Removes the top item from the stack, discarding it.
  # [Notes]
  #   Pop is typically used when the return value of another opcode is not
  #   required.

  def pop
    <<-CODE
    stack_pop();
    CODE
  end

  def test_pop
    <<-CODE
    task->push(Qtrue);

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), -1);
    CODE
  end

  # [Operation]
  #   Pop the first item off the stack and fill $! with it.
  # [Format]
  #   \pop_exception
  # [Stack Before]
  #   * exception
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   The top item on the stack becomes the current exception

  def pop_exception
    <<-CODE
      Object* top = stack_pop();
      if(top->nil_p()) {
        state->thread_state()->clear_exception();
      } else {
        state->thread_state()->set_exception(state, top);
      }
    CODE
  end

  def test_pop_exception
    <<-CODE
      Exception* exc = Exception::create(state);
      task->push(exc);

      TS_ASSERT_EQUALS(task->exception(), Qnil);
      run();
      TS_ASSERT_EQUALS(task->exception(), exc);
    CODE
  end

  # [Operation]
  #   Pushes a block onto the stack
  # [Format]
  #   \push_block
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * block
  #   * ...
  # [Description]
  #   Pushes the current block onto the stack. Used when a block passed to a
  #   method is used.
  # [Example]
  #   <code>
  #     def takes_block
  #       yield # yields to the block passed to the method, which causes
  #             # push_block to be called
  #     end
  #   </code>

  def push_block
    <<-CODE
    stack_push(call_frame->top_scope()->block());
    CODE
  end

  def test_push_block
    <<-CODE
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active(), task->active(), 0);
    task->active()->block(state, be);
    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), be);
    CODE
  end

  # [Operation]
  #   Pushes a constant onto the stack
  # [Format]
  #   \push_const constant
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * const
  #   * ...
  # [Description]
  #   Locates the constant indicated by the literal +constant+ from the
  #   current context, and pushes it onto the stack. If the constant cannot be
  #   found in the current context, nothing is pushed onto the stack, and a
  #   NameError exception is raised.
  # [Example]
  #   <code>
  #     engine = RUBY_ENGINE # RUBY_ENGINE is a constant defined by Rubinius
  #   </code>

  def push_const(index)
    <<-CODE
    bool found;
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    Object* res = Helpers::const_get(state, call_frame, sym, &found);
    if(!found) {
      Module* under;
      StaticScope* scope = call_frame->static_scope();
      if(scope->nil_p()) {
        under = G(object);
      } else {
        under = scope->module();
      }
      res = Helpers::const_missing(state, under, sym, call_frame);
    } else if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, call_frame);
    }

    HANDLE_EXCEPTION(res);

    stack_push(res);
    CODE
  end

  def test_push_const
    <<-CODE
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    StaticScope* cs = StaticScope::create(state);
    cs->module(state, child);
    cs->parent(state, ps);

    cm->scope(state, cs);

    Symbol* name = state->symbol("Number");
    parent->set_const(state, name, Fixnum::from(3));

    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(3));

    CODE
  end

  # [Operation]
  #   Pushes a constant onto the stack. Caches the lookup to provide
  #   faster future lookup
  # [Format]
  #   \push_const_fast symbol_index association_index
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * const
  #   * ...
  # [Description]
  #   Locates the constant indicated by the literal +constant+ from the
  #   current context, and pushes it onto the stack. If the constant cannot be
  #   found in the current context, nothing is pushed onto the stack, and a
  #   NameError exception is raised.
  #
  #   Internally, the constant is cached via the internal Association object
  #   in the literals spot specified by the second arg. This allows for future
  #   lookup to not require the full LookupTable resolution.
  # [Example]
  #   <code>
  #     engine = RUBY_ENGINE # RUBY_ENGINE is a constant defined by Rubinius
  #   </code>

  def push_const_fast(symbol_index, association_index)
    <<-CODE
    bool found;
    Object* res = 0;

    Object* val = call_frame->cm->literals()->at(state, association_index);

    // See if the cache is present, if so, validate it and use the value
    GlobalCacheEntry* cache;
    if((cache = try_as<GlobalCacheEntry>(val)) != NULL) {
      if(cache->valid_p(state)) {
        res = cache->value();
      } else {
        Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, symbol_index));
        res = Helpers::const_get(state, call_frame, sym, &found);
        if(found) {
          cache->update(state, res);
        }
      }
    } else {
      Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, symbol_index));
      res = Helpers::const_get(state, call_frame, sym, &found);
      if(found) {
        cache = GlobalCacheEntry::create(state, res);
        call_frame->cm->literals()->put(state, association_index, cache);
      } else {
        Module* under;
        StaticScope* scope = call_frame->static_scope();
        if(scope->nil_p()) {
          under = G(object);
        } else {
          under = scope->module();
        }
        res = Helpers::const_missing(state, under, sym, call_frame);
      }
    }

    HANDLE_EXCEPTION(res);

    if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, call_frame);
      if(cache && res) {
        cache->update(state, res);
      }
    }

    HANDLE_EXCEPTION(res);

    stack_push(res);
    CODE
  end

  def test_push_const_fast
    <<-CODE
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    StaticScope* cs = StaticScope::create(state);
    cs->module(state, child);
    cs->parent(state, ps);

    cm->scope(state, cs);

    Symbol* name = state->symbol("Number");
    parent->set_const(state, name, Fixnum::from(3));

    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(3));
    Object* obj = call_frame->cm->literals()->at(state, 1);
    TS_ASSERT(!obj->nil_p());
    TS_ASSERT(kind_of<GlobalCacheEntry>(obj));
    GlobalCacheEntry* assoc = as<GlobalCacheEntry>(obj);

    TS_ASSERT_EQUALS(assoc->value(), Fixnum::from(3));

    assoc->value(state, Fixnum::from(42));

    run();
    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(42));
    CODE
  end

  # [Operation]
  #   Pushes the Object, the root of constants
  # [Format]
  #   \push_cpath_top
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * cpathtop
  #   * ...
  # [Description]
  #   Pushes the top-level global object that represents the top-level
  #   namespace for constants. Used to find constants relative to the
  #   toplevel.

  def push_cpath_top
    <<-CODE
    stack_push(G(object));
    CODE
  end

  def test_push_cpath_top
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->stack_top(), G(object));
    CODE
  end

  # [Operation]
  #   Pushes the current exception onto the stack
  # [Format]
  #   \push_exception
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * exception
  #   * ...
  # [Description]
  #   Pushes the current exception onto the stack, so that it can be used for
  #   some purpose, such as checking the exception type, setting an exception
  #   variable in a rescue clause, etc.
  # [See Also]
  #   * raise_exc
  # [Example]
  #   <code>
  #     begin
  #       foo = BAR        # BAR is not defined
  #     rescue NameError   # push_exception used to check type of exception (via ===)
  #       puts "No BAR"
  #     end
  #   </code>

  def push_exception
    <<-CODE
    stack_push(state->thread_state()->as_object(state));
    CODE
  end

  def test_push_exception
    <<-CODE
    Exception* exc = Exception::create(state);
    task->exception(state, exc);
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), exc);
    CODE
  end

  # [Operation]
  #   Pushes false onto the stack
  # [Format]
  #   \push_false
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * false
  #   * ...
  # [Description]
  #   The special object false is pushed onto the stack.

  def push_false
    <<-CODE
    stack_push(Qfalse);
    CODE
  end

  def test_push_false
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qfalse);
    CODE
  end

  # [Operation]
  #   Pushes a small integer literal value onto the stack
  # [Format]
  #   \push_int value
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Pushes the value of the integer literal onto the stack.
  # [See Also]
  #   * meta_push_0
  #   * meta_push_1
  #   * meta_push_2
  #   * meta_push_neg_1
  # [Notes]
  #   Certain common cases (i.e. -1, 0, 1, and 2) are optimised to avoid the
  #   decoding of the argument.

  def push_int(val)
    <<-CODE
    stack_push(Fixnum::from(val));
    CODE
  end

  def test_push_int
    <<-CODE
    stream[1] = (opcode)47;
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT(kind_of<Fixnum>(task->stack_top()));
    TS_ASSERT_EQUALS(as<Integer>(task->stack_top())->to_native(), 47);
    CODE
  end

  # [Operation]
  #   Pushes an instance variable onto the stack
  # [Format]
  #   \push_ivar lit
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Pushes the instance variable identified by +lit+ onto the stack.

  def push_ivar(index)
    <<-CODE
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    Object* ret = call_frame->self()->get_ivar(state, sym);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def test_push_ivar
    <<-CODE
    Symbol* name = state->symbol("@blah");
    call_frame->self(state, Qtrue);
    call_frame->self()->set_ivar(state, name, Qtrue);
    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    CODE
  end

  # [Operation]
  #   Pushes a literal from the current state onto the stack.
  # [Format]
  #   \push_literal index
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * literal
  #   * ...
  # [Description]
  #   The literal identified by the opcode argument (+index+) in the current
  #   state literals tuple is retrieved and placed onto the stack.
  #
  #   The literals tuple is part of the machine state, and holds all literal
  #   objects defined or used within a particular scope.

  def push_literal(val)
    <<-CODE
    Object* t2 = call_frame->cm->literals()->at(state, val);
    stack_push(t2);
    CODE
  end

  def test_push_literal
    <<-CODE
    call_frame->cm->literals(state, Tuple::from(state, 1, Qtrue));
    stream[1] = (opcode)0;
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    CODE
  end

  # [Operation]
  #   Pushes the value of a local variable onto the stack
  # [Format]
  #   \push_local local
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * local_value
  #   * ...
  # [Description]
  #   Retrieves the current value (+local_value+) of the referenced local
  #   variable (+local+), and pushes it onto the stack.

  def push_local(index)
    <<-CODE
    stack_push(call_frame->top_scope()->get_local(index));
    CODE
  end

  def test_push_local
    <<-CODE
    task->push(Qtrue);
    stream[1] = (opcode)0;
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 1);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    CODE
  end

  # [Operation]
  #   Pushes the value of a local from an enclosing scope onto the stack
  # [Format]
  #   \push_local_depth depth local
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * localval
  #   * ...
  # [Description]
  #   Retrieves the value of a local variable from a context enclosing the
  #   current context, and pushes it onto the stack.
  # [Example]
  #   <code>
  #     foo.each do |i|
  #       bar.each do |j|
  #         i = i + j  # i is a local variable from enclosing scope at depth 1
  #       end
  #     end
  #   </code>

  def push_local_depth(depth, index)
    <<-CODE
    VariableScope* scope = call_frame->scope;

    for(int j = 0; j < depth; j++) {
      scope = scope->parent();
    }

    stack_push(scope->get_local(index));
    CODE
  end

  def test_push_local_depth
    <<-CODE
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active(), task->active(), 0);
    BlockContext* bc = be->create_context(state, (MethodContext*)Qnil);

    BlockEnvironment* be2 = BlockEnvironment::under_context(state, cm, bc, bc, 1);
    BlockContext* bc2 = be2->create_context(state, (MethodContext*)Qnil);

    task->make_active(bc2);

    bc->set_local(0, Qtrue);

    stream[1] = (opcode)1;
    stream[2] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    CODE
  end

  # [Operation]
  #   Pushes a value from an object field onto the stack
  # [Format]
  #   \push_my_field fld
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Pushes the value of the specified field in the current object onto the
  #   stack.
  # [Notes]
  #   Fields are similar to instance variables, but have dedicated storage
  #   allocated. They are primarily used on core or bootstap classes.
  #   Accessing a field is slightly faster than accessing an ivar.

  def push_my_field(index)
    <<-CODE
    stack_push(call_frame->self()->get_field(state, index));
    CODE
  end

  def push_my_offset(index)
    <<-CODE
    Object* val = *reinterpret_cast<Object**>(
        reinterpret_cast<uintptr_t>(call_frame->self()) + index);
    stack_push(val);
    CODE
  end

  def test_push_my_field
    <<-CODE
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, Qtrue);

    call_frame->self(state, tup);

    stream[1] = (opcode)0;

    TS_ASSERT_THROWS(run(), std::runtime_error);

    Class* cls = state->new_class("Blah");

    call_frame->self(state, cls);

    stream[1] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), state->symbol("Blah"));
    CODE
  end

  # [Operation]
  #   Puts nil on the stack
  # [Format]
  #   \push_nil
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * nil
  #   * ...
  # [Description]
  #   The special object nil is pushed onto the stack.

  def push_nil
    <<-CODE
    stack_push(Qnil);
    CODE
  end

  def test_push_nil
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qnil);
    CODE
  end

  # [Operation]
  #   Pushes the current StaticScope object on the stack
  # [Format]
  #   \push_scope
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * #<StaticScope>
  #   * ...
  # [Description]
  #   Many operations are defered to the current scope. This operation
  #   retrieves the current scope so methods can be called on it.
  #

  def push_scope
    <<-CODE
    stack_push(call_frame->static_scope());
    CODE
  end

  def test_push_scope
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->active()->cm->scope(), task->stack_top());
    CODE
  end

  # [Operation]
  #   Pushes a reference to the current self object onto the stack.
  # [Format]
  #   \push_self
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * self
  #   * ...
  # [Description]
  #   The current self object is pushed onto the stack.

  def push_self
    <<-CODE
    stack_push(call_frame->self());
    CODE
  end

  def test_push_self
    <<-CODE
    call_frame->self(state, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    CODE
  end

  # [Operation]
  #   Pushes true onto the stack
  # [Format]
  #   \push_true
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * true
  #   * ...
  # [Description]
  #   The special value true is pushed onto the stack.

  def push_true
    <<-CODE
    stack_push(Qtrue);
    CODE
  end

  def test_push_true
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    CODE
  end

  # [Operation]
  #   Raises an exception
  # [Format]
  #   \raise_exc
  # [Stack Before]
  #   * exception
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops an exception instance +exception+ off the stack, and uses it to
  #   raise an exception in the machine.

  def raise_exc
    <<-CODE
    Object* t1 = stack_pop();
    state->thread_state()->raise_exception(as<Exception>(t1));
    RUN_EXCEPTION();
    CODE
  end

  def test_raise_exc
    <<-CODE
      Exception* exc = Exception::create(state);
      task->push(exc);

      TS_ASSERT_EQUALS(task->exception(), Qnil);
      run();

      TS_ASSERT_EQUALS(task->exception(), exc);
    CODE
  end

  # [Operation]
  #   Simple return from a method (only)
  # [Format]
  #   \ret
  # [Stack Before]
  #   * retval
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops the top value from the stack, and uses it as the return value from
  #   a method.
  # [See Also]
  #   * caller_return
  #   * raise_exc

  def ret
    <<-CODE
    return stack_top();
    CODE
  end

  def test_ret
    <<-CODE
    task->push(Fixnum::from(100));
    MethodContext *s1 = task->active()->sender();
    run();
    MethodContext *s2 = task->active()->sender();
    TS_ASSERT_EQUALS(Fixnum::from(100), task->pop());
    TS_ASSERT_DIFFERS(s1, s2);
    TS_ASSERT_EQUALS(Qnil, s2);
    CODE
  end

  # [Operation]
  #   Reverses the order on the stack of the top +count+ items
  # [Format]
  # \rotate count
  # [Stack Before]
  # * obj1
  # * obj2
  # * obj3
  # * ...
  # [Stack After]
  # * obj3
  # * obj2
  # * obj1
  # * ...

  def rotate(count)
    <<-CODE
    int i;
    int diff = count / 2;
    Object* tmp;

    for(i = 0; i < diff; i++) {
      int offset = count - i - 1;
      tmp = STACK_PTR[-offset];
      STACK_PTR[-offset] = STACK_PTR[-i];
      STACK_PTR[-i] = tmp;
    }

    CODE
  end

  def test_rotate
    <<-CODE
      task->push(Fixnum::from(0));
      task->push(Fixnum::from(1));
      task->push(Fixnum::from(2));
      stream[1] = (opcode)3;
      TS_ASSERT_EQUALS(task->calculate_sp(), 2);
      run();
      TS_ASSERT_EQUALS(as<Integer>(task->stack_at(0))->to_native(), 2);
      TS_ASSERT_EQUALS(as<Integer>(task->stack_at(1))->to_native(), 1);
      TS_ASSERT_EQUALS(as<Integer>(task->stack_at(2))->to_native(), 0);
      TS_ASSERT_EQUALS(task->calculate_sp(), 2);
    CODE
  end

  # [Operation]
  #   Sends a message with no args to a receiver
  # [Format]
  #   \send_method method_name
  # [Stack Before]
  #   * receiver
  #   * ...
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops an object off the top of the stack (+receiver+), and sends it the no
  #   arg message +method_name+.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [See Also]
  #   * send_with_arg_register
  # [Notes]
  #   This form of send is for methods that take no arguments.
  #

  def send_method(index)
    <<-CODE
    Object* recv = stack_top();
    SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(index);
    Class* const klass = recv->lookup_begin(state);

    Arguments args(recv, Qnil, 0, 0);

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, ALLOW_PRIVATE())) {
        args.unshift(state, cache->name);
      }
    }

    Object* ret = cache->method->execute(state, call_frame, *cache, args);

    SET_ALLOW_PRIVATE(false);
    stack_pop();

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def test_send_method
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(0));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(10));

    Symbol* name = state->symbol("blah");
    G(true_class)->method_table()->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    TypedRoot<SendSite*> tr_ss(state, ss);
    vmm->sendsites = &tr_ss;

    call_frame->cm->literals()->put(state, 0, ss);
    task->push(Qtrue);

    stream[1] = (opcode)0;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 0U);
    TS_ASSERT_EQUALS(call_frame->self(), Qtrue);
    CODE
  end

  # [Operation]
  #   Sends a message with arguments on the stack
  # [Format]
  #   \send_stack method argc
  # [Stack Before]
  #   * argN
  #   * ...
  #   * arg2
  #   * arg1
  #   * receiver
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops the receiver and the block to be passed off the stack, and sends
  #   the message +method+ with +argc+ arguments. The arguments to the method
  #   remain on the stack, ready to be converted to locals when the method is
  #   activated.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [See Also]
  #   * send_stack_with_block
  # [Notes]
  #   This opcode does not pass a block to the receiver; see
  #   send_stack_with_block for the equivalent op code used when a block is to
  #   be passed.

  def send_stack(index, count)
    <<-CODE
    Object* recv = stack_back(count);
    SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(index);
    Class* const klass = recv->lookup_begin(state);

    Arguments args(recv, Qnil, count,
                   stack_back_position(count));

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, ALLOW_PRIVATE())) {
        args.unshift(state, cache->name);
      }
    }

    SET_ALLOW_PRIVATE(false);

    Object* ret = cache->method->execute(state, call_frame, *cache, args);

    stack_clear(count + 1);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def test_send_stack
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(1));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(1));

    Symbol* name = state->symbol("blah");
    G(true_class)->method_table()->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    TypedRoot<SendSite*> tr_ss(state, ss);
    vmm->sendsites = &tr_ss;


    call_frame->cm->literals()->put(state, 0, ss);
    task->push(Qtrue);
    task->push(Fixnum::from(3));

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 1U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(call_frame->self(), Qtrue);
    CODE
  end

  # [Operation]
  #   Sends a message with arguments and a block on the stack
  # [Format]
  #   \send_stack_with_block method argc
  # [Stack Before]
  #   * block
  #   * argN
  #   * ...
  #   * arg2
  #   * arg1
  #   * receiver
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops the receiver +receiver+ and a block off the stack, and sends the
  #   message +method+ with +argc+ arguments. The arguments to the method
  #   remain on the stack, ready to be converted to locals as part of method
  #   activation.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [See Also]
  #   * send_stack
  # [Notes]
  #   This opcode passes a block to the receiver; see send_stack for the
  #   equivalent op code used when no block is to be passed.

  def send_stack_with_block(index, count)
    <<-CODE
    Object* block = stack_pop();
    Object* recv = stack_back(count);
    SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(index);
    Class* const klass = recv->lookup_begin(state);

    Arguments args(recv, block, count,
                   stack_back_position(count));

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, ALLOW_PRIVATE())) {
        args.unshift(state, cache->name);
      }
    }

    SET_ALLOW_PRIVATE(false);

    Object* ret = cache->method->execute(state, call_frame, *cache, args);

    stack_clear(count + 1);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def test_send_stack_with_block
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(1));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(1));

    Symbol* name = state->symbol("blah");
    G(true_class)->method_table()->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);
    TypedRoot<SendSite*> tr_ss(state, ss);
    vmm->sendsites = &tr_ss;

    call_frame->cm->literals()->put(state, 0, ss);
    task->push(Qtrue);
    task->push(Fixnum::from(3));

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active(), task->active(), 0);
    task->push(be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 1U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->active()->block(), be);
    TS_ASSERT_EQUALS(call_frame->self(), Qtrue);
    CODE
  end

  # [Operation]
  #   Sends a message with args to a receiver
  # [Format]
  #   \send_stack_with_splat method direct_args
  # [Stack Before]
  #   * block
  #   * splat
  #   * argN
  #   * ...
  #   * arg2
  #   * arg1
  #   * receiver
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops the receiver +receiver+ and a block +block+ off the top of the stack,
  #   and sends the message +method+. The number of arguments taken by the
  #   method must have previously been set in the args register, and the arg
  #   values themselves remain on the top of the stack, to be converted to
  #   locals as part of method activation.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [See Also]
  #   * send_method
  #   * set_args
  #   * cast_array_for_args
  # [Notes]
  #   The number of arguments to be passed to the method in +args+ must have
  #   been set previously via a call to either set_args or
  #   cast_array_for_args.

  def send_stack_with_splat(index, count)
    <<-CODE
    Object* block = stack_pop();
    Object* ary   = stack_pop();
    Object* recv =  stack_back(count);
    SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(index);
    Class* const klass = recv->lookup_begin(state);

    Arguments args(recv, block, count,
                   stack_back_position(count));

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, ALLOW_PRIVATE())) {
        args.unshift(state, cache->name);
      }
    }

    if(!ary->nil_p()) {
      if(CALL_FLAGS() & #{CALL_FLAG_CONCAT}) {
        args.prepend(state, as<Array>(ary));
      } else {
        args.append(state, as<Array>(ary));
      }
    }

    SET_CALL_FLAGS(0);
    SET_ALLOW_PRIVATE(false);

    Object* ret = cache->method->execute(state, call_frame, *cache, args);

    stack_clear(count + 1);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def test_send_stack_with_splat
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(2));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(2));

    Symbol* name = state->symbol("blah");
    G(true_class)->method_table()->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);
    TypedRoot<SendSite*> tr_ss(state, ss);
    vmm->sendsites = &tr_ss;

    call_frame->cm->literals()->put(state, 0, ss);
    task->push(Qtrue);
    task->push(Fixnum::from(3));

    Array* splat = Array::create(state, 1);
    splat->set(state, 0, Fixnum::from(47));
    task->push(splat);

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active(), task->active(), 0);
    task->push(be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 2U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->stack_at(1), Fixnum::from(47));
    TS_ASSERT_EQUALS(task->active()->block(), be);
    TS_ASSERT_EQUALS(call_frame->self(), Qtrue);
    CODE
  end

  # [Operation]
  #   Call a method on the superclass with a block
  # [Format]
  #   \send_super_stack_with_block method argc
  # [Stack Before]
  #   * block
  #   * argN
  #   * ...
  #   * arg2
  #   * arg1
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops a block off the stack, and sends the message +method+ with +argc+
  #   arguments. The arguments to the method remain on the stack, ready to be
  #   converted into locals when the method is activated.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [Notes]
  #   The receiver is not specified for a call to super; it is the superclass
  #   of the current object that will receive the message.

  def send_super_stack_with_block(index, count)
    <<-CODE
    Object* block = stack_pop();
    SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(index);
    Object* const recv = call_frame->self();
    Class* const klass = recv->lookup_begin(state);

    Arguments args(recv, block, count,
                   stack_back_position(count));

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, true,
                     call_frame->module()->superclass())) {
        args.unshift(state, cache->name);
      }
    }

    SET_ALLOW_PRIVATE(false);

    Object* ret = cache->method->execute(state, call_frame, *cache, args);

    stack_clear(count);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);
    CODE
  end

  def test_send_super_stack_with_block
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(1));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(1));

    Class* parent = state->new_class("Parent", G(object));
    Class* child =  state->new_class("Child", parent);

    Symbol* blah = state->symbol("blah");
    parent->method_table()->store(state, blah, target);
    SendSite* ss = SendSite::create(state, blah);
    TypedRoot<SendSite*> tr_ss(state, ss);
    vmm->sendsites = &tr_ss;

    Object* obj = state->new_object<Object>(child);
    call_frame->self(state, obj);

    StaticScope *sc = StaticScope::create(state);
    sc->module(state, child);

    cm->scope(state, sc);

    task->active()->module(state, child);
    task->active()->name(state, blah);
    task->active()->self(state, call_frame->self());

    call_frame->cm->literals()->put(state, 0, ss);
    task->push(Qnil); // sentinal value, to make sure its not used
    task->push(Fixnum::from(3));

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active(), task->active(), 0);
    task->push(be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 1U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->active()->block(), be);
    TS_ASSERT_EQUALS(call_frame->self(), obj);


    // Now test that send finds a private method

    state->global_cache->clear(child, blah);
    task = Task::create(state);

    ctx = MethodContext::create(state, Qnil, cm);
    task->make_active(ctx);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, target);
    vis->visibility(state, G(sym_private));

    parent->method_table()->store(state, blah, vis);

    call_frame->self(state, obj);
    task->active()->module(state, child);
    task->active()->name(state, blah);
    task->active()->self(state, call_frame->self());

    call_frame->cm->literals()->put(state, 0, ss);
    task->push(Qnil);
    task->push(Fixnum::from(3));

    be = BlockEnvironment::under_context(state, target, task->active(), task->active(), 0);
    task->push(be);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 1U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->active()->block(), be);
    TS_ASSERT_EQUALS(call_frame->self(), obj);
    CODE
  end

  # [Operation]
  #   Call a method on the superclass, passing args plus a block
  # [Format]
  #   \send_super_stack_with_splat method direct_args
  # [Stack Before]
  #   * block
  #   * argN
  #   * ...
  #   * arg2
  #   * arg1
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops a block off the stack, and sends the message +method+ to the current
  #   objects  superclass. The arguments to the method are left on the top of
  #   the stack, ready to be converted into locals when the method is
  #   activated.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [Notes]
  #   The args register must have previously been set to the count of the
  #   number of arguments in +args+ via either set_args or
  #   cast_array_for_args.

  def send_super_stack_with_splat(index, count)
    <<-CODE
    Object* block = stack_pop();
    Object* ary   = stack_pop();
    Object* const recv = call_frame->self();
    SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(index);
    Class* const klass = recv->lookup_begin(state);

    Arguments args(recv, block, count,
                   stack_back_position(count));

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, true,
                        call_frame->module()->superclass())) {
        args.unshift(state, cache->name);
      }
    }

    if(!ary->nil_p()) {
      if(CALL_FLAGS() & #{CALL_FLAG_CONCAT}) {
        args.prepend(state, as<Array>(ary));
      } else {
        args.append(state, as<Array>(ary));
      }
    }

    SET_CALL_FLAGS(0);
    SET_ALLOW_PRIVATE(false);

    Object* ret = cache->method->execute(state, call_frame, *cache, args);

    stack_clear(count);

    HANDLE_EXCEPTION(ret);
    stack_push(ret);

    CODE
  end

  def test_send_super_stack_with_splat
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(2));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(2));

    Class* parent = state->new_class("Parent", G(object));
    Class* child =  state->new_class("Child", parent);

    Symbol* blah = state->symbol("blah");
    parent->method_table()->store(state, blah, target);
    SendSite* ss = SendSite::create(state, blah);
    TypedRoot<SendSite*> tr_ss(state, ss);
    vmm->sendsites = &tr_ss;

    Object* obj = state->new_object<Object>(child);
    call_frame->self(state, obj);

    StaticScope *sc = StaticScope::create(state);
    sc->module(state, child);

    cm->scope(state, sc);

    task->active()->module(state, child);
    task->active()->name(state, blah);
    task->active()->self(state, call_frame->self());

    call_frame->cm->literals()->put(state, 0, ss);
    task->push(obj);
    task->push(Fixnum::from(3));

    Array* splat = Array::create(state, 1);
    splat->set(state, 0, Fixnum::from(47));
    task->push(splat);

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active(), task->active(), 0);
    task->push(be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 2U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->stack_at(1), Fixnum::from(47));
    TS_ASSERT_EQUALS(task->active()->block(), be);
    TS_ASSERT_EQUALS(call_frame->self(), obj);
    CODE
  end

  # [Operation]
  #   Sets call flags prior to a send operation
  # [Format]
  #   \set_call_flags flags
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   The call flags on the current execution context are set to the opcode
  #   argument +flags+.
  # [Notes]
  #   Currently, the only call flag is 1, which instructs the machine to
  #   include private methods when looking for a method that responds to a
  #   message.

  def set_call_flags(flags)
    <<-CODE
    SET_CALL_FLAGS(flags);
    CODE
  end

  def test_set_call_flags
    <<-CODE
    stream[1] = (opcode)1;
    TS_ASSERT_EQUALS(0, task->call_flags);
    run();
    TS_ASSERT_EQUALS(1, task->call_flags);
    CODE
  end

  # [Operation]
  #   Sets a literal to refer to a constant
  # [Format]
  #   \set_const lit
  # [Stack Before]
  #   * item
  #   * ...
  # [Stack After]
  #   * const
  #   * ...
  # [Description]
  #   Pops an object off the stack, and sets the literal referenced in the
  #   opcode to refer to the object as a constant. The constant is pushed back
  #   onto the stack.

  def set_const(index)
    <<-CODE
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    call_frame->static_scope()->module()->set_const(state, sym, stack_top());
    CODE
  end

  def test_set_const
    <<-CODE
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    cm->scope(state, ps);
    Symbol* name = state->symbol("Age");

    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    task->push(Fixnum::from(3));
    run();

    TS_ASSERT_EQUALS(parent->get_const(state, name), Fixnum::from(3));
    CODE
  end

  # [Operation]
  #   Sets the value of a constant under a module
  # [Format]
  #   \set_const_at lit
  # [Stack Before]
  #   * value
  #   * module
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops the new +value+ for a constant identified by the +lit+ index in the
  #   literals tuple, in the scope of +module+, which is also popped from the
  #   stack.

  def set_const_at(index)
    <<-CODE
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    Object* val = stack_pop();
    Module* under = as<Module>(stack_pop());

    under->set_const(state, sym, val);
    stack_push(val);
    CODE
  end

  def test_set_const_at
    <<-CODE
    Symbol* name = state->symbol("Age");
    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    task->push(G(true_class));
    task->push(Qtrue);

    run();

    TS_ASSERT_EQUALS(G(true_class)->get_const(state, name), Qtrue);

    CODE
  end
  # [Operation]
  #   Sets an instance variable on self
  # [Format]
  #   \set_ivar ivar
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Pops a value off the stack, and uses it to set the value of the instance
  #   variable identifies by the literal +ivar+ on the current +self+ object.
  #   The value popped off the stack is then pushed back on again.

  def set_ivar(index)
    <<-CODE
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    call_frame->self()->set_ivar(state, sym, stack_top());
    CODE
  end

  def test_set_ivar
    <<-CODE
    Symbol* name = state->symbol("@blah");
    call_frame->self(state, Qtrue);
    call_frame->cm->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    task->push(Qfalse);
    run();

    TS_ASSERT_EQUALS(Qtrue->get_ivar(state, name), Qfalse);
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qfalse);
    CODE
  end

  # [Operation]
  #   Sets a literal to the specified value
  # [Format]
  #   \set_literal lit
  # [Stack Before]
  #   * regexlit
  #   * ...
  # [Stack After]
  #   * regex
  #   * ...
  # [Description]
  #   Used to set the value of a literal that is a regular expression. The
  #   Regexp object to which the literal is to be set is copied from the top
  #   of the stack, but is not consumed.
  # [Notes]
  #   Unlike other literals such as strings and numbers, creating a Regexp
  #   literal (i.e. via the /regex/ syntax) is a two step process to create
  #   the literal slot for the Regexp, create a literal for the string between
  #   the '/' delimiters and create a new Regexp object passing it the string.
  #   Only then can the literal value be set, using the set_literal opcode.

  def set_literal(val)
    <<-CODE
    call_frame->cm->literals()->put(state, val, stack_top());
    CODE
  end

  def test_set_literal
    <<-CODE
    call_frame->cm->literals(state, Tuple::from(state, 1, Qtrue));
    stream[1] = (opcode)0;
    task->push(Qtrue);
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    TS_ASSERT_EQUALS(call_frame->cm->literals()->at(state, 0), Qtrue);
    CODE
  end

  # [Operation]
  #   Sets the value of a local variable
  # [Format]
  #   \set_local index
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Sets the local variable identified by +index+ to +value+.
  #   The stack is not modified by this instruction.

  def set_local(index)
    <<-CODE
    call_frame->top_scope()->set_local(state, index, stack_top());
    CODE
  end

  def test_set_local
    <<-CODE
    task->push(Qnil);
    task->push(Qtrue);

    stream[1] = (opcode)0;
    run();

    TS_ASSERT_EQUALS(task->stack_at(0), Qtrue);
    TS_ASSERT_EQUALS(task->calculate_sp(), 1);
    CODE
  end

  # [Operation]
  #   Updates the value of a local variable contained in an enclosing scope
  # [Format]
  #   \set_local_depth depth local
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Uses the +value+ on the top of the stack to update the value of the
  #   local variable +local+ in an enclosing scope. The value is then pushed
  #   back onto the stack, to represent the return value from the expression.
  # [Example]
  #   <code>
  #     foo.each do |i|
  #       bar.each do |j|
  #         i = i + j  # i is a local variable from enclosing scope at depth 1
  #       end
  #     end
  #   </code>

  def set_local_depth(depth, index)
    <<-CODE
    VariableScope* scope = call_frame->scope;

    for(int j = 0; j < depth; j++) {
      scope = scope->parent();
    }

    Object* val = stack_pop();
    scope->set_local(state, index, val);
    stack_push(val);
    CODE
  end

  def test_set_local_depth
    <<-CODE
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active(), task->active(), 0);
    BlockContext* bc = be->create_context(state, (MethodContext*)Qnil);

    BlockEnvironment* be2 = BlockEnvironment::under_context(state, cm, bc, bc, 0);
    BlockContext* bc2 = be2->create_context(state, (MethodContext*)Qnil);

    task->make_active(bc2);

    task->push(Qtrue);
    bc->set_local(0, Qnil);

    stream[1] = (opcode)1;
    stream[2] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    TS_ASSERT_EQUALS(bc->get_local(0), Qtrue);
    CODE
  end

  # [Operation]
  #   Shifts the first item in a tuple onto the stack
  # [Format]
  #   \shift_array
  # [Stack Before]
  #   * [value1, value2, ..., valueN]
  #   * ...
  # [Stack After]
  #   * value1
  #   * [value2, ..., valueN]
  #   * ...
  # [Description]
  #   Pops an array off the top of the stack. If the array is empty, it is
  #   pushed back onto the stack, followed by nil.
  #   Otherwise, the array is shifted, then pushed back onto the stack,
  #   followed by the object that was shifted from the front of the array.

  def shift_array
    <<-CODE
    Array* array = as<Array>(stack_pop());
    size_t size = (size_t)array->size();

    if(size == 0) {
      stack_push(array);
      stack_push(Qnil);
    } else {
      size_t j = size - 1;
      Object* shifted_value = array->get(state, 0);
      Array* smaller_array = Array::create(state, j);

      for(size_t i = 0; i < j; i++) {
        smaller_array->set(state, i, array->get(state, i+1));
      }

      stack_push(smaller_array);
      stack_push(shifted_value);
    }
    CODE
  end

  def test_shift_array
    <<-CODE
      Array* empty = Array::create(state, 0);
      Array* array = Array::create(state, 2);
      array->set(state, 0, Fixnum::from(10));
      array->set(state, 1, Fixnum::from(20));

      task->push(empty);

      run();

      TS_ASSERT_EQUALS(Qnil, task->pop());
      TS_ASSERT_EQUALS(empty, task->pop());

      task->push(array);

      run();
      TS_ASSERT_EQUALS(Fixnum::from(10), as<Fixnum>(task->pop()));
      TS_ASSERT_EQUALS(Fixnum::from(20), as<Array>(task->pop())->get(state, 0));
    CODE
  end

  # [Operation]
  #   Store a value into a field of self
  # [Format]
  #   \store_my_field fld
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Stores the value at the top of the stack into the field specified by
  #   +fld+ on +self+.
  #
  #   The stack is left unmodified.

  def store_my_field(index)
    <<-CODE
    call_frame->self()->set_field(state, index, stack_top());
    CODE
  end

  def test_store_my_field
    <<-CODE
    Class* cls = state->new_class("Blah");

    call_frame->self(state, cls);

    Symbol* name = state->symbol("Foo");
    task->push(name);
    stream[1] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), name);
    TS_ASSERT_EQUALS(cls->name(), name);
    CODE
  end

  # [Operation]
  #   Appends two stings together to form a single string
  # [Format]
  #   \string_append
  # [Stack Before]
  #   * string1
  #   * string2
  #   * ...
  # [Stack After]
  #   * string1string2
  #   * ...
  # [Description]
  #   Pops two strings off the stack, appends the second to the first, and
  #   then pushes the combined string back onto the stack.
  # [Notes]
  #   The original string is modified by the append.

  def string_append
    <<-CODE
    String* s1 = as<String>(stack_pop());
    String* s2 = as<String>(stack_pop());
    s1->append(state, s2);
    stack_push(s1);
    CODE
  end

  def test_string_append
    <<-CODE
      String* s1 = String::create(state, "first");
      String* s2 = String::create(state, " second");
      task->push(s2);
      task->push(s1);
      run();
      TS_ASSERT_EQUALS(task->stack_at(0)->class_object(state), G(string));
      TS_ASSERT_SAME_DATA(as<String>(task->pop())->byte_address(), "first second", 13);
    CODE
  end

  # [Operation]
  #   Create a new string with the same contents as the string currently on
  #   the stack
  # [Format]
  #   \string_dup
  # [Stack Before]
  #   * original
  #   * ...
  # [Stack After]
  #   * duplicate
  #   * ...
  # [Description]
  #   Consume the string on the stack, replacing it with a duplicate. Mutating
  #   operations on the original string will not affect the duplicate, and
  #   vice-versa.

  def string_dup
    <<-CODE
    String *s1 = as<String>(stack_pop());
    stack_push(s1->string_dup(state));
    CODE
  end

  def test_string_dup
    <<-CODE
      String* s1 = String::create(state, "something");

      task->push(s1);

      run();

      String* s2 = as<String>(task->pop());

      TS_ASSERT_SAME_DATA(s1, s2, s1->size());
      TS_ASSERT_DIFFERS(s1->id(state), s2->id(state));
    CODE
  end

  # [Operation]
  #   Swap the top two stack values
  # [Format]
  #   \swap_stack
  # [Stack Before]
  #   * value1
  #   * value2
  #   * ...
  # [Stack After]
  #   * value2
  #   * value1
  #   * ...
  # [Description]
  #   Swaps the top two items on the stack, so that the second item becomes
  #   the first, and the first item becomes the second.

  def swap_stack
    <<-CODE
    Object* t1 = stack_pop();
    Object* t2 = stack_pop();
    stack_push(t1);
    stack_push(t2);
    CODE
  end

  def test_swap_stack
    <<-CODE
    task->push(Qtrue);
    task->push(Qfalse);

    run();

    TS_ASSERT_EQUALS(task->stack_at(0), Qfalse);
    TS_ASSERT_EQUALS(task->stack_at(1), Qtrue);

    CODE
  end

  def move_down(positions)
    <<-CODE
    Object* val = stack_top();
    for(int i = 0; i < positions; i++) {
      int target = -i;
      int current = target - 1;
      STACK_PTR[target] = STACK_PTR[current];
    }
    STACK_PTR[-positions] = val;
    CODE
  end

  def test_move_down
    <<-CODE
    call_frame->push(Fixnum::from(0));
    call_frame->push(Fixnum::from(1));
    call_frame->push(Fixnum::from(2));
    call_frame->push(Fixnum::from(3));

    stream[1] = (opcode)3;
    run();

    TS_ASSERT_EQUALS(call_frame->pop(), Fixnum::from(2));
    TS_ASSERT_EQUALS(call_frame->pop(), Fixnum::from(1));
    TS_ASSERT_EQUALS(call_frame->pop(), Fixnum::from(0));
    TS_ASSERT_EQUALS(call_frame->pop(), Fixnum::from(3));
    CODE
  end

  # [Operation]
  #   Placeholder for a removed opcode
  # [Format]
  #   \unused
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #    Raises an assert failure if used.
  # [Notes]
  #   This opcode is used wherever a previously used opcode has been removed.
  #   It ensures that removing an opcode does not change the numbering of
  #   succeeding opcodes.
  #
  #   The same opcode symbol (:unused) and instruction body can be used anywhere
  #   an opcode has been removed.

  def unused
    <<-CODE
    sassert(0 && "deprecated opcode was encountered!");
    CODE
  end

  # [Operation]
  #   Pauses execution and yields to the debugger
  # [Format]
  #   \yield_debugger
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pauses virtual machine execution at the \yield_debugger instruction, and
  #   yields control to the debugger on the debug channel. If no debugger is
  #   registered, an error is raised.
  # [Notes]
  #   The \yield_debugger instruction is VM plumbing that exists to support
  #   "full-speed" debugging. As such, it is not output by the compiler, but
  #   rather is used by the debugger to replace an existing instruction at a
  #   breakpoint. Prior to encountering a \yield_debugger instruction, the VM
  #   will execute normally, i.e. at full speed, and not be slowed
  #   significantly by the fact that a debugger is attached.
  #
  #   When the debugger is yielded to by this instruction, it can examine the
  #   execution context, stack, etc, or replace the \yield_debugger instruction
  #   with the original instruction at that point, and then step through the
  #   code.

  def yield_debugger
    <<-CODE
    Helpers::yield_debugger(state, call_frame);
    CODE
  end


  def setup_unwind(ip, type)
    <<-CODE
    assert(current_unwind < kMaxUnwindInfos);
    UnwindInfo& info = unwinds[current_unwind++];
    info.target_ip = ip;
    info.stack_depth = stack_calculate_sp();
    info.type = (UnwindType)type;
    CODE
  end

  def test_setup_unwind
    <<-CODE
    call_frame->push(Fixnum::from(0));
    call_frame->push(Fixnum::from(0));

    stream[1] = (opcode)15;
    run();

    TS_ASSERT_EQUALS(call_frame->current_unwind, 1);
    TS_ASSERT_EQUALS(call_frame->unwinds[0].target_ip, 15U);
    TS_ASSERT_EQUALS(call_frame->unwinds[0].stack_depth, 1U);
    CODE
  end

  def pop_unwind
    <<-CODE
    assert(current_unwind > 0);
    --current_unwind;
    CODE
  end

  def test_pop_unwind
    <<-CODE
    call_frame->push_unwind(0);

    run();

    TS_ASSERT_EQUALS(call_frame->current_unwind, 0);
    CODE
  end

  def reraise
    <<-CODE
    assert(state->thread_state()->raise_reason() != cNone);
    RUN_EXCEPTION();
    CODE
  end

  def raise_return
    <<-CODE
    if(!(call_frame->flags & CallFrame::cIsLambda) &&
       !call_frame->scope_still_valid(call_frame->scope->parent())) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
    } else {
      state->thread_state()->raise_return(stack_top(), call_frame->top_scope());
    }
    RUN_EXCEPTION();
    CODE
  end

  def ensure_return
    <<-CODE
    state->thread_state()->raise_return(stack_top(), call_frame->scope);
    RUN_EXCEPTION();
    CODE
  end

  def raise_break
    <<-CODE
    state->thread_state()->raise_break(stack_top(), call_frame->scope->parent());
    RUN_EXCEPTION();
    CODE
  end

  def push_variables
    <<-CODE
    call_frame->promote_scope(state);
    stack_push(call_frame->scope);
    CODE
  end

  def check_interrupts
    <<-CODE
    if(unlikely(state->interrupts.timer)) {
      {
        state->interrupts.timer = false;
        state->set_call_frame(call_frame);
        // unlock..
        GlobalLock::UnlockGuard lock(state->global_lock());
        // and relock automatically!
      }
    }
    if(!state->check_async(call_frame)) {
      return NULL;
    }
    CODE
  end

end

# == Manipulation ==
# This section contains code which sets up instructions.rb to be
# manipulated and examined.
#

require File.dirname(__FILE__) + "/codegen/instructions_gen.rb"

if $0 == __FILE__
  si = Instructions.new
  methods = si.decode_methods
  si.inject_superops(methods)

  File.open("vm/gen/iseq_instruction_names.cpp","w") do |f|
    f.puts si.generate_names(methods)
  end

  File.open("vm/gen/iseq_instruction_names.hpp","w") do |f|
    f.puts si.generate_names_header(methods)
  end

  File.open("vm/gen/implementation_prototype.hpp","w") do |f|
    f.puts si.generate_ops_prototypes(methods)
  end

  File.open("vm/gen/iseq_instruction_size.gen", "w") do |f|
    f.puts si.generate_size(methods)
  end

  #File.open("vm/test/test_instructions.hpp", "w") do |f|
  #  si.generate_tests(f)
  #end

end
