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
# the opcode.  This is necessary to test opcodes that take arguments, such
# as open_class.
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
  #   Adds a method to a \class or module
  # [Format]
  #   \add_method name
  # [Stack Before]
  #   * receiver
  #   * method
  #   * ...
  # [Stack After]
  #   * method
  #   * ...
  # [Description]
  #   Hooks up a compiled method to a \class or module.
  #
  #   The \class or module the method is to be added to (+receiver+) and the
  #   compiled method object (+method+) are popped from the stack, while the
  #   name of the method is an argument to the opcode (+name+). On return, the
  #   compiled method is pushed back onto the stack.
  #
  #   NOTE: this instruction is only emitted when the compiler is in kernel mode.
  #   In normal ruby code, the method __add_method__ is sent to self to add a method.
  # [See Also]
  #   * attach_method
  # [Notes]
  #   Singleton methods are handled by attach_method.

  def add_method(index)
    <<-CODE
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    Object* recv = stack_pop();
    Module* mod = try_as<Module>(recv);
    /* If the receiver is not a module, use the receiver's class_object instead */
    if(!mod) {
      mod = as<Module>(recv->class_object(state));
    }
    CompiledMethod* meth = as<CompiledMethod>(stack_pop());

    task->add_method(mod, sym, meth);
    stack_push(meth);
    CODE
  end

  def test_add_method
    <<-CODE
    Symbol* name1 = state->symbol("true_method");
    Symbol* name2 = state->symbol("kernel_method");
    task->literals()->put(state, 0, name1);
    task->literals()->put(state, 1, name2);

    task->push(cm);
    task->push(G(true_class));

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(G(true_class)->method_table()->fetch(state, name1), cm);
    /* Clear the stack for the next scenario */
    task->pop();

    String* recv = String::create(state, "main");

    task->push(cm);
    task->push(recv);
    stream[1] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(cm, G(string)->method_table()->fetch(state, name2));
    TS_ASSERT_EQUALS(Qnil, G(object)->method_table()->fetch(state, name2));
    CODE
  end

  # [Operation]
  #   Set the StaticScope of the current CompiledMethod
  # [Format]
  #   \push_scope
  # [Stack Before]
  #   * #<Module>
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Create a new StaticScope object and set it to the current
  #   CompiledMethod object. The value on the top of the stack is set
  #   to be the module within the StaticScope. This scope is chained off
  #   the current scope of the method.
  #

  def add_scope
    <<-CODE
    Object* obj = stack_pop();
    Module* mod = as<Module>(obj);
    StaticScope* scope = StaticScope::create(state);
    scope->module(state, mod);
    scope->parent(state, task->active()->cm()->scope());
    task->active()->cm()->scope(state, scope);
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

  # [Operation]
  #   Attaches a method definition to an object's singleton \class
  # [Format]
  #   \attach_method name
  # [Stack Before]
  #   * receiver
  #   * method
  #   * ...
  # [Stack After]
  #   * method
  #   * ...
  # [Description]
  #   Hooks up a compiled method to an object instance via it's singleton
  #   \class.
  #
  #   The object the method is to be added to (+receiver+) and the compiled
  #   method object (+method+) are popped from the stack, while the name of
  #   the  method is an argument to the opcode (+name+). On return, the
  #   compiled method is pushed back onto the stack.
  # [See Also]
  #   * add_method
  # [Notes]
  #   Class/module methods are handled by add_method.

  def attach_method(index)
    <<-CODE
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    Object* obj = stack_pop();
    Object* obj2 = stack_pop();
    CompiledMethod* meth = as<CompiledMethod>(obj2);

    task->attach_method(obj, sym, meth);
    stack_push(meth);
    CODE
  end

  def test_attach_method
    <<-CODE
    Symbol* name = state->symbol("blah");
    task->literals()->put(state, 0, name);

    task->push(cm);
    task->push(G(true_class));

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(G(true_class)->metaclass(state)->method_table()->fetch(state, name), cm);
    TS_ASSERT(!cm->scope()->nil_p());
    TS_ASSERT_EQUALS(cm->scope()->module(), G(true_class));
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
      Array* ary = Array::create(state, 1);
      ary->set(state, 0, t1);
      t1 = ary;
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
    Object* obj = stack_top();

    if(!obj->nil_p()) {
      Tuple* tup = as<Tuple>(obj);
      int k = tup->num_fields();
      /* If there is only one thing in the tuple... */
      if(k == 1) {
        Object* t1 = tup->at(state, 0);
        /* and that thing is an array... */
        if(kind_of<Array>(t1)) { // HACK use try_as
          /* make a tuple out of the array contents... */
          Array* ary = as<Array>(t1);
          int j = ary->size();
          Tuple* out = Tuple::create(state, j);

          for(k = 0; k < j; k++) {
            out->put(state, k, ary->get(state, k));
          }

          /* and put it on the top of the stack. */
          stack_set_top(out);
        }
      }
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
    Tuple* tup = as<Tuple>(stack_pop());
    int k = tup->num_fields();
    if(k == 0) {
      stack_push(Qnil);
    } else if(k == 1) {
      stack_push(tup->at(state, 0));
    } else {
      stack_push(Array::from_tuple(state, tup));
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
  #   \check_serial method serial
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
    Object* t1 = stack_pop();
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));

    if(task->check_serial(t1, sym, serial)) {
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
      task->literals()->put(state, 0, sym);

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
  #   Get the \class for the specified object
  # [Format]
  #   \class
  # [Stack Before]
  #   * object
  #   * ...
  # [Stack After]
  #   * class
  #   * ...
  # [Description]
  #   Consume the object reference on the stack, and push a reference to the
  #   parent \class in its place.
  #   TODO - Use this somewhere!

  def class
    <<-CODE
    Object* t1 = stack_pop();
    stack_push(t1->class_object(state));
    CODE
  end

  def test_class
    <<-CODE
      task->push(Qtrue);
      run();
      TS_ASSERT_EQUALS(G(true_class), task->pop());
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
    task->exception(state, (Exception*)Qnil);
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
    Object* _lit = task->literals()->at(state, index);
    CompiledMethod* cm = as<CompiledMethod>(_lit);

    MethodContext* parent;
    if(kind_of<BlockContext>(task->active())) {
      parent = as<BlockContext>(task->active())->env()->home();
    } else {
      parent = task->active();
    }

    parent->reference(state);
    task->active()->reference(state);

    cm->scope(state, task->active()->cm()->scope());

    Object* t2 = BlockEnvironment::under_context(state, cm, parent, task->active(), index);
    stack_push(t2);
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
  #   Perform a raw comparison of two object references
  # [Format]
  #   \equal
  # [Stack Before]
  #   * value1
  #   * value2
  #   * ...
  # [Stack After]
  #   * result
  #   * ...
  # [Description]
  #   Performs a comparison of two objects, resulting in either +true+ or
  #   +false+ being pushed onto the stack as a result. The comparison is done
  #   without any method calls.
  #
  #   For two Fixnums, two Symbols, or two literals (+true+, +false+, +nil+),
  #   return +true+ if the values are identical.
  #
  #   For two object references (including Bignum), return +true+ if value1
  #   and value2 point to the same instance.

  def equal
    <<-CODE
    Object* t1 = stack_pop();
    Object* t2 = stack_pop();
    stack_push(t1 == t2 ? Qtrue : Qfalse);
    CODE
  end

  def test_equal
    <<-CODE
    String* s1 = String::create(state, "test_equal");
    task->push(s1);
    task->push(s1);
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());

    String* s2 = as<String>(s1->dup(state));
    task->push(s2);
    task->push(s1);
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());

    task->push(Fixnum::from(123));
    task->push(Fixnum::from(123));
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());
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
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    Object* res = task->const_get(under, sym, &found);
    if(!found) {
      Message& msg = *task->msg;
      msg.recv = under;
      msg.name = state->symbol("const_missing");
      msg.block = Qnil;
      msg.stack = 0;
      msg.lookup_from = msg.recv->lookup_begin(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, sym);
      msg.set_arguments(state, args);

      bool res = task->send_message_slowly(msg);
      RETURN(res);
    } else if(kind_of<Autoload>(res)) {
      Message& msg = *task->msg;
      msg.recv = res;
      msg.name = G(sym_call);
      msg.stack = 0;
      msg.lookup_from = res->lookup_begin(state);
      msg.set_args(0);

      bool res = task->send_message_slowly(msg);
      RETURN(res);
    }

    stack_push(res);
    RETURN(false);
    CODE
  end

  def test_find_const
    <<-CODE
    Symbol* name = state->symbol("Number");
    G(true_class)->set_const(state, name, Fixnum::from(3));

    task->literals()->put(state, 0, name);
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
    task->set_ip(location);
    cache_ip();
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
      task->set_ip(location);
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
      task->set_ip(location);
      cache_ip();
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
      task->set_ip(location);
      cache_ip();
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
  #   Halts the current task
  # [Format]
  #   \halt
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Causes the current Task to halt. No further execution will be performed
  #   on the current Task. This instruction is only used inside the trampoline
  #   method used as the first MethodContext of a Task.

  def halt
    <<-CODE
    throw Task::Halt("Task halted");
    CODE
  end

  def test_halt
    <<-CODE
    try {
      run();
      TS_FAIL("halt did not throw an error during run()");
    } catch(Task::Halt& e) {
      const char* msg = e.what();
      TS_ASSERT_SAME_DATA("Task halted", msg, 11);
    }
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
    Class* parent = state->new_class("Parent", G(object), 1);
    Class* child =  state->new_class("Child", parent, 1);

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
  #   Return true if value is a Fixnum, otherwise false
  # [Format]
  #   \is_fixnum
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * result
  #   * ...
  # [Description]
  #   Consume the value on the stack, and put the special values true or false
  #   depending on whether the consumed value was of Fixnum type

  def is_fixnum
    <<-CODE
    Object* t1 = stack_pop();
    stack_push(t1->fixnum_p() ? Qtrue : Qfalse);
    CODE
  end

  def test_is_fixnum
    <<-CODE
    task->push(Integer::from(state, 10));
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());
    task->push(String::create(state, "no"));
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
  #   Return true if value is a Symbol, otherwise false
  # [Format]
  #   \is_symbol
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * result
  #   * ...
  # [Description]
  #   Consume the value on the stack, and put the special values true or false
  #   depending on whether the consumed value was of Symbol type

  def is_symbol
    <<-CODE
    Object* t1 = stack_pop();
    stack_push(t1->symbol_p() ? Qtrue : Qfalse);
    CODE
  end

  def test_is_symbol
    <<-CODE
    task->push(Qnil);
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());
    task->push(String::create(state, "no"));
    run();
    TS_ASSERT_EQUALS(Qfalse, task->pop());
    task->push(state->symbol("foo"));
    run();
    TS_ASSERT_EQUALS(Qtrue, task->pop());
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
    Class* parent = state->new_class("Parent", G(object), 1);
    Class* child =  state->new_class("Child", parent, 1);

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
  #   Locates a method by searching the \class hierarchy from a specified
  #   object
  # [Format]
  #   \locate_method
  # [Stack Before]
  #   * include_private
  #   * method_name
  #   * self
  #   * ...
  # [Stack After]
  #   * method
  #   * ...
  # [Description]
  #   Pops a flag indicating whether or not to search in private methods, the
  #   method name, and the object to search from off the stack. If a matching
  #   method is located, the module it was found in and the method is pushed
  #   onto the stack; otherwise, nil is pushed onto the stack.

  def locate_method
    <<-CODE
    Object* t1 = stack_pop(); // include_private
    Symbol* name = as<Symbol>(stack_pop()); // meth
    Object* t3 = stack_pop(); // self
    stack_push(task->locate_method_on(t3, name, t1));
    CODE
  end

  def test_locate_method
    <<-CODE
    Symbol* name = state->symbol("blah");
    G(true_class)->method_table()->store(state, name, cm);

    task->push(Qtrue);
    task->push(name);
    task->push(Qfalse);

    run();

    TS_ASSERT_EQUALS(G(true_class), as<Tuple>(task->stack_top())->at(state, 1));
    TS_ASSERT_EQUALS(cm, as<Tuple>(task->stack_top())->at(state, 0));
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

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      env->call(state, task, count);
      RETURN(true);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_call), count));
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

    task->literals()->put(state, 0, block_method);

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
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_equal), 1));
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
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_gt), 1));
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
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_lt), 1));
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
      stack_pop();
      Object* res = ((Fixnum*)(left))->sub(state, (Fixnum*)(right));
      stack_push(res);
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_minus), 1));
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
  #   Implementation of != optimised for fixnums and symbols
  # [Format]
  #   \meta_send_op_nequal
  # [Stack Before]
  #   * value2
  #   * value1
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Pops +value1+ and +value2+ off the stack, and pushes the logical result
  #   of !(+value1+ == +value2+). If +value1+ and +value2+ are both fixnums or
  #   both symbols, the comparison is done directly; otherwise, the != method
  #   is called on +value1+, passing +value2+ as the argument.
  # [Notes]
  #   Is this correct? Shouldn't the non-optimised case call ==, and negate
  #   the result?

  def meta_send_op_nequal
    <<-CODE
    Object* t1 = stack_back(1);
    Object* t2 = stack_back(0);
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qfalse : Qtrue);
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_nequal), 1));
    CODE
  end

  def test_meta_send_op_nequal
    <<-CODE
    Object* one = Fixnum::from(1);
    Object* two = Fixnum::from(2);

    task->push(two);
    task->push(one);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);

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
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_plus), 1));
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
      RETURN(false);
    }

    RETURN(send_slowly(vmm, task, ctx, G(sym_tequal), 1));
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
  #   Creates or re-opens a \class.
  # [Format]
  #   \open_class class_name
  # [Stack Before]
  #   * super
  #   * ...
  # [Stack After]
  #   * class
  #   * ...
  # [Description]
  #   Creates or re-opens a \class, taking the superclass (or nil) from the
  #   stack. Upon return, the new \class is pushed onto the stack.
  #
  #   The +class_name+ argument to the opcode is the \class literal
  #   identifying the \class to be opened.
  # [See Also]
  #   * push_encloser
  #   * open_class_under
  # [Notes]
  #   The enclosing \class (if any) will be the current enclosing \class in
  #   the current execution context. Typically, this will have been set by
  #   executing the opcode push_encloser.
  # [Example]
  #   <code>
  #     class A
  #       class B
  #       end
  #     end
  #     # Stack transition:
  #     # [...,A] => [...,B]
  #   </code>

  def open_class(index)
    <<-CODE
    bool created;
    Object* super = stack_pop();
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));

    Class* cls = task->open_class(super, sym, &created);

    stack_push(cls);
    CODE
  end

  def test_open_class
    <<-CODE
    Symbol* name = state->symbol("C");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, G(true_class));
    ps->parent(state, (StaticScope*)Qnil);
    cm->scope(state, ps);

    task->push(G(true_class));

    task->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Class>(G(true_class)->get_const(state, name)));

    Class* cls = (Class*)task->stack_top();
    TS_ASSERT_EQUALS(cls->metaclass(state)->superclass(), G(true_class)->metaclass(state))
    CODE
  end

  # [Operation]
  #   Creates or re-opens a \class.
  # [Format]
  #   \open_class_under \class
  # [Stack Before]
  #   * super
  #   * enclosing_class
  #   * ...
  # [Stack After]
  #   * class
  #   * ...
  # [Description]
  #   Creates or re-opens a \class, popping the superclass (or nil) and the
  #   enclosing \class from the stack. Upon return, the new \class is pushed
  #   onto the stack.
  #
  #   The +\class+ argument to the opcode is index of a literal symbol that
  #   indicates the name of the class to be opened. 
  # [See Also]
  #   * open_class
  # [Example]
  #   <code>
  #     class A
  #     end
  #
  #     class A::B < C
  #     end
  #     # Stack transition:
  #     # [...,A,C] => [...,B]
  #   </code>

  def open_class_under(index)
    <<-CODE
    bool created;
    Object* super = stack_pop();
    Module* under = as<Module>(stack_pop());
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));

    Class* cls = task->open_class(under, super, sym, &created);
    // TODO use created? it's only for running the opened_class hook, which
    // we're eliminating anyway.

    stack_push(cls);
    CODE
  end

  def test_open_class_under
    <<-CODE
    Symbol* name = state->symbol("C");
    task->push(G(true_class));
    task->push(Qnil);

    task->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Class>(G(true_class)->get_const(state, name)));
    CODE
  end

  # [Operation]
  #   Returns the metaclass for an object
  # [Format]
  #   \open_metaclass
  # [Stack Before]
  #   * obj
  #   * ...
  # [Stack After]
  #   * metaclass
  #   * ...
  # [Description]
  #   Pops an object off the stack, obtains it's metaclass (creating it if
  #   necessary), and pushes it onto the stack.
  # [Notes]
  #   An object metaclass is it's singleton class, i.e. the class that is used
  #   to hold object specific methods. In Rubinius, any object can call the
  #   +metaclass+ method to obtain its metaclass.
  # [Example]
  #   <code>
  #     o.metclass   # returns o's metaclass
  #   </code>

  def open_metaclass
    <<-CODE
    Object* t1 = stack_pop();
    stack_push(t1->metaclass(state));
    CODE
  end

  def test_open_metaclass
    <<-CODE
    Tuple* tup = Tuple::create(state, 1);
    task->push(tup);

    run();

    TS_ASSERT_EQUALS(task->stack_top(), tup->metaclass(state));
    CODE
  end
  # [Operation]
  #   Creates or reopens a module
  # [Format]
  #   \open_module module
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * module
  #   * ...
  # [Description]
  #   Creates or re-opens the module referenced by the literal +module+, and
  #   pushes it onto the stack.
  # [See Also]
  #   * open_class
  # [Example]
  #   <code>
  #     module A   # Creates module A (or re-opens it if it already exists)
  #     end
  #   </code>

  def open_module(index)
    <<-CODE
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));

    stack_push(task->open_module(sym));
    CODE
  end

  def test_open_module
    <<-CODE
    Symbol* name = state->symbol("C");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, G(true_class));
    ps->parent(state, (StaticScope*)Qnil);
    cm->scope(state, ps);

    task->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Module>(G(true_class)->get_const(state, name)));
    CODE
  end

  # [Operation]
  #   Creates or opens a module nested under another module
  # [Format]
  #   \open_module_under name
  # [Stack Before]
  #   * parent
  #   * ...
  # [Stack After]
  #   * module
  #   * ...
  # [Description]
  #   Pops an enclosing \class or module from the stack, and creates or
  #   re-opens the module identified by the literal +name+. The module is then
  #   pushed onto the  stack.
  # [Example]
  #   <code>
  #     module X::Y    # Opens module Y under the module X
  #     end
  #   </code>

  def open_module_under(index)
    <<-CODE
    Module* mod = as<Module>(stack_pop());
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));

    stack_push(task->open_module(mod, sym));
    CODE
  end

  def test_open_module_under
    <<-CODE
    Symbol* name = state->symbol("C");
    task->push(G(true_class));

    task->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Module>(G(true_class)->get_const(state, name)));
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
    if((unsigned long int)index < task->active()->args) {
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
    if((unsigned int)count == task->blockargs) {
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
        task->exception(state, (Exception*)Qnil);
      } else {
        task->exception(state, as<Exception>(top));
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
    // HACK test this for yield in block to outer block
    stack_push(task->home()->block());
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
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    Object* res = task->const_get(sym, &found);
    if(!found) {
      Message& msg = *task->msg;
      StaticScope* scope = task->active()->cm()->scope();
      if(scope->nil_p()) {
        msg.recv = G(object);
      } else {
        msg.recv = scope->module();
      }
      msg.name = state->symbol("const_missing");
      msg.block = Qnil;
      msg.stack = 0;
      msg.lookup_from = msg.recv->lookup_begin(state);
      Array* args = Array::create(state, 1);
      args->set(state, 0, sym);
      msg.set_arguments(state, args);

      bool res = task->send_message_slowly(msg);
      RETURN(res);
    } else if(kind_of<Autoload>(res)) {
      Message& msg = *task->msg;
      msg.recv = res;
      msg.name = G(sym_call);
      msg.stack = 0;
      msg.lookup_from = res->lookup_begin(state);
      msg.set_args(0);

      bool res = task->send_message_slowly(msg);
      RETURN(res);
    }

    stack_push(res);
    RETURN(false);
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

    task->literals()->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->stack_top(), Fixnum::from(3));

    CODE
  end

  # [Operation]
  #   Pushes the current method context onto the stack
  # [Format]
  #   \push_context
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * methodctxt
  #   * ...
  # [Description]
  #   Creates a reference to the current method execution context, and pushes
  #   it onto the stack.

  def push_context
    <<-CODE
    ctx->reference(state);
    stack_push(ctx);
    CODE
  end

  def test_push_context
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), task->active());
    CODE
  end

  # [Operation]
  #   Pushes the top level global object onto the stack
  # [Format]
  #   \push_cpath_top
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * cpathtop
  #   * ...
  # [Description]
  #   Pushes the top-level global object that represents the top-level
  #   namespace for constants. Used when a global variable is referenced.
  # [Example]
  #   <code>
  #     puts $: # Global variables are looked up on the top-level Globals object
  #   </code>

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
    stack_push(task->exception());
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
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    stack_push(task->self()->get_ivar(state, sym));
    CODE
  end

  def test_push_ivar
    <<-CODE
    Symbol* name = state->symbol("@blah");
    task->self(state, Qtrue);
    task->self()->set_ivar(state, name, Qtrue);
    task->literals()->put(state, 0, name);
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
    Object* t2 = task->literals()->at(state, val);
    stack_push(t2);
    CODE
  end

  def test_push_literal
    <<-CODE
    task->literals(state, Tuple::from(state, 1, Qtrue));
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
    stack_push(task->home()->get_local(index)); // HACK test vs. ->home-less
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
    BlockContext* bc = as<BlockContext>(task->active());
    BlockEnvironment* env;

    for(int j = 0; j < depth; j++) {
      env = bc->env();
      bc = as<BlockContext>(env->home_block());
    }

    stack_push(bc->get_local(index));
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
    stack_push(task->self()->get_field(state, index));
    CODE
  end

  def test_push_my_field
    <<-CODE
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, Qtrue);

    task->self(state, tup);

    stream[1] = (opcode)0;

    TS_ASSERT_THROWS(run(), std::runtime_error);

    Class* cls = state->new_class("Blah");

    task->self(state, cls);

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
    stack_push(task->active()->cm()->scope());
    CODE
  end

  def test_push_scope
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->active()->cm()->scope(), task->stack_top());
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
    stack_push(task->self());
    CODE
  end

  def test_push_self
    <<-CODE
    task->self(state, Qtrue);
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
    task->raise_exception(as<Exception>(t1));
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
    Object* value = stack_top();
    MethodContext* active_context = ctx;
    MethodContext* dest = active_context->sender();

    NativeMethodContext* nmc = try_as<NativeMethodContext>(dest);

    if(unlikely(nmc)) {
      task->active(state, nmc);

      nmc->value_returned_to_c(value);
      nmc->action(NativeMethodContext::RETURNED_BACK_TO_C);

      NativeMethod::activate_from(nmc);
    }
    else {
      // restore_sender();
      // === manual inline ===
      // == restore_sender ==

      // if(unlikely(profiler)) profiler->leave_method();
      /* Try to recycle this context to be used again. */
      if(active_context->young_object_p()) {
        state->om->deallocate_context(active_context);
      }

      // == restore_context ==
      task->active(state, dest);

      // === end manual inline ===
      dest->push(value);
    }
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
    Object** objs = new Object*[count];
    int i;

    for(i = 0; i < count; i++) {
      objs[i] = stack_pop();
    }

    for(i = 0; i < count; i++) {
      stack_push(objs[i]);
    }

    delete[] objs;
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
    Message& msg = *task->msg;

    msg.setup(
      vmm->sendsites[index].get(),
      stack_top(),
      ctx,
      0,
      1);

    msg.block = Qnil;
    msg.splat = Qnil;

    msg.priv = task->call_flags & #{CALL_FLAG_PRIVATE};
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name();

    task->call_flags = 0;

    RETURN(msg.send_site->performer(state, task, msg));
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
    ctx->vmm->sendsites = &tr_ss;

    task->literals()->put(state, 0, ss);
    task->push(Qtrue);

    stream[1] = (opcode)0;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 0U);
    TS_ASSERT_EQUALS(task->self(), Qtrue);
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
    Message& msg = *task->msg;

    msg.setup(
      vmm->sendsites[index].get(),
      stack_back(count),
      ctx,
      count,
      count + 1);

    msg.block = Qnil;
    msg.splat = Qnil;

    msg.priv = task->call_flags & #{CALL_FLAG_PRIVATE};
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name();

    task->call_flags = 0;

    RETURN(msg.send_site->performer(state, task, msg));
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
    ctx->vmm->sendsites = &tr_ss;


    task->literals()->put(state, 0, ss);
    task->push(Qtrue);
    task->push(Fixnum::from(3));

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 1U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->self(), Qtrue);
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
    Message& msg = *task->msg;

    msg.block = stack_pop();

    msg.setup(
      vmm->sendsites[index].get(),
      stack_back(count),
      ctx,
      count,
      count + 1);

    msg.splat = Qnil;

    msg.priv = task->call_flags & #{CALL_FLAG_PRIVATE};
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name();

    task->call_flags = 0;

    RETURN(msg.send_site->performer(state, task, msg));
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
    ctx->vmm->sendsites = &tr_ss;

    task->literals()->put(state, 0, ss);
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
    TS_ASSERT_EQUALS(task->self(), Qtrue);
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
    Message& msg = *task->msg;

    msg.block = stack_pop();
    Object* ary = stack_pop();

    msg.setup(
      vmm->sendsites[index].get(),
      stack_back(count), /* receiver */
      ctx,
      count,
      count + 1);

    msg.splat = Qnil;

    if(!ary->nil_p()) {
      if(task->call_flags & #{CALL_FLAG_CONCAT}) {
        msg.append_arguments(state, as<Array>(ary));
      } else {
        msg.append_splat(state, as<Array>(ary));
      }
    }

    msg.priv = task->call_flags & #{CALL_FLAG_PRIVATE};
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name();

    task->call_flags = 0;

    RETURN(msg.send_site->performer(state, task, msg));
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
    ctx->vmm->sendsites = &tr_ss;

    task->literals()->put(state, 0, ss);
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
    TS_ASSERT_EQUALS(task->self(), Qtrue);
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
    Message& msg = *task->msg;

    msg.block = stack_pop();

    msg.setup(
      vmm->sendsites[index].get(),
      task->self(),
      ctx,
      count,
      count);

    msg.splat = Qnil;

    msg.priv = TRUE;
    msg.lookup_from = task->current_module()->superclass();
    msg.name = msg.send_site->name();

    task->call_flags = 0;

    RETURN(msg.send_site->performer(state, task, msg));
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

    Class* parent = state->new_class("Parent", G(object), 1);
    Class* child =  state->new_class("Child", parent, 1);

    Symbol* blah = state->symbol("blah");
    parent->method_table()->store(state, blah, target);
    SendSite* ss = SendSite::create(state, blah);
    TypedRoot<SendSite*> tr_ss(state, ss);
    ctx->vmm->sendsites = &tr_ss;

    Object* obj = state->new_object<Object>(child);
    task->self(state, obj);

    StaticScope *sc = StaticScope::create(state);
    sc->module(state, child);

    cm->scope(state, sc);

    task->active()->module(state, child);
    task->active()->name(state, blah);
    task->active()->self(state, task->self());

    task->literals()->put(state, 0, ss);
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
    TS_ASSERT_EQUALS(task->self(), obj);


    // Now test that send finds a private method

    state->global_cache->clear(child, blah);
    task = Task::create(state);

    ctx = MethodContext::create(state, Qnil, cm);
    task->make_active(ctx);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, target);
    vis->visibility(state, G(sym_private));

    parent->method_table()->store(state, blah, vis);

    task->self(state, obj);
    task->active()->module(state, child);
    task->active()->name(state, blah);
    task->active()->self(state, task->self());

    task->literals()->put(state, 0, ss);
    task->push(Qnil);
    task->push(Fixnum::from(3));

    be = BlockEnvironment::under_context(state, target, task->active(), task->active(), 0);
    task->push(be);

    run();

    TS_ASSERT_EQUALS(task->active()->cm(), target);
    TS_ASSERT_EQUALS(task->active()->args, 1U);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->active()->block(), be);
    TS_ASSERT_EQUALS(task->self(), obj);
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
    Message& msg = *task->msg;

    msg.block = stack_pop();
    Object* ary = stack_pop();

    msg.setup(
      vmm->sendsites[index].get(),
      task->self(),
      ctx,
      count,
      count);

    msg.splat = Qnil;

    if(!ary->nil_p()) {
      if(task->call_flags & #{CALL_FLAG_CONCAT}) {
        msg.append_arguments(state, as<Array>(ary));
      } else {
        msg.append_splat(state, as<Array>(ary));
      }
    }

    msg.priv = TRUE;  // TODO: how do we test this?
    msg.lookup_from = task->current_module()->superclass();
    msg.name = msg.send_site->name();

    task->call_flags = 0;

    RETURN(msg.send_site->performer(state, task, msg));
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

    Class* parent = state->new_class("Parent", G(object), 1);
    Class* child =  state->new_class("Child", parent, 1);

    Symbol* blah = state->symbol("blah");
    parent->method_table()->store(state, blah, target);
    SendSite* ss = SendSite::create(state, blah);
    TypedRoot<SendSite*> tr_ss(state, ss);
    ctx->vmm->sendsites = &tr_ss;

    Object* obj = state->new_object<Object>(child);
    task->self(state, obj);

    StaticScope *sc = StaticScope::create(state);
    sc->module(state, child);

    cm->scope(state, sc);

    task->active()->module(state, child);
    task->active()->name(state, blah);
    task->active()->self(state, task->self());

    task->literals()->put(state, 0, ss);
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
    TS_ASSERT_EQUALS(task->self(), obj);
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
    task->call_flags = flags;
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
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    task->const_set(sym, stack_top());
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

    task->literals()->put(state, 0, name);
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
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    Object* val = stack_pop();
    Module* under = as<Module>(stack_pop());

    task->const_set(under, sym, val);
    stack_push(val);
    CODE
  end

  def test_set_const_at
    <<-CODE
    Symbol* name = state->symbol("Age");
    task->literals()->put(state, 0, name);
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
    Symbol* sym = as<Symbol>(task->literals()->at(state, index));
    task->self()->set_ivar(state, sym, stack_top());
    CODE
  end

  def test_set_ivar
    <<-CODE
    Symbol* name = state->symbol("@blah");
    task->self(state, Qtrue);
    task->literals()->put(state, 0, name);
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
    task->literals()->put(state, val, stack_top());
    CODE
  end

  def test_set_literal
    <<-CODE
    task->literals(state, Tuple::from(state, 1, Qtrue));
    stream[1] = (opcode)0;
    task->push(Qtrue);
    run();
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    TS_ASSERT_EQUALS(task->literals()->at(state, 0), Qtrue);
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
    task->home()->set_local(index, stack_top()); // HACK test vs. ->home-less
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
    BlockContext* bc = as<BlockContext>(task->active());
    BlockEnvironment* env;

    for(int j = 0; j < depth; j++) {
      env = bc->env();
      bc = as<BlockContext>(env->home_block());
    }

    Object* t3 = stack_pop();
    bc->set_local(index, t3);
    stack_push(t3);
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
    task->self()->set_field(state, index, stack_top());
    CODE
  end

  def test_store_my_field
    <<-CODE
    Class* cls = state->new_class("Blah");

    task->self(state, cls);

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
    task->yield_debugger();
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

  File.open("vm/gen/iseq_instruction_names.cpp","w") do |f|
    f.puts si.generate_names
  end

  File.open("vm/gen/iseq_instruction_names.hpp","w") do |f|
    f.puts si.generate_names_header
  end

  File.open("vm/gen/implementation_prototype.hpp","w") do |f|
    f.puts si.generate_ops_prototypes
  end

  File.open("vm/gen/iseq_instruction_size.gen", "w") do |f|
    f.puts si.generate_size
  end

  File.open("vm/test/test_instructions.hpp", "w") do |f|
    si.generate_tests(f)
  end

end
