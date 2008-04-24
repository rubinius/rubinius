require "#{File.dirname(__FILE__)}/../kernel/core/iseq"
require 'rubygems'
require 'parse_tree'

class Instructions

  def generate_switch(fd, op="op")
    pt = ParseTree.new(true)

    fd.puts "switch(#{op}) {"
    InstructionSet::OpCodes.each do |ins|
      code = send(ins.opcode) rescue nil
      if code
        sexp = pt.parse_tree_for_method(Instructions, ins.opcode).flatten
        line = sexp[sexp.index(:newline) + 1] + 1

        fd.puts "   case #{ins.bytecode}: { /* #{ins.opcode} */"
        fd.puts "#line #{line} \"instructions.rb\""
        fd.puts code
        if ins.check_interrupts?
          fd.puts "   goto check_interrupts;"
        elsif ins.terminator?
          fd.puts "   goto insn_start;"
        else
          fd.puts "   goto next_op;"
        end
        fd.puts "   }"
      else
        STDERR.puts "Problem with opcode: #{ins.opcode}"
      end
    end
    fd.puts "default: printf(\"Invalid bytecode: %d\\n\", (int)op); sassert(0);\n"
    fd.puts "}"
    fd.puts
  end

  def generate_tests(fd)
    pt = ParseTree.new(true)

    fd.puts <<-CODE
#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestInstructions : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }
    CODE

    InstructionSet::OpCodes.each do |ins|
      meth = "test_#{ins.opcode}".to_sym
      code = send(meth) rescue nil
      if code
        sexp = pt.parse_tree_for_method(Instructions, meth).flatten
        line = sexp[sexp.index(:newline) + 1] + 1

        fd.puts "void #{meth}() {"
        fd.puts "Task* task = Task::create(state);"
        fd.puts "Tuple* stack;"
        fd.puts "CompiledMethod* cm = CompiledMethod::create(state);"
        fd.puts "cm->iseq = ISeq::create(state, 10);"
        fd.puts "task->active = task->generate_context(Qnil, cm);"
        fd.puts "stack = task->stack = task->active->stack = Tuple::create(state, 10);"
        fd.puts "opcode stream[100];"
        fd.puts "stream[0] = ISeq::insn_#{ins.opcode};"
        fd.puts "#define run(val) task->execute_stream(stream)"
        fd.puts "#line #{line} \"instructions.rb\""
        fd.puts code
        fd.puts "#undef run"
        fd.puts "}"
      end
    end
    fd.puts "  };"
  end

  def generate_threaded(fd, op="op")
    InstructionSet::OpCodes.each do |ins|
      code = send(ins.opcode) rescue nil
      if code
        fd.puts "   insn_#{ins.bytecode}: {"
        fd.puts code
        if ins.check_interrupts?
          fd.puts "   goto check_interrupts;"
        elsif ins.terminator?
          fd.puts "   goto insn_start;"
        else
          fd.puts "   NEXT_OP;"
        end
        fd.puts "   }"

      else
        STDERR.puts "Problem with opcode: #{ins.opcode}"
      end
    end
    fd.puts
  end

  def generate_dter
    code = ""
    code << "static int _ip_size(uint32_t bc) {\nswitch(bc) {\n"
    InstructionSet::OpCodes.each do |ins|
      if ins.arg_count == 2
        code << "  case #{ins.bytecode}:\n"
      end
    end
    code << "    return 3;\n"

    InstructionSet::OpCodes.each do |ins|
      if ins.arg_count == 1
        code << "  case #{ins.bytecode}:\n"
      end
    end
    code << "   return 2;\n"

    code << "}\nreturn 1;\n}\n\n"

    code << "#define DT_ADDRESSES static void* _dt_addresses[#{InstructionSet::OpCodes.size + 1}]; static int _dt_size = #{InstructionSet::OpCodes.size};\n"
    code << "#define SETUP_DT_ADDRESSES "

    InstructionSet::OpCodes.each do |ins|
      code << "_dt_addresses[#{ins.bytecode}] = &&insn_#{ins.bytecode}; "
    end
    code << "\n"

    code << <<-CODE

    static inline uint32_t read_int_from_be(uint8_t *str) {
      return (uint32_t)((str[0] << 24)
                      | (str[1] << 16)
                      | (str[2] << 8 )
                      |  str[3]      );
    }

    static void calculate_into_gotos(STATE, OBJECT iseq, OBJECT output, void* addrs[], int size) {
      uint8_t *insn;
      uint32_t op;
      uintptr_t *compiled;

      int i, f, offset, sz;

      f = bytearray_bytes(state, iseq);
      insn = (uint8_t*)bytearray_byte_address(state, iseq);
      compiled = (uintptr_t*)bytearray_byte_address(state, output);

      offset = 0;
      for(offset = 0; offset < f; offset += 4) {
#if CONFIG_BIG_ENDIAN
        op = *((uint32_t*)(insn + offset));
#else
        op = read_int_from_be(insn + offset);
#endif
        /* Protect against errant data */
        if(op > size) op = 0;
        *compiled++ = (uintptr_t)addrs[op];
        /* copy the instruction args */
        sz = _ip_size(op) - 1;
        for(i = 0; i < sz; i++) {
          offset += 4;
#if CONFIG_BIG_ENDIAN
          op = *((uint32_t*)(insn + offset));
#else
          op = read_int_from_be(insn + offset);
#endif
          *compiled++ = (uintptr_t)op;
        }
      }
    }

    CODE

    code
  end

  def generate_declarations(fd)
    fd.puts "int _int;"
    fd.puts "native_int j, k;"
    fd.puts "OBJECT _lit, t1, t2, t3;"
    fd.puts "Message msg(state);"
  end

  def generate_names
    str =  "const char *rubinius::ISeq::get_instruction_name(int op) {\n"
    str << "static const char instruction_names[] = {\n"
    InstructionSet::OpCodes.each do |ins|
      str << "  \"#{ins.opcode.to_s}\\0\"\n"
    end
    str << "};\n\n"
    offset = 0
    str << "static const unsigned int instruction_name_offsets[] = {\n"
    InstructionSet::OpCodes.each_with_index do |ins, index|
      str << ",\n" if index > 0
      str << "  #{offset}"
      offset += ins.opcode.to_s.length + 1
    end
    str << "\n};\n\n"
    str << <<CODE
  return instruction_names + instruction_name_offsets[op];
}
CODE
  end

  def generate_names_header
    str = "const char *get_instruction_name(int op);\n"

    str << "typedef enum {\n"
    InstructionSet::OpCodes.each do |ins|
      str << "insn_#{ins.opcode.to_s} = #{ins.bytecode},\n"
    end
    str << "} instruction_names;\n"

    str
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
    ""
  end

  def test_noop
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    CODE
  end

  # [Operation]
  #   Pushes an integer literal value onto the stack
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
  #   lookup of the literal value in the literals tuple.

  def push_int
    <<-CODE
    next_int;
    stack_push(Object::i2n(_int));
    CODE
  end

  def test_push_int
    <<-CODE
    stream[1] = (opcode)47;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(stack->at(task->sp)->n2i(), 47);
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
    stack_push(Object::i2n(-1));
    CODE
  end

  def test_meta_push_neg_1
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(stack->at(task->sp)->n2i(), -1);
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
    stack_push(Object::i2n(0));
    CODE
  end

  def test_meta_push_0
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(stack->at(task->sp)->n2i(), 0);
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
    stack_push(Object::i2n(1));
    CODE
  end

  def test_meta_push_1
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(stack->at(task->sp)->n2i(), 1);
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
    stack_push(Object::i2n(2));
    CODE
  end

  def test_meta_push_2
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(stack->at(task->sp)->n2i(), 2);
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
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qnil);
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
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
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
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qfalse);
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
    active->reference(state);
    stack_push(active);
    CODE
  end

  def test_push_context
    <<-CODE
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), task->active);
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

  def push_literal
    <<-CODE
    next_int;
    check_bounds(literals, _int);
    t2 = literals->field[_int];
    stack_push(t2);
    CODE
  end

  def test_push_literal
    <<-CODE
    task->literals = Tuple::from(state, 1, Qtrue);
    stream[1] = (opcode)0;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
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

  def set_literal
    <<-CODE
    next_int;
    check_bounds(literals, _int);
    SET(literals, field[_int], stack_top());
    CODE
  end

  def test_set_literal
    <<-CODE
    task->literals = Tuple::from(state, 1, Qtrue);
    stream[1] = (opcode)0;
    stack->put(state, ++task->sp, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
    TS_ASSERT_EQUALS(task->literals->at(0), Qtrue);
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
    stack_push(self);
    CODE
  end

  def test_push_self
    <<-CODE
    task->self = Qtrue;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
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

  def push_local
    <<-CODE
    next_int;
    check_bounds(locals, _int);
    stack_push(fast_fetch(locals, _int));
    CODE
  end

  def test_push_local
    <<-CODE
    stack->put(state, ++task->sp, Qtrue);
    stream[1] = (opcode)0;
    run();
    TS_ASSERT_EQUALS(task->sp, 1);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
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

  def push_local_depth
    <<-CODE
    next_int;
    k = (native_int)_int;
    next_int;
    BlockContext* bc = as<BlockContext>(active);
    BlockEnvironment* env;

    for(j = 0; j < k; j++) {
      env = bc->env();
      bc = as<BlockContext>(env->home_block);
    }

    stack_push(locals->at(_int));
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
    stack_push(exception);
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
    exception = (Exception*)Qnil;
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
    stack_push(current_block);
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

  def push_ivar
    <<-CODE
    next_literal;
    stack_push(self->get_ivar(state, _lit));
    CODE
  end

  # [Operation]
  #   Allocate space for a new object
  # [Format]
  #   \allocate
  # [Stack Before]
  #   * count
  #   * ...
  # [Stack After]
  #   * object
  #   * ...
  # [Description]
  #   DEPRECATED
  #   Pop a field +count+ and \allocate space for an object with the
  #   appropriate header and field space. The object created will be
  #   uninitialized; it will have its parent \class and all fields initialized
  #   to +nil+.

  def allocate
    <<-CODE
    sassert(0);
    CODE
  end

  # [Operation]
  #   Sets a literal to reference the specified \class
  # [Format]
  #   \set_class
  # [Stack Before]
  #   * literal
  #   * class
  #   * ...
  # [Stack After]
  #   * literal
  #   * ...
  # [Description]
  #   DEPRECATED
  #   Pops a literal reference and a \class off the top of the stack, sets the
  #   \class of the literal to the new \class, and then pushes it back onto
  #   the stack.

  def set_class
    <<-CODE
    sassert(0);
    CODE
  end

  # [Operation]
  #   Store a value into the specified object field
  # [Format]
  #   \store_field
  # [Stack Before]
  #   * field
  #   * value
  #   * object
  #   * ...
  # [Stack After]
  #   * object
  #   * ...
  # [Description]
  #   Overwrite the value of a particular field slot with the specified
  #   object. The object reference is left on the stack.

  def store_field
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    size_t idx = (size_t)t1->n2i();
    check_bounds(t3, idx);
    SET(t3, field[idx], t2);
    stack_push(t3);
    CODE
  end

  # [Operation]
  #   Retrieve the value within the field of the specified object
  # [Format]
  #   \fetch_field
  # [Stack Before]
  #   * field
  #   * object
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Retrieve the object of the specified object field number.

  def fetch_field
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(t2->at(t1->n2i()));
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

  def push_my_field
    <<-CODE
    next_int;
    stack_push(self->at(_int));
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

  def store_my_field
    <<-CODE
    next_int;
    check_bounds(self, _int);
    SET(self, field[_int], stack_top());
    CODE
  end

  # [Operation]
  #   Calls a primitive method
  # [Format]
  #   \send_primitive primitive_index argc
  # [Stack Before]
  #   * receiver
  #   * arg1
  #   * arg2
  #   * ...
  #   * argN
  #   * ...
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Execute a primitive method on the receiver (+receiver+). The index of the
  #   primitive to execute (+primitive_index+), and the number of arguments
  #   being passed (+argc+) are  specified as paramters.
  #
  #   When the primitive returns, the return value will be on top of the
  #   stack.
  # [Notes]
  #   The implementation of the primitive is responsible for consuming the
  #   receiver and any arguments that are on the stack, and for placing a
  #   return value onto the stack.

  def send_primitive
    <<-CODE
    sassert(0);
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

  def goto
    <<-CODE
    next_int;
    ip = _int;
    cache_ip();
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

  def goto_if_false
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(!RTEST(t1)) {
      ip = _int;
      cache_ip();
    }
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

  def goto_if_true
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(RTEST(t1)) {
      ip = _int;
      cache_ip();
    }
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

  def goto_if_defined
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(t1 != Qundef) {
      ip = _int;
      cache_ip();
    }
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
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(t1);
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
    t1 = stack_top();
    stack_push(t1);
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

  # [Operation]
  #   Sets the value of a local variable
  # [Format]
  #   \set_local local
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * value
  #   * ...
  # [Description]
  #   Pops +value+ off the stack, and uses it to set the value of the local
  #   variable identified by the literal +local+. The value is then pushed back
  #   onto the stack, to represent the return value from the expression.

  def set_local
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(locals->zone == 0) {
      check_bounds(locals, _int);
      /* Manipulating directly because contains a write barrier variant */
      locals->field[_int] = t1;
    } else {
      locals->put(state, _int, t1);
    }
    stack_push(t1);
    CODE
  end

  # [Operation]
  #   Sets a method argument local from the caller's stack
  # [Format]
  #   \set_local_from_fp local arg
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Copies the value of method argument +arg+ into the local variable
  #   +local+. Method arguments are accessed from the caller's stack via
  #   negative offsets from the frame pointer, where 0 is the first argument,
  #   1 is the second, etc.
  # 
  #   This opcode is now deprecated, and is not currently used under
  #   compiler2.
  # [See Also]
  #   * from_fp
  # [Notes]
  #   Under compiler1, this opcode was used to copy method args into stack
  #   locals if the method arg was modified, but not captured. The use of
  #   stack and frame locals has been deprecated under compiler2, and all
  #   locals are now allocated in the execution context locals tuple. This
  #   opcode has thus been modified to store a frame referenced method arg in
  #   the locals tuple.

  def set_local_from_fp
    <<-CODE
    next_int;
    k = (native_int)_int;
    next_int;

    t1 = stack_top[fp - _int];

    t2 = locals;
    if(t2->gc_zone == 0) {
      sassert(k < NUM_FIELDS(t2) && "locals tuple sized wrong");
      fast_unsafe_set(t2, k, t1);
    } else {
      tuple_put(state, t2, k, t1);
    }
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

  def set_local_depth
    <<-CODE
    next_int;
    k = (native_int)_int;
    next_int;
    BlockContext* bc = as<BlockContext>(active);
    BlockEnvironment* env;

    for(j = 0; j < k; j++) {
      env = bc->env();
      bc = as<BlockContext>(env->home_block);
    }

    t3 = stack_pop();
    Tuple *tup = locals;

    if(tup->zone == 0) {
      check_bounds(tup, _int);
      tup->field[_int] = t3;
    } else {
      tup->put(state, _int, t3);
    }
    stack_push(t3);
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

  def make_array
    <<-CODE
    next_int;
    Array* ary = Array::create(state, _int);
    j = _int - 1;
    for(; j >= 0; j--) {
      t2 = stack_pop();
      ary->set(state, j, t2);
    }

    stack_push(ary);
    CODE
  end

  # [Operation]
  #   Unpack the array on the top of the stack into individual items on the
  #   stack
  # [Format]
  #   \push_array
  # [Stack Before]
  #   * [value1, value2, ..., valueN]
  #   * ...
  # [Stack After]
  #   * value1
  #   * value2
  #   * ...
  #   * valueN
  #   * ...
  # [Description]
  #   Removes the array currently on top of the stack, and replaces it with
  #   the contents of the array, such that the first item in the array ends up
  #   on top of the stack.
  # [See Also]
  #   * make_array
  # [Notes]
  #   Push array places the contents of the array onto the stack in reverse
  #   order to the order in which items on the stack are added to an array by
  #   make_array.

  def push_array
    <<-CODE
    sassert(0);
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
    t1 = stack_pop();
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

  # [Operation]
  #   Casts the value on the top of the stack into an array to be used as
  #   method args
  # [Format]
  #   \cast_array_for_args size
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * array
  #   * ...
  # [Description]
  #   Removes the object on the top of the stack, and:
  #
  #   If the object is a tuple, it is cast to an array.
  #
  #   If the object is already an array, it is left unmodified.
  #
  #   If the object is any other type, it is wrapped in a 1-element array with
  #   the object as the value of the single element.
  #
  #   The resulting array is then pushed back onto the stack, and the size of
  #   the array is added to the argument +size+ that was specified with the
  #   opcode, and saved as the current method argument count.
  #
  #   This opcode will always be followed by a push_array instruction that
  #   unpacks the array elements into discrete stack entries prior to calling
  #   either send_with_arg_register or send_super_with_arg_register.
  # [See Also]
  #   * push_array
  #   * get_args
  # [Notes]
  #   This opcode exists to handle argscat and argspush, where a splat arg is
  #   specified in addition to one or more discrete arguments.

  def cast_array_for_args
    <<-CODE
    sassert(00;
    CODE
  end

  # [Operation]
  #   Convert stack object to a tuple
  # [Format]
  #   \cast_tuple
  # [Stack Before]
  #   * value
  #   * ...
  # [Stack After]
  #   * tuple
  #   * ...
  # [Description]
  #   If stack object is an array, create a new tuple from the array data
  #
  #   If the stack value is a tuple, leave the stack unmodified
  #
  #   Otherwise, create a unary tuple from the value on the stack

  def cast_tuple
    <<-CODE
    t1 = stack_pop();
    if(kind_of<Array>(t1)) {
      Array* ary = as<Array>(t1);
      j = ary->size();

      Tuple* tup = Tuple::create(state, j);

      for(k = 0; k < j; k++) {
        tup->put(state, k, ary->get(state, k));
      }
      t1 = ary;
    } else if(!kind_of<Tuple>(t1)) {
      Tuple* tup = Tuple::create(state, 1);
      tup->put(state, 0, t1);
      t1 = tup;
    }
    stack_push(t1);
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
    k = NUM_FIELDS(tup);
    if(k == 0) {
      stack_push(Qnil);
    } else if(k == 1) {
      stack_push(tup->at(0));
    } else {
      stack_push(Array::from_tuple(state, tup));
    }
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
    Tuple* tup = as<Tuple>(stack_top());
    k = NUM_FIELDS(t1);
    /* If there is only one thing in the tuple... */
    if(k == 1) {
      t1 = tup->at(0);
      /* and that thing is an array... */
      if(kind_of<Array>(t1)) {
        /* make a tuple out of the array contents... */
        Array* ary = as<Array>(t1);
        j = ary->size();
        Tuple* out = Tuple::create(state, j);

        for(k = 0; k < j; k++) {
          out->put(state, k, ary->get(state, k));
        }

        /* and put it on the top o the stack. */
        stack_set_top(out);
      }
    }
    CODE
  end

  # [Operation]
  #   Create a hash and populate with items on the stack
  # [Format]
  #   \make_hash argc
  # [Stack Before]
  #   * keyN
  #   * valueN
  #   * ...
  #   * key2
  #   * value2
  #   * key1
  #   * value1
  #   * ...
  # [Stack After]
  #   * { key1 => value1, key2 => value2, ..., keyN => valueN }
  #   * ...
  # [Description]
  #   Creates a new hash, populating its contents with the number of items
  #   (+argc+) specified in the opcode. The contents of the new hash are taken
  #   from the stack, with the top item on the stack becoming the last key in
  #   the hash, the second top item becoming the last value, and so on. The
  #   resulting hash is added back to the stack.

  def make_hash
    <<-CODE
    next_int;
    Hash* hsh = Hash::create(state);
    j = _int - 1;
    while(j >= 0) {
      t2 = stack_pop();
      t3 = stack_pop();
      hsh->set(state, t2, t3);
      j -= 2;
    }

    stack_push(t1);
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

  def set_ivar
    <<-CODE
    next_literal;
    t2 = stack_pop();
    self->set_ivar(state, _lit, t2);
    stack_push(t2);
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

  def push_const
    <<-CODE
    next_literal;
    bool found;
    t1 = const_get(as<Symbol>(_lit), &found);
    if(found) {
      stack_push(t1);
    } else {
      /* TODO const_missing call */
    }
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

  def find_const
    <<-CODE
    t1 = stack_pop();
    next_literal;
    bool found;
    t2 = const_get(as<Module>(t1), as<Symbol>(_lit), &found);
    if(found) {
      stack_push(t2);
    } else {
      /* TODO call const_missing */
    }
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

  def set_const
    <<-CODE
    next_literal;
    t1 = stack_top();
    const_set(as<Symbol>(_lit), t1);
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

  def set_const_at
    <<-CODE
    next_literal;
    t2 = stack_pop();
    t3 = stack_pop();

    const_set(as<Module>(t3), as<Symbol>(_lit), t2);

    stack_push(t2);
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

  # [Operation]
  #   Sets the enclosing class/module when opening a class/module
  # [Format]
  #   \set_encloser
  # [Stack Before]
  #   * class
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops +\class+ off the stack, and uses it as the enclosing scope for
  #   subsequent \class or module definitions.
  # [See Also]
  #   * push_encloser
  # [Example]
  #   <code>
  #     module A    # encloser is set to A after this line
  #       module B  # encloser is set to B after this line
  #       end       # encloser is reset to A after this line
  #     end
  #   </code>

  def set_encloser
    <<-CODE
    sassert(0);
    CODE
  end

  # [Operation]
  #   Resets the current class/module to the parent class/module
  # [Format]
  #   \push_encloser
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Resets the current enclosing \class or module to the parent \class or
  #   module in the enclosing scope path. Used when a \class or module
  #   definition ends, to \pop up a level in the enclosure stack.
  # [See Also]
  #   * set_encloser
  # [Example]
  #   <code>
  #     module A    # encloser is set to A after this line
  #       module B  # encloser is set to B after this line
  #       end       # encloser is reset to A after this line
  #     end
  #   </code>

  def push_encloser
    <<-CODE
    sassert(0);
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
  #   The +\class+ argument to the opcode is the \class literal identifying
  #   the \class to be opened.
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

  def open_class_under
    <<-CODE
    bool created;

    t1 = stack_pop();
    t2 = stack_pop();

    next_literal;
    t3 = open_class(as<Module>(t2), t1, as<Symbol>(_lit), &created);
    if(t3 != Qundef) {
      stack_push(t3);
      if(created) perform_hook(t3, G(sym_opened_class), t1);
    }
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

  def open_class
    <<-CODE
    bool created;

    t1 = stack_pop();
    next_literal;
    t3 = open_class(t1, as<Symbol>(_lit), &created);
    if(t3 != Qundef) {
      stack_push(t3);
      if(created) perform_hook(t3, G(sym_opened_class), t1);
    }
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

  def open_module_under
    <<-CODE
    next_literal;
    t1 = stack_pop();
    stack_push(open_module(as<Module>(t1), as<Symbol>(_lit)));
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

  def open_module
    <<-CODE
    next_literal;
    stack_push(open_module(as<Symbol>(_lit)));
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
    t1 = stack_pop();
    stack_push(t1->metaclass(state));
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

  def attach_method
    <<-CODE
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    attach_method(t1, as<Symbol>(_lit), as<CompiledMethod>(t2));
    stack_push(t2);
    perform_hook(t1, G(sym_s_method_added), _lit);
    CODE
  end

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
  # [See Also]
  #   * attach_method
  # [Notes]
  #   Singleton methods are handled by attach_method.

  def add_method
    <<-CODE
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    add_method(as<Module>(t1), as<Symbol>(_lit), as<CompiledMethod>(t2));
    stack_push(t2);
    perform_hook(t1, G(sym_method_added), _lit);
    CODE
  end

  # [Operation]
  #   Activates a cached method on an object in preparation for execution.
  # [Format]
  #   \activate_method argc
  # [Stack Before]
  #   * receiver
  #   * method
  #   * size
  #   * locals
  #   * block
  #   * arg1
  #   * arg2
  #   * ...
  #   * argN
  #   * ...
  # [Stack After]
  #   * retval
  #   * ...
  # [Description]
  #   Pops the top five items off the stack, using these to identify the
  #   object on which to invoke the method (+receiver+), the CompiledMethod
  #   instance to execute (+method+, i.e. +self+), the number of argments the
  #   method defines (+size+), the +locals+ currently in scope, and any
  #   +block+ passed to the method.
  #
  #   Additionally, the actual arguments to be passed to the method must also
  #   be on the stack as an array, with the number of actual arguments passed
  #   (+argc+) specified alongside the opcode; these arguments will be
  #   consumed from the stack and pushed into locals when the method starts
  #   executing.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [Notes]
  #   Used by CompiledMethod#activate.
  #   TODO Remove infavor of a primitive.

  def activate_method
    <<-CODE
    next_int;
    msg.recv   = stack_pop();
    CompiledMethod* meth = as<CompiledMethod>(stack_pop());
    msg.method = meth;
    msg.module = as<Module>(stack_pop());
    msg.args   = stack_pop()->n2i();
    msg.name   = meth->name;
    t3 = stack_pop(); /* locals */
    msg.block  = stack_pop();

    activate_method(msg);
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

  def send_method
    <<-CODE
    next_literal;
    msg.send_site = as<SendSite>(_lit);
    msg.recv = stack_top();
    msg.block = Qnil;
    msg.splat = Qnil;
    msg.args = 0;
    msg.stack = 1;
    goto perform_send;
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

  def send_stack
    <<-CODE
    next_literal;
    msg.send_site = as<SendSite>(_lit);
    next_int_into(msg.args);
    msg.recv = stack_back(msg.args);
    msg.splat = Qnil;
    msg.block = Qnil;

    msg.stack = msg.args + 1;

    goto perform_send;
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

  def send_stack_with_block
    <<-CODE
    next_literal;
    msg.send_site = as<SendSite>(_lit);
    next_int_into(msg.args);
    msg.block = stack_pop();
    msg.splat = Qnil;
    msg.recv = stack_back(msg.args);
    msg.stack = msg.args + 2;

    goto perform_send;
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

  def send_stack_with_splat
    <<-CODE
    next_literal;
    msg.send_site = as<SendSite>(_lit);
    next_int_into(msg.args);

    msg.block = stack_pop();
    t1 = stack_pop();

    msg.combine_with_splat(state, this, as<Array>(t1)); /* call_flags & 3 */

    msg.recv = stack_pop();

    msg.stack = msg.args + 3;

    perform_send:

    msg.priv = call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);

    call_flags = 0;

    send_message(msg);
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

  def send_super_stack_with_block
    <<-CODE
    next_literal;
    msg.send_site = as<SendSite>(_lit);
    next_int_into(msg.args);
    msg.block = stack_pop();
    msg.splat = Qnil;
    msg.recv = stack_back(msg.args);
    msg.stack = msg.args + 2;

    goto perform_super_send;
    CODE
  end

  # [Operation]
  #   Call a method on the superclass, passing args plus a block
  # [Format]
  #   \send_super_with_splat method direct_args
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
  #   objects  superclass. The arguments to the method areleft on the top of
  #   the stack, ready to be converted into locals when the method is
  #   activated.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [Notes]
  #   The args register must have previously been set to the count of the
  #   number of arguments in +args+ via either set_args or
  #   cast_array_for_args.

  def send_super_with_splat
    <<-CODE
    next_literal;
    msg.send_site = as<SendSite>(_lit);
    next_int_into(msg.args);
    msg.block = stack_pop();
    t1 = stack_pop();

    msg.combine_with_splat(state, this, as<Array>(t1)); /* call_flags & 3 */

    msg.recv = stack_pop();
    msg.stack = msg.args + 3;

    perform_super_send:

    msg.priv = TRUE;
    call_flags = 0;

    msg.lookup_from = current_module()->superclass;

    send_message(msg);
    CODE
  end

  # [Operation]
  #   Calls the method specified on the top of the stack on an object
  # [Format]
  #   \send_off_stack
  # [Stack Before]
  #   * method
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
  #   Pops the name of the method +method+ to call off the stack, and if
  #   necessary, converts it to a symbol. Then pops +block+ and the receiver
  #   +receiver+ off the stack. Any arguments required by the method must have
  #   been placed on the stack previously, and the number of arguments set in
  #   the args register. The arguments will be popped from the stack and
  #   converted to locals when the method is activated.
  #
  #   When the method returns, the return value will be on top of the stack.
  # [See Also]
  #   * set_args
  #   * cast_array_for_args
  # [Notes]
  #   The number of arguments to be passed to the method in +args+ must have
  #   been set previously via a call to either set_args or
  #   cast_array_for_args.

  def send_off_stack
    <<-CODE
    sassert(0 && "deprecated");

    t3 = stack_pop();
    if(!t3->symbol_p()) {
      if(RISA(t3, string)) {
        t3 = string_to_sym(state, t3);
      } else {
        t2 = stack_pop();
        t1 = stack_pop();
        stack_push(t3);
        _lit = G(sym_send);
        j = args;
        goto perform_send;
      }
    }
    t2 = stack_pop();
    t1 = stack_pop();
    _lit = t3;
    j = args;
    goto perform_send;
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
  #   method is located, it is pushed onto the stack; otherwise, nil is pushed
  #   onto the stack.

  def locate_method
    <<-CODE
    t1 = stack_pop(); // include_private
    SYMBOL name = as<Symbol>(stack_pop()); // meth
    t3 = stack_pop(); // self
    stack_push(locate_method_on(t3, name, t1));
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    if(t1->fixnum_p() && t2->fixnum_p()) {
      stack_pop();
      stack_set_top(as<Fixnum>(t1)->add(state, as<Fixnum>(t2)));
    } else {
      _lit = G(sym_plus);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    if(t1->fixnum_p() && t2->fixnum_p()) {
      stack_pop();
      stack_set_top(as<Fixnum>(t1)->sub(state, as<Fixnum>(t2)));
    } else {
      _lit = G(sym_minus);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qtrue : Qfalse);
    } else {
      _lit = G(sym_equal);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qfalse : Qtrue);
    } else {
      _lit = G(sym_nequal);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    /* If both are fixnums, or both are symbols, compare the ops directly. */
    if((t1->fixnum_p() && t2->fixnum_p()) || (t1->symbol_p() && t2->symbol_p())) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qtrue : Qfalse);
    } else {
      _lit = G(sym_tequal);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    if(t1->fixnum_p() && t2->fixnum_p()) {
      j = t1->n2i();
      k = t2->n2i();
      stack_pop();
      stack_set_top((j < k) ? Qtrue : Qfalse);
    } else {
      _lit = G(sym_lt);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
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
    t1 = stack_back(1);
    t2 = stack_back(0);
    if(t1->fixnum_p() && t2->fixnum_p()) {
      j = t1->n2i();
      k = t2->n2i();
      stack_pop();
      stack_set_top((j > k) ? Qtrue : Qfalse);
    } else {
      _lit = G(sym_gt);
      t2 = Qnil;
      j = 1;
      goto perform_no_ss_send;
    }
    CODE
  end

  def meta_send_call
    <<-CODE
    next_int;
    t1 = stack_back(_int);

    if(kind_of<BlockEnvironment>(t1)) {
      stack_pop();
      as<BlockEnvironment>(t1)->call(state, _int);
    } else {
      _lit = G(sym_call);
      t2 = Qnil;
      j = _int;

perform_no_ss_send:
      msg.recv = t1;
      msg.import_arguments(state, this, j);
      msg.name = as<Symbol>(_lit);
      msg.block = t2;
      stack_pop(); /* remove receiver */
      send_message_slowly(msg);
    }
    CODE
  end

  # [Operation]
  #   Soft return from a block
  # [Format]
  #   \soft_return
  # [Stack Before]
  #   * retval
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops the return value from the stack, and returns to the calling method
  #   or block. The return value is pushed onto the stack of the caller during
  #   the return.
  # [See Also]
  #   * sret
  #   * ret
  #   * caller_return
  # [Notes]
  #   Unlike ret, this return opcode does not consider non-local returns. It
  #   simply returns to the calling block or method context. Thus, it is used
  #   when, for example, breaking from a loop, or upon the normal completion
  #   of a block.

  def soft_return
    <<-CODE
    t1 = stack_pop();
    simple_return(state, t1);
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
    t1 = stack_pop();
    raise_exception(as<Exception>(t1));
    CODE
  end

  # [Operation]
  #   Simple return from a method (only)
  # [Format]
  #   \sret
  # [Stack Before]
  #   * retval
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops the top value from the stack, and uses it as the return value from
  #   a method.
  # [See Also]
  #   * ret
  #   * caller_return
  #   * soft_return
  #   * raise_exc
  # [Notes]
  #   \sret is an optimised version of the more general ret. It works only
  #   with method (MethodContext) returns, but as a result, can skip the
  #   extra work to figure out how to long return from a block.

  def ret
    <<-CODE
    simple_return(stack_top());
    CODE
  end

  # [Operation]
  #   Shifts the first item in a tuple onto the stack
  # [Format]
  #   \shift_tuple
  # [Stack Before]
  #   * [value1, value2, ..., valueN]
  #   * ...
  # [Stack After]
  #   * value1
  #   * [value2, ..., valueN]
  #   * ...
  # [Description]
  #   Pops a tuple off the top of the stack. If the tuple is empty, the tuple
  #   is pushed back onto the stack, followed by nil. Otherwise, the tuple is
  #   shifted, with the tuple then pushed back onto the stack, followed by the
  #   item that was previously at the head of the tuple.
  # [Notes]
  #   This opcode is poorly named; it actually performs a shift, rather than
  #   an unshift.

  def shift_tuple
    <<-CODE
    t1 = stack_pop();
    sassert(t1->reference_p());
    if(NUM_FIELDS(t1) == 0) {
      stack_push(t1);
      stack_push(Qnil);
    } else {
      j = NUM_FIELDS(t1) - 1;
      t3 = t1->at(0);
      Tuple* tup = Tuple::create(state, j);
      tup->copy_from(state, as<Tuple>(t1), 1, j);
      stack_push(t2);
      stack_push(t3);
    }
    CODE
  end

  # [Operation]
  #   Converts multiple individual caller arguments into an array for a rest
  #   argument
  # [Format]
  #   \make_rest argc
  # [Stack Before]
  #   * rest1
  #   * rest2
  #   * ...
  #   * restn
  #   * ...
  # [Stack After]
  #   * [rest1, rest2, ..., restN]
  #   * ...
  # [Description]
  #   Pops 0 or more rest arguments from the top of the stack into an array to
  #   be used as a rest argument to a method whose last (non-block) parameter
  #   is a splat.
  #
  #   The number of arguments to place into the rest array is determined by
  #   how many actual args have been passed to the method, less the number of
  #   arguments +argc+ defined by the method. If this is less than or \equal to
  #   0, an empty array is pushed onto the stack. If the number is greater
  #   than 0, arguments are popped from the stack so that the first rest arg
  #   is at the start of the array, and the last rest arg is at the end.
  # [Notes]
  #   This opcode is performed after the non-splat arguments have been dealt
  #   with, so that the top of the stack contains the first rest argument
  #   (remembering that arguments are pushed onto the stack in right to left
  #   order).
  # [Example]
  #   <code>
  #     def foo(a,b,*c)
  #       ...
  #     end
  #
  #     foo(1,2,3,4,5)
  #
  #     # In foo, c = [3,4,5]
  #     # On the top of the stack after this opcode is [3,4,5]
  #   </code>

  def make_rest
    <<-CODE
    sassert(0);
    next_int;
    j = argcount - _int;
    if(j < 0) j = 0;
    t1 = array_new(state, j);
    for(k = 0; k < j; k++) {
      array_set(state, t1, k, stack_pop());
    }
    stack_push(t1);
    CODE
  end

  # [Operation]
  #   Converts multiple individual caller arguments into an array for a rest
  #   argument
  # [Format]
  #   \make_rest_fp argc
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * [rest1, rest2, ..., restN]
  #   * ...
  # [Description]
  #   Copies multiple individual arguments to a method from the caller's stack
  #   into an array to be used as a rest argument. Used when the method to be
  #   called has a rest argument (i.e. the last defined method parameter is a
  #   splat).
  #
  #   The number of arguments to place into the rest array is determined by
  #   how many actual args have been passed to the method, less the number of
  #   arguments +argc+ defined by the method. If this is less than or \equal to
  #   0, an empty array is pushed onto the stack. If the number is greater
  #   than 0, arguments are copied from before the frame pointer into the
  #   array, so that the first rest arg is at the start of the array, and the
  #   last rest arg is at the end.
  # [Example]
  #   <code>
  #     def foo(a,b,*c)
  #       ...
  #     end
  #     
  #     foo(1,2,3,4,5)
  #     
  #     # In foo, c = [3,4,5]
  #     # On the top of the stack after this opcode is [3,4,5]
  #   </code>

  def make_rest_fp
    <<-CODE
    sassert(0);
    next_int;
    j = argcount - _int;
    if(j < 0) j = 0;
    t1 = array_new(state, j);
    for(k = _int, m = 0; k < argcount; k++, m++) {
      array_set(state, t1, m, stack_top[fp - k]);
    }
    stack_push(t1);
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

  def passed_arg
    <<-CODE
    next_int;
    if((unsigned long int)_int < argcount) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  # [Operation]
  #   Test to determine whether a block argument was passed
  # [Format]
  #   \passed_blockarg index
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * true | false
  #   * ...
  # [Description]
  #   Checks if a block was passed to a method, and pushes the result of the
  #   test onto the stack.

  def passed_blockarg
    <<-CODE
    next_int;
    if(_int == blockargs) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
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
    stack_push(t1);
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
    stack_push(as<String>(stack_pop())->string_dup(state));
    CODE
  end

  # [Operation]
  #   Sets the number of args from the stack
  # [Format]
  #   \set_args
  # [Stack Before]
  #   * argc
  #   * ...
  # [Stack After]
  #   * ...
  # [Description]
  #   Pops a fixnum off the top of the stack, and uses its value to set the
  #   argument count for a method prior to executing it.
  # [See Also]
  #   * send_with_arg_register
  #   * send_super_with_arg_register
  # [Notes]
  #   Used prior to send_with_arg_register and send_super_with_arg_register to
  #   set the number of arguments being passed to a method.

  def set_args
    <<-CODE
    sassert(0);
    t1 = stack_pop();
    args = t1->n2i();
    CODE
  end

  # [Operation]
  #   Pushes the number of arguments passed to a method onto the stack
  # [Format]
  #   \get_args
  # [Stack Before]
  #   * ...
  # [Stack After]
  #   * argc
  #   * ...
  # [Description]
  #   Pushes the number of arguments +argc+ actually passed to a method onto
  #   the stack.
  # [See Also]
  #   * set_args

  def get_args
    <<-CODE
    sassert(0);
    stack_push(Object::i2n(args));
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

  def set_call_flags
    <<-CODE
    next_int;
    call_flags = _int;
    CODE
  end

  # [Operation]
  #   Creates a compiler2 style block
  # [Format]
  #   \create_block2
  # [Stack Before]
  #   * compiled_method
  #   * ...
  # [Stack After]
  #   * block_env
  #   * ...
  # [Description]
  #   Pops +compiled_method+ off of the stack, and converts it into a
  #   block environment +block_env+, which is then pushed back onto the stack.
  # [See Also]
  #   * create_block
  # [Notes]
  #   This opcode replaces create_block, which is used under compiler1, but
  #   deprecated under compiler2. Unlike create_block which uses the same
  #   instruction sequence as the enclosing method context, \create_block2
  #   takes its own instruction sequence in the form of a compiled method.

  def create_block
    <<-CODE
    /* the method */
    CompiledMethod* cm = as<CompiledMethod>(stack_pop());

    MethodContext* parent;
    if(kind_of<BlockContext>(active)) {
      parent = as<BlockContext>(active)->env()->home;
    } else {
      parent = active;
    }

    parent->reference(state);
    active->reference(state);

    cm->set_scope(current_scope);

    t2 = BlockEnvironment::under_context(state, cm, parent, active);
    stack_push(t2);
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
    t1 = stack_pop();
    Class* cls = as<Class>(stack_pop());
    if(t1->kind_of_p(state, cls)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
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
    t1 = stack_pop();
    Class* cls = as<Class>(stack_pop());
    if(t1->class_object(state) == cls) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
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
    yield_debugger(Qtrue);
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
    t1 = stack_pop();
    stack_push(t1->fixnum_p() ? Qtrue : Qfalse);
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
    t1 = stack_pop();
    stack_push(t1->symbol_p() ? Qtrue : Qfalse);
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
    t1 = stack_pop();
    stack_push(t1 == Qnil ? Qtrue : Qfalse);
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

  def class
    <<-CODE
    t1 = stack_pop();
    stack_push(t1->class_object(state));
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
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(t1 == t2 ? Qtrue : Qfalse);
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

  def check_serial
    <<-CODE
    t1 = stack_pop();
    next_literal;
    next_int;

    if(check_serial(t1, as<Symbol>(_lit), _int)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

end

si = Instructions.new

Dir.mkdir "gen" unless File.directory?("gen")

File.open("gen/task_instructions_switch.c", "w") do |f|
  si.generate_declarations(f)
  si.generate_switch(f)
end

File.open("gen/iseq_instruction_names.cpp","w") do |f|
  f.puts si.generate_names
end

File.open("gen/iseq_instruction_names.hpp","w") do |f|
  f.puts si.generate_names_header
end

File.open("gen/iseq_instruction_dt_helper.cpp", "w") do |f|
  f.puts si.generate_dter
end

File.open("gen/task_instruction_dt.cpp", "w") do |f|
  si.generate_declarations(f)
  si.generate_threaded(f)
end

File.open("test/test_instructions.hpp", "w") do |f|
  si.generate_tests(f)
end
