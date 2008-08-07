# Instruction related generation code.
#
# This code uses the contents of instructions.rb to create varies
# views of the instructions.
#
# This includes their implementations in functions, functions which
# describe instructions, etc.
#

require "#{File.dirname(__FILE__)}/../kernel/core/iseq"
require 'rubygems'
require 'parse_tree'

class Instructions

  # Wraps instruction implementation code provided by instructions.rb.
  # It also captures some meta info using parsetree to generate
  # better output.
  #
  Implementation = Struct.new(:name, :sexp, :line, :body)

  class Implementation

    # Return a list of symbols, containing the names of the arguments
    # this code takes. The list is extracted from the ruby method signature.
    #
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

    # Look at the implementation code and figure out if the implementation
    # code returns void or bool.
    #
    def return_type
      if /return\s+/.match(body)
        "bool"
      else
        "void"
      end
    end

    # Generate a C signature for the implementation code, to be used as a
    # function.
    #
    def signature
      av = args.map { |name| "int #{name}" }.join(", ")
      av = ", #{av}" unless av.empty?
      "#{return_type} op_#{name.opcode}(Task* task, struct jit_state* const js #{av})"
    end
  end

  # Using InstructionSet::OpCodes as a key, gather up all the implementation
  # code into Implementation objects from instructions.rb and return it.
  #
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

  # Using an array of Implementation objects, +methods+, print one function
  # per object into +io+.
  #
  def generate_functions(methods, io)
    methods.each do |impl|
      io.puts "#{impl.signature} {"
      io.puts "#line #{impl.line} \"vm/instructions.rb\""
      io.puts impl.body
      io.puts "}"
    end
  end

  # Using an array of Implementation objects, +methods+, print a switch
  # statement which decodes the arguments and calls the function that
  # contains the implementation of the instruction.
  #
  def generate_decoder_switch(methods, io, flow=false)
    io.puts "switch(op) {"

    methods.each do |impl|
      io.puts "  case #{impl.name.bytecode}: { // #{impl.name.opcode}"
      if flow and impl.name.flow and impl.name.flow != :sequential
        # Flush the ip register from the task
        io.puts "  ctx->ip = task->ip;"
      end

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

      if flow
        if [:send, :return, :raise].include?(impl.name.flow)
          io.puts "  return;"
        end
      end

      io.puts "  break;"
      io.puts "  }"
    end

    io.puts "}"
  end

  # Print to +fd+ a cxxtest formatted class, which contains the test code
  # gathered from instructions.rb.
  #
  def generate_tests(fd)
    pt = ParseTree.new(true)

    fd.puts <<-CODE
#include "builtin/iseq.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/list.hpp"
#include "builtin/task.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/contexts.hpp"
#include "builtin/exception.hpp"
#include "builtin/sendsite.hpp"

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

        fd.puts <<-EOF
void #{meth}() {
  Task* task = Task::create(state);
  CompiledMethod* cm = CompiledMethod::create(state);
  cm->iseq = InstructionSequence::create(state, 10);
  cm->stack_size = Object::i2n(10);
  cm->local_count = Object::i2n(0);
  cm->literals = Tuple::create(state, 10);
  cm->formalize(state);

  MethodContext* ctx = task->generate_context(Qnil, cm);
  task->make_active(ctx);
  // The += 0 disable unused variable warnings.

  Tuple* stack = task->current_stack(); stack += 0;

  opcode stream[100];
  stream[0] = InstructionSequence::insn_#{ins.opcode};

#define run(val) task->execute_stream(stream)
#line #{line} \"vm/instructions.rb\"
#{code}
#undef run
}

        EOF
      end
    end
    fd.puts "};"
  end

  # Generate a switch statement which, given +op+, sets +width+ to
  # how many operands +op+ takes.
  #
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

  # Generate a function (get_instruction_name) which, given +op+, returns
  # a char* that is the name of the function that implements the instruction.
  #
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

  # Generate header information for instruction functions and other
  # info.
  #
  def generate_names_header
    str = "static const char *get_instruction_name(int op);\n"

    str << "typedef enum {\n"
    InstructionSet::OpCodes.each do |ins|
      str << "insn_#{ins.opcode.to_s} = #{ins.bytecode},\n"
    end
    str << "} instruction_names;\n"

    str
  end
end
