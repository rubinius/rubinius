require "#{File.dirname(__FILE__)}/../kernel/core/iseq"
require 'rubygems'
require 'parse_tree'

class Instructions


  Implementation = Struct.new(:name, :sexp, :line, :body)

  class Implementation
    def args
      return @args if defined? @args

      args = sexp[2][1][1]
      args.shift
      ret = []
      while sym = args.shift
        break unless sym.kind_of? Symbol
        ret << sym
      end

      @args = ret
    end

    def return_type
      if /return\s+false/.match(body)
        "bool"
      else
        "void"
      end
    end

    def signature
      av = args.map { |name| "int #{name}" }.join(", ")
      av = ", #{av}" unless av.empty?
      "#{return_type} op_#{name.opcode}(Task* task, struct jit_state* const js #{av})"
    end
  end

  def decode_methods
    pt = ParseTree.new(true)
    
    InstructionSet::OpCodes.map do |ins|
      meth = method(ins.opcode) rescue nil
      if meth
        # Be sure to call it with the right number of args, to get the code
        # out.
        args = [nil] * meth.arity
        code = meth.call *args
        sexp = pt.parse_tree_for_method(Instructions, ins.opcode)
        flat = sexp.flatten
        line = flat[flat.index(:newline) + 1] + 1

        Implementation.new ins, sexp, line, code
      else
        nil
      end
    end
  end

  def generate_functions(methods, io)
    methods.each do |impl|
      io.puts "#{impl.signature} {"
      io.puts "#line #{impl.line} \"instructions.rb\""
      io.puts impl.body
      io.puts "}"
    end
  end

  def generate_decoder_switch(methods, io)
    io.puts "switch(op) {"

    methods.each do |impl|
      io.puts "  case #{impl.name.bytecode}: { // #{impl.name.opcode}"
      args = impl.args
      case args.size
      when 2
        io.puts "  int a1 = next_int;"
        io.puts "  int a2 = next_int;"
        io.puts "  op_#{impl.name.opcode}(task, js, a1, a2);"
      when 1
        io.puts "  int a1 = next_int;"
        io.puts "  op_#{impl.name.opcode}(task, js, a1);"
      when 0
        io.puts "  op_#{impl.name.opcode}(task, js);"
      end

      io.puts "  break;"
      io.puts "  }"
    end
    
    io.puts "}"
  end

  def generate_switch(fd, op="op")
    pt = ParseTree.new(true)

    fd.puts "switch(#{op}) {"
    InstructionSet::OpCodes.each do |ins|
      code = send(ins.opcode) rescue nil
      if code
        sexp = pt.parse_tree_for_method(Instructions, ins.opcode).flatten
        line = sexp[sexp.index(:newline) + 1] + 1

        fd.puts "   case #{ins.bytecode}: { /* #{ins.opcode} */"
        fd.puts "#line #{line} \"#{File.expand_path(__FILE__)}\""
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
        fd.puts "CompiledMethod* cm = CompiledMethod::create(state);"
        fd.puts "cm->iseq = InstructionSequence::create(state, 10);"
        fd.puts "cm->stack_size = Object::i2n(10);"
        fd.puts "cm->local_count = Object::i2n(0);"
        fd.puts "cm->literals = Tuple::create(state, 10);"
        fd.puts "cm->formalize(state);"
        fd.puts "MethodContext* ctx = task->generate_context(Qnil, cm);"
        fd.puts "task->make_active(ctx);"
        # The += 0 disable unused variable warnings.
        fd.puts "Tuple* stack = task->stack; stack += 0;"
        fd.puts "opcode stream[100];"
        fd.puts "stream[0] = InstructionSequence::insn_#{ins.opcode};"
        fd.puts "#define run(val) task->execute_stream(stream)"
        fd.puts "#line #{line} \"#{File.expand_path(__FILE__)}\""
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

  def generate_size
    code = "size_t width = 1; switch(op) {\n"
    InstructionSet::OpCodes.each do |ins|
      if ins.arg_count == 2
        code << "  case #{ins.bytecode}:\n"
      end
    end
    code << "    width = 3; break;\n"

    InstructionSet::OpCodes.each do |ins|
      if ins.arg_count == 1
        code << "  case #{ins.bytecode}:\n"
      end
    end
    code << "   width = 2; break;\n"

    code << "}\n"
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

  def generate_names
    str =  "const char *rubinius::InstructionSequence::get_instruction_name(int op) {\n"
    str << "static const char instruction_names[] = {\n"
    InstructionSet::OpCodes.each do |ins|
      str << "  \"op_#{ins.opcode.to_s}\\0\"\n"
    end
    str << "};\n\n"
    offset = 0
    str << "static const unsigned int instruction_name_offsets[] = {\n"
    InstructionSet::OpCodes.each_with_index do |ins, index|
      str << ",\n" if index > 0
      str << "  #{offset}"
      offset += ins.opcode.to_s.length + 4
    end
    str << "\n};\n\n"
    str << <<CODE
  return instruction_names + instruction_name_offsets[op];
}
CODE
  end

  def generate_names_header
    str = "static const char *get_instruction_name(int op);\n"

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
    stack_push(Object::i2n(val));
    CODE
  end

  def test_push_int
    <<-CODE
    stream[1] = (opcode)47;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 47);
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
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), -1);
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
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 0);
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
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 1);
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
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 2);
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
    MethodContext* ctx = task->active;
    ctx->reference(state);
    stack_push(ctx);
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

  def push_literal(val)
    <<-CODE
    OBJECT t2 = task->literals->field[val];
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

  def set_literal(val)
    <<-CODE
    SET(task->literals, field[val], stack_top());
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
    stack_push(task->self);
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

  def push_local(index)
    <<-CODE
    OBJECT obj = task->stack->field[index];
    stack_push(obj);
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

  def push_local_depth(depth, index)
    <<-CODE
    BlockContext* bc = as<BlockContext>(task->active);
    BlockEnvironment* env;

    for(int j = 0; j < depth; j++) {
      env = bc->env();
      bc = as<BlockContext>(env->home_block);
    }

    stack_push(bc->locals()->at(index));
    CODE
  end

  def test_push_local_depth
    <<-CODE
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active, task->active, 0);
    BlockContext* bc = be->create_context(state, (MethodContext*)Qnil);

    BlockEnvironment* be2 = BlockEnvironment::under_context(state, cm, bc, bc, 1);
    BlockContext* bc2 = be2->create_context(state, (MethodContext*)Qnil);

    task->make_active(bc2);

    bc->locals()->put(state, 0, Qtrue);

    stream[1] = (opcode)1;
    stream[2] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->active->stack->at(task->sp), Qtrue);
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
    stack_push(task->exception);
    CODE
  end

  def test_push_exception
    <<-CODE
    Exception* exc = Exception::create(state);
    task->exception = exc;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->active->stack->at(task->sp), exc);
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
    task->exception = (Exception*)Qnil;
    CODE
  end

  def test_clear_exception
    <<-CODE
    task->exception = Exception::create(state);
    run();
    TS_ASSERT_EQUALS(task->exception, Qnil);
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
    stack_push(task->active->block);
    CODE
  end

  def test_push_block
    <<-CODE
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active, task->active, 0);
    task->active->block = be;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), be);
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
    OBJECT sym = task->literals->field[index];
    stack_push(task->self->get_ivar(state, sym));
    CODE
  end

  def test_push_ivar
    <<-CODE
    SYMBOL name = state->symbol("@blah");
    task->self = Qtrue;
    task->self->set_ivar(state, name, Qtrue);
    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
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
    stack_push(task->self->get_field(state, index));
    CODE
  end

  def test_push_my_field
    <<-CODE
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, Qtrue);

    task->self = tup;

    stream[1] = (opcode)0;

    TS_ASSERT_THROWS(run(), std::runtime_error);

    Class* cls = state->new_class("Blah");

    task->self = cls;

    stream[1] = (opcode)2;

    task->sp = -1;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), state->symbol("Blah"));
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
    task->self->set_field(state, index, stack_top());
    CODE
  end

  def test_store_my_field
    <<-CODE
    Class* cls = state->new_class("Blah");

    task->self = cls;

    SYMBOL name = state->symbol("Foo");
    stack->put(state, ++task->sp, name);
    stream[1] = (opcode)2;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), name);
    TS_ASSERT_EQUALS(cls->name, name);
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
    task->ip = location;
    cache_ip();
    CODE
  end

  def test_goto
    <<-CODE
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
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
    OBJECT t1 = stack_pop();
    #if JIT
    return !RTEST(t1);
    #else
    if(!RTEST(t1)) {
      task->ip = location;
      cache_ip();
    }
    #endif
    CODE
  end

  def test_goto_if_false
    <<-CODE
    stack->put(state, ++task->sp, Qtrue);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    TS_ASSERT_EQUALS(task->ip, 0);

    stack->put(state, ++task->sp, Qfalse);
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
    TS_ASSERT_EQUALS(task->sp, -1);
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
    OBJECT t1 = stack_pop();
    #if JIT
    return RTEST(t1);
    #else
    if(RTEST(t1)) {
      task->ip = location;
      cache_ip();
    }
    #endif
    CODE
  end

  def test_goto_if_true
    <<-CODE
    stack->put(state, ++task->sp, Qfalse);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    TS_ASSERT_EQUALS(task->ip, 0);

    stack->put(state, ++task->sp, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
    TS_ASSERT_EQUALS(task->sp, -1);
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
    OBJECT t1 = stack_pop();
    #if JIT
    return t1 != Qundef;
    #else
    if(t1 != Qundef) {
      task->ip = location;
      cache_ip();
    }
    #endif
    CODE
  end

  def test_goto_if_defined
    <<-CODE
    stack->put(state, ++task->sp, Qundef);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    TS_ASSERT_EQUALS(task->ip, 0);

    stack->put(state, ++task->sp, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
    TS_ASSERT_EQUALS(task->sp, -1);
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
    OBJECT t1 = stack_pop();
    OBJECT t2 = stack_pop();
    stack_push(t1);
    stack_push(t2);
    CODE
  end

  def test_swap_stack
    <<-CODE
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Qfalse);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);
    TS_ASSERT_EQUALS(stack->at(1), Qtrue);

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
    OBJECT t1 = stack_top();
    stack_push(t1);
    CODE
  end

  def test_dup_top
    <<-CODE
    stack->put(state, ++task->sp, Qtrue);

    run();

    TS_ASSERT_EQUALS(task->sp, 1);
    TS_ASSERT_EQUALS(stack->at(1), Qtrue);
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
    stack->put(state, ++task->sp, Qtrue);

    run();

    TS_ASSERT_EQUALS(task->sp, -1);
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

  def set_local(index)
    <<-CODE
    task->stack->field[index] = stack_top();
    CODE
  end

  def test_set_local
    <<-CODE
    task->sp++; /* reserve space */
    stack->put(state, ++task->sp, Qtrue);

    stream[1] = (opcode)0;
    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);
    TS_ASSERT_EQUALS(task->sp, 1);
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
    BlockContext* bc = as<BlockContext>(task->active);
    BlockEnvironment* env;

    for(int j = 0; j < depth; j++) {
      env = bc->env();
      bc = as<BlockContext>(env->home_block);
    }

    OBJECT t3 = stack_pop();
    Tuple *tup = bc->locals();
    tup->put(state, index, t3);
    stack_push(t3);
    CODE
  end

  def test_set_local_depth
    <<-CODE
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active, task->active, 0);
    BlockContext* bc = be->create_context(state, (MethodContext*)Qnil);

    BlockEnvironment* be2 = BlockEnvironment::under_context(state, cm, bc, bc, 0);
    BlockContext* bc2 = be2->create_context(state, (MethodContext*)Qnil);

    task->make_active(bc2);

    task->stack->put(state, ++task->sp, Qtrue);
    bc->locals()->put(state, 0, Qnil);

    stream[1] = (opcode)1;
    stream[2] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->active->stack->at(task->sp), Qtrue);
    TS_ASSERT_EQUALS(bc->locals()->at(0), Qtrue);
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
    OBJECT t2;
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
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Qfalse);

    stream[1] = 2;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    Array* ary = as<Array>(stack->at(task->sp));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qfalse);
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
    OBJECT t1 = stack_pop();
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
    stack->put(state, ++task->sp, Qtrue);
    run();

    Array* ary = as<Array>(stack->at(0));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);

    stack->put(state, 0, Tuple::from(state, 1, Qfalse));
    run();

    ary = as<Array>(stack->at(0));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qfalse);

    Array* custom = Array::create(state, 1);
    stack->put(state, 0, custom);
    run();

    TS_ASSERT_EQUALS(stack->at(0), custom);
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
    OBJECT t1 = stack_pop();
    if(kind_of<Array>(t1)) {
      Array* ary = as<Array>(t1);
      int j = ary->size();

      Tuple* tup = Tuple::create(state, j);

      for(int k = 0; k < j; k++) {
        tup->put(state, k, ary->get(state, k));
      }
      t1 = tup;
    } else if(!kind_of<Tuple>(t1)) {
      Tuple* tup = Tuple::create(state, 1);
      tup->put(state, 0, t1);
      t1 = tup;
    }
    stack_push(t1);
    CODE
  end

  def test_cast_tuple
    <<-CODE
    Array* custom = Array::create(state, 1);
    custom->set(state, 0, Qtrue);
    stack->put(state, ++task->sp, custom);
    run();

    Tuple* tup = as<Tuple>(stack->at(0));
    TS_ASSERT_EQUALS(tup->at(0), Qtrue);


    stack->put(state, 0, Qfalse);
    run();

    tup = as<Tuple>(stack->at(0));
    TS_ASSERT_EQUALS(tup->at(0), Qfalse);

    tup = Tuple::create(state, 1);
    stack->put(state, 0, tup);
    run();

    TS_ASSERT_EQUALS(tup, stack->at(0));
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
    int k = NUM_FIELDS(tup);
    if(k == 0) {
      stack_push(Qnil);
    } else if(k == 1) {
      stack_push(tup->at(0));
    } else {
      stack_push(Array::from_tuple(state, tup));
    }
    CODE
  end

  def test_cast_for_single_block_arg
    <<-CODE
    Tuple* tup = Tuple::create(state, 0);
    stack->put(state, ++task->sp, tup);
    run();

    TS_ASSERT_EQUALS(stack->at(0), Qnil);

    tup = Tuple::from(state, 1, Qtrue);
    stack->put(state, 0, tup);
    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);

    tup = Tuple::from(state, 2, Qtrue, Qfalse);
    stack->put(state, 0, tup);
    run();

    Array* ary = as<Array>(stack->at(0));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qfalse);
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
    int k = NUM_FIELDS(tup);
    /* If there is only one thing in the tuple... */
    if(k == 1) {
      OBJECT t1 = tup->at(0);
      /* and that thing is an array... */
      if(kind_of<Array>(t1)) {
        /* make a tuple out of the array contents... */
        Array* ary = as<Array>(t1);
        int j = ary->size();
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

  def test_cast_for_multi_block_arg
    <<-CODE
    Tuple* tup = Tuple::from(state, 2, Qtrue, Qfalse);
    stack->put(state, ++task->sp, tup);
    run();

    TS_ASSERT_EQUALS(stack->at(0), tup);

    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Object::i2n(1));
    tup = Tuple::from(state, 1, ary);
    stack->put(state, task->sp, tup);
    run();

    tup = as<Tuple>(stack->at(0));
    TS_ASSERT_EQUALS(tup->at(0), Object::i2n(1));
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
    OBJECT sym = task->literals->field[index];
    task->self->set_ivar(state, sym, stack_top());
    CODE
  end

  def test_set_ivar
    <<-CODE
    SYMBOL name = state->symbol("@blah");
    task->self = Qtrue;
    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, Qfalse);
    run();

    TS_ASSERT_EQUALS(Qtrue->get_ivar(state, name), Qfalse);
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qfalse);
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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT res = task->const_get(sym, &found);
    if(!found) {
      assert("implement const_missing");
    } else {
      stack_push(res);
    }
    CODE
  end

  def test_push_const
    <<-CODE
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, parent);
    ps->parent = (StaticScope*)Qnil;

    StaticScope* cs = StaticScope::create(state);
    SET(cs, module, child);
    SET(cs, parent, ps);

    SET(cm, scope, cs);

    SYMBOL name = state->symbol("Number");
    parent->set_const(state, name, Object::i2n(3));

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(3));

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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT res = task->const_get(under, sym, &found);
    if(!found) {
      assert("implement const_missing");
    } else {
      stack_push(res);
    }
    CODE
  end

  def test_find_const
    <<-CODE
    SYMBOL name = state->symbol("Number");
    G(true_class)->set_const(state, name, Object::i2n(3));

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, G(true_class));

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(3));
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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    task->const_set(sym, stack_top());
    CODE
  end

  def test_set_const
    <<-CODE
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, parent);
    ps->parent = (StaticScope*)Qnil;

    SET(cm, scope, ps);
    SYMBOL name = state->symbol("Age");

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, Object::i2n(3));
    run();

    TS_ASSERT_EQUALS(parent->get_const(state, name), Object::i2n(3));
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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT val = stack_pop();
    Module* under = as<Module>(stack_pop());

    task->const_set(under, sym, val);
    stack_push(val);
    CODE
  end

  def test_set_const_at
    <<-CODE
    SYMBOL name = state->symbol("Age");
    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, G(true_class));
    stack->put(state, ++task->sp, Qtrue);

    run();

    TS_ASSERT_EQUALS(G(true_class)->get_const(state, name), Qtrue);

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
    TS_ASSERT_EQUALS(stack->at(task->sp), G(object));
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
    OBJECT super = stack_pop();
    Module* under = as<Module>(stack_pop());
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    Class* cls = task->open_class(under, super, sym, &created);
    // TODO use created? it's only for running the opened_class hook, which
    // we're eliminating anyway.

    stack_push(cls);
    CODE
  end

  def test_open_class_under
    <<-CODE
    SYMBOL name = state->symbol("C");
    stack->put(state, ++task->sp, G(true_class));
    stack->put(state, ++task->sp, Qnil);

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Class>(G(true_class)->get_const(state, name)));
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
    OBJECT super = stack_pop();
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    Class* cls = task->open_class(super, sym, &created);

    stack_push(cls);
    CODE
  end

  def test_open_class
    <<-CODE
    SYMBOL name = state->symbol("C");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, G(true_class));
    ps->parent = (StaticScope*)Qnil;
    SET(cm, scope, ps);

    stack->put(state, ++task->sp, Qnil);

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Class>(G(true_class)->get_const(state, name)));
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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    stack_push(task->open_module(mod, sym));
    CODE
  end

  def test_open_module_under
    <<-CODE
    SYMBOL name = state->symbol("C");
    stack->put(state, ++task->sp, G(true_class));

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Module>(G(true_class)->get_const(state, name)));
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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    stack_push(task->open_module(sym));
    CODE
  end

  def test_open_module
    <<-CODE
    SYMBOL name = state->symbol("C");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, G(true_class));
    ps->parent = (StaticScope*)Qnil;
    SET(cm, scope, ps);

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Module>(G(true_class)->get_const(state, name)));
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
    OBJECT t1 = stack_pop();
    stack_push(t1->metaclass(state));
    CODE
  end

  def test_open_metaclass
    <<-CODE
    Tuple* tup = Tuple::create(state, 1);
    stack->put(state, ++task->sp, tup);

    run();

    TS_ASSERT_EQUALS(stack->at(0), tup->metaclass(state));
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
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT obj = stack_pop();
    OBJECT obj2 = stack_pop();
    CompiledMethod* meth = as<CompiledMethod>(obj2);

    task->attach_method(obj, sym, meth);
    stack_push(meth);
    CODE
  end

  def test_attach_method
    <<-CODE
    SYMBOL name = state->symbol("blah");
    task->literals->put(state, 0, name);

    stack->put(state, ++task->sp, cm);
    stack->put(state, ++task->sp, G(true_class));

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(G(true_class)->metaclass(state)->method_table->fetch(state, name), cm);
    TS_ASSERT(!cm->scope->nil_p());
    TS_ASSERT_EQUALS(cm->scope->module, G(true_class));
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
  #
  #   NOTE: this instruction is only emitted when the compiler is in kernel mode.
  #   In normal ruby code, the method __add_method__ is sent to self to add a method.
  # [See Also]
  #   * attach_method
  # [Notes]
  #   Singleton methods are handled by attach_method.

  def add_method(index)
    <<-CODE
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    Module* obj = as<Module>(stack_pop());
    CompiledMethod* meth = as<CompiledMethod>(stack_pop());

    task->add_method(obj, sym, meth);
    stack_push(meth);
    CODE
  end

  def test_add_method
    <<-CODE
    SYMBOL name = state->symbol("blah");
    task->literals->put(state, 0, name);

    stack->put(state, ++task->sp, cm);
    stack->put(state, ++task->sp, G(true_class));

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(G(true_class)->method_table->fetch(state, name), cm);
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

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.recv = stack_top();
    msg.block = Qnil;
    msg.splat = Qnil;
    msg.args = 0;
    msg.stack = 1;

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
    CODE
  end

  def test_send_method
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 1);
    target->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_ret));
    target->total_args = Object::i2n(0);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(10);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);

    stream[1] = (opcode)0;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 0U);
    TS_ASSERT_EQUALS(task->self, Qtrue);
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

  def send_stack(index, count)
    <<-CODE
    Message& msg = *task->msg;

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.recv = stack_back(count);
    msg.block = Qnil;
    msg.splat = Qnil;
    msg.args = count;
    msg.stack = count + 1;
    msg.use_from_task(task, count);

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
    CODE
  end

  def test_send_stack
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 1);
    target->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_ret));
    target->total_args = Object::i2n(1);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(1);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Object::i2n(3));

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    MethodContext* s = task->active->sender;
    TS_ASSERT_EQUALS(s->sp, -1);
    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 1U);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->self, Qtrue);
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

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.block = stack_pop();
    msg.splat = Qnil;
    msg.args = count;
    msg.recv = stack_back(count);
    msg.stack = count + 2;
    msg.use_from_task(task, count);

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
    CODE
  end

  def test_send_stack_with_block
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 1);
    target->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_ret));
    target->total_args = Object::i2n(1);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(1);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Object::i2n(3));

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active, 0);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 1U);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, Qtrue);
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

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.block = stack_pop();
    OBJECT ary = stack_pop();
    msg.splat = Qnil;
    msg.args = count;
    msg.recv = stack_back(count);
    msg.stack = count + 3;
    msg.combine_with_splat(state, task, as<Array>(ary)); /* call_flags & 3 */

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
    CODE
  end

  def test_send_stack_with_splat
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 1);
    target->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_ret));
    target->total_args = Object::i2n(2);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(2);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Object::i2n(3));

    Array* splat = Array::create(state, 1);
    splat->set(state, 0, Object::i2n(47));
    stack->put(state, ++task->sp, splat);

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active, 0);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 2U);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->stack->at(1), Object::i2n(47));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, Qtrue);
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
    
    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.block = stack_pop();
    msg.splat = Qnil;
    msg.args = count;
    msg.recv = stack_back(count);
    msg.stack = count + 2;
    msg.use_from_task(task, count);

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
    CODE
  end

  def test_send_super_stack_with_block
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 1);
    target->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_ret));
    target->total_args = Object::i2n(1);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(1);

    Class* parent = state->new_class("Parent", 1);
    Class* child =  state->new_class("Child", parent, 1);

    SYMBOL name = state->symbol("blah");
    parent->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    OBJECT obj = state->new_object(child);
    task->self = obj;

    StaticScope *sc = StaticScope::create(state);
    SET(sc, module, child);

    SET(cm, scope, sc);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, obj);
    stack->put(state, ++task->sp, Object::i2n(3));

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active, 0);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 1U);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, obj);
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
  #   objects  superclass. The arguments to the method areleft on the top of
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

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.block = stack_pop();
    OBJECT ary = stack_pop();
    msg.splat = Qnil;
    msg.args = count;
    msg.recv = task->self;
    msg.stack = count + 2;
    msg.combine_with_splat(state, task, as<Array>(ary)); /* call_flags & 3 */

    msg.priv = TRUE;
    msg.lookup_from = task->current_module()->superclass;
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
    CODE
  end

  def test_send_super_stack_with_splat
    <<-CODE
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 1);
    target->iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_ret));
    target->total_args = Object::i2n(2);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(2);

    Class* parent = state->new_class("Parent", 1);
    Class* child =  state->new_class("Child", parent, 1);

    SYMBOL name = state->symbol("blah");
    parent->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    OBJECT obj = state->new_object(child);
    task->self = obj;

    StaticScope *sc = StaticScope::create(state);
    SET(sc, module, child);

    SET(cm, scope, sc);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, obj);
    stack->put(state, ++task->sp, Object::i2n(3));

    Array* splat = Array::create(state, 1);
    splat->set(state, 0, Object::i2n(47));
    stack->put(state, ++task->sp, splat);

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active, 0);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    target->formalize(state);

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 2U);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->stack->at(1), Object::i2n(47));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, obj);
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
    OBJECT t1 = stack_pop(); // include_private
    SYMBOL name = as<Symbol>(stack_pop()); // meth
    OBJECT t3 = stack_pop(); // self
    stack_push(task->locate_method_on(t3, name, t1));
    CODE
  end

  def test_locate_method
    <<-CODE
    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, cm);

    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, name);
    stack->put(state, ++task->sp, Qfalse);

    run();

    TS_ASSERT_EQUALS(stack->at(0), cm);
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
    OBJECT left =  stack_back(1);
    OBJECT right = stack_back(0);

    if(both_fixnum_p(left, right)) {
      stack_pop();
      stack_pop();
      OBJECT res = ((FIXNUM)(left))->add(state, (FIXNUM)(right));
      stack_push(res);
      return false;
    }

    return send_slowly(task, js, G(sym_plus));
    CODE
  end

  def test_meta_send_op_plus
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, one);
    stack->put(state, ++task->sp, two);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(3));

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
    OBJECT left =  stack_back(1);
    OBJECT right = stack_back(0);

    if(both_fixnum_p(left, right)) {
      stack_pop();
      stack_pop();
      OBJECT res = ((FIXNUM)(left))->sub(state, (FIXNUM)(right));
      stack_push(res);
      return false;
    }

    return send_slowly(task, js, G(sym_minus));
    CODE
  end

  def test_meta_send_op_minus
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(1));

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
    OBJECT t1 = stack_back(1);
    OBJECT t2 = stack_back(0);
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qtrue : Qfalse);
      return false;
    }
    
    return send_slowly(task, js, G(sym_equal));
    CODE
  end

  def test_meta_send_op_equal
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);

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
    OBJECT t1 = stack_back(1);
    OBJECT t2 = stack_back(0);
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qfalse : Qtrue);
      return false;
    }
    
    return send_slowly(task, js, G(sym_nequal));
    CODE
  end

  def test_meta_send_op_nequal
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);

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
    OBJECT t1 = stack_back(1);
    OBJECT t2 = stack_back(0);
    /* If both are fixnums, or both are symbols, compare the ops directly. */
    if((t1->fixnum_p() && t2->fixnum_p()) || (t1->symbol_p() && t2->symbol_p())) {
      stack_pop();
      stack_set_top((t1 == t2) ? Qtrue : Qfalse);
      return false;
    }
    
    return send_slowly(task, js, G(sym_tequal));
    CODE
  end

  def test_meta_send_op_tequal
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);

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
    OBJECT t1 = stack_back(1);
    OBJECT t2 = stack_back(0);
    if(t1->fixnum_p() && t2->fixnum_p()) {
      int j = as<Integer>(t1)->n2i();
      int k = as<Integer>(t2)->n2i();
      stack_pop();
      stack_set_top((j < k) ? Qtrue : Qfalse);
      return false;
    }
    
    return send_slowly(task, js, G(sym_lt));
    CODE
  end

  def test_meta_send_op_lt
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, one);
    stack->put(state, ++task->sp, two);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);

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
    OBJECT t1 = stack_back(1);
    OBJECT t2 = stack_back(0);
    if(t1->fixnum_p() && t2->fixnum_p()) {
      int j = as<Integer>(t1)->n2i();
      int k = as<Integer>(t2)->n2i();
      stack_pop();
      stack_set_top((j > k) ? Qtrue : Qfalse);
      return false;
    }
    
    return send_slowly(task, js, G(sym_gt));
    CODE
  end

  def test_meta_send_op_gt
    <<-CODE
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, one);
    stack->put(state, ++task->sp, two);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);

    CODE
  end

  def meta_send_call(count)
    <<-CODE
    OBJECT t1 = stack_back(count);

    if(kind_of<BlockEnvironment>(t1)) {
      as<BlockEnvironment>(t1)->call(state, task, count);
      return false;
    }

    return send_slowly(task, js, G(sym_call));
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
    OBJECT t1 = stack_pop();
    task->simple_return(state, t1);
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
    OBJECT t1 = stack_pop();
    task->raise_exception(as<Exception>(t1));
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
    task->simple_return(stack_top());
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
    OBJECT t1 = stack_pop();
    Tuple* tup = as<Tuple>(t1);
    if(NUM_FIELDS(t1) == 0) {
      stack_push(tup);
      stack_push(Qnil);
    } else {
      int j = NUM_FIELDS(t1) - 1;
      OBJECT t2 = tup->at(0);
      Tuple* tup = Tuple::create(state, j);
      tup->copy_from(state, tup, 1, j);
      stack_push(tup);
      stack_push(t2);
    }
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

  def passed_arg(count)
    <<-CODE
    if((unsigned long int)count < task->active->args) {
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

  def passed_blockarg(count)
    <<-CODE
    if((unsigned int)count == task->blockargs) {
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
    OBJECT t1 = stack_pop();
    OBJECT t2 = stack_pop();
    String* s1 = as<String>(t1);
    String* s2 = as<String>(t2);
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
    String *s1 = as<String>(stack_pop());
    stack_push(s1->string_dup(state));
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
  #   Takes a +compiled_method+ out o the literals tuple, and converts it into a
  #   block environment +block_env+, which is then pushed back onto the stack.

  def create_block(index)
    <<-CODE
    /* the method */
    OBJECT _lit = task->literals->field[index];
    CompiledMethod* cm = as<CompiledMethod>(_lit);

    MethodContext* parent;
    if(kind_of<BlockContext>(task->active)) {
      parent = as<BlockContext>(task->active)->env()->home;
    } else {
      parent = task->active;
    }

    parent->reference(state);
    task->active->reference(state);

    cm->set_scope(task->active->cm->scope);

    OBJECT t2 = BlockEnvironment::under_context(state, cm, parent, task->active, index);
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
    OBJECT t1 = stack_pop();
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
    OBJECT t1 = stack_pop();
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
    task->yield_debugger();
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
    OBJECT t1 = stack_pop();
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
    OBJECT t1 = stack_pop();
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
    OBJECT t1 = stack_pop();
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
    OBJECT t1 = stack_pop();
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
    OBJECT t1 = stack_pop();
    OBJECT t2 = stack_pop();
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

  def check_serial(index, serial)
    <<-CODE
    OBJECT t1 = stack_pop();
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    if(task->check_serial(t1, sym, serial)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

end


if $0 == __FILE__
  si = Instructions.new

  Dir.mkdir "gen" unless File.directory?("gen")

  File.open("gen/iseq_instruction_names.cpp","w") do |f|
    f.puts si.generate_names
  end

  File.open("gen/iseq_instruction_names.hpp","w") do |f|
    f.puts si.generate_names_header
  end

  File.open("gen/iseq_instruction_dt_helper.cpp", "w") do |f|
    f.puts si.generate_dter
  end

  File.open("gen/iseq_instruction_size.gen", "w") do |f|
    f.puts si.generate_size
  end

  File.open("test/test_instructions.hpp", "w") do |f|
    si.generate_tests(f)
  end

end
