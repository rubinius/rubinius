# Instruction related generation code.
#
# This code uses the contents of instructions.rb to create various
# views of the instructions.
#
# This includes their implementations in functions, functions which
# describe instructions, etc.
#

class Object # for vm debugging
  def __show__; end
end

require "#{File.dirname(__FILE__)}/../../kernel/delta/iseq"
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
      if /RETURN\(/.match(body)
        "bool"
      else
        "void"
      end
    end

    # Indicates if this opcode dynamicly defines whether
    # execution should continue
    def custom_continue?
      /RETURN\(/.match(body)
    end

    # Generate a C signature for the implementation code, to be used as a
    # function.
    #
    def signature
      av = args.map { |name| "int #{name}" }.join(", ")
      av = ", #{av}" unless av.empty?
      "bool op_#{name.opcode}(rubinius::VM* state, rubinius::VMMethod* vmm, rubinius::CallFrame* const call_frame #{av})"
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
      io.puts impl.body
      unless impl.custom_continue?
        io.puts "return cExecuteContinue;"
      end
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

      args = impl.args
      case args.size
      when 2
        io.puts "  int #{args[0]} = next_int;"
        io.puts "  int #{args[1]} = next_int;"
        io.puts "  #{impl.body}"
      when 1
        io.puts "  int #{args[0]} = next_int;"
        io.puts "  #{impl.body}"
      when 0
        io.puts "  #{impl.body}"
      end

      io.puts "  break;"
      io.puts "  }"
    end

    io.puts "default:"
    io.puts %q!  std::cout << "Invalid instruction: " << InstructionSequence::get_instruction_name(op) << "\n"; abort();!

    io.puts "}"
  end

  # Using an array of Implementation objects, +methods+, print out
  # the code for each instruction. This uses an indirect threaded
  # goto to jump between instructions.
  #
  def generate_jump_implementations(methods, io, flow=false)
    io.puts generate_jump_table()
    io.puts "DISPATCH_NEXT_INSN;"

    methods.each do |impl|
      io.puts "  op_impl_#{impl.name.opcode}: {"

      args = impl.args
      case args.size
      when 2
        io.puts "  int #{args[0]} = next_int;"
        io.puts "  int #{args[1]} = next_int;"
        io.puts "  #{impl.body}"
      when 1
        io.puts "  int #{args[0]} = next_int;"
        io.puts "  #{impl.body}"
      when 0
        io.puts "  #{impl.body}"
      end

      #if impl.name.check_interrupts?
      #  io.puts "    if(unlikely(state->interrupts.check)) return;"
      #end

      io.puts "  DISPATCH_NEXT_INSN;"
      io.puts "  }"
    end

  end

  # Print to +fd+ a cxxtest formatted class, which contains the test code
  # gathered from instructions.rb.
  #
  def generate_tests(fd)
    missing = []
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
#include "builtin/compiledmethod.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"

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
  cm->iseq(state, InstructionSequence::create(state, 10));
  cm->stack_size(state, Fixnum::from(10));
  cm->local_count(state, Fixnum::from(0));
  cm->literals(state, Tuple::create(state, 10));
  cm->formalize(state);

  MethodContext* ctx = MethodContext::create(state, Qnil, cm);
  task->make_active(ctx);

  opcode stream[100];
  memset(stream, 0, sizeof(opcode) * 100);
  stream[0] = InstructionSequence::insn_#{ins.opcode};

#define run() task->execute_stream(stream)
#{code}
#undef run
}

        EOF
      else
        missing.push "#{ins.opcode}"
      end
    end
    fd.puts "};"
    if missing.any? then
      $stderr.puts "WARN: Missing tests for instructions: #{missing.sort.join(', ')}"
    end
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

  def generate_jump_table
    str = "static const void* insn_locations[] = {\n"
    InstructionSet::OpCodes.each do |ins|
      str << "  &&op_impl_#{ins.opcode.to_s},\n"
    end
    str << "  NULL\n};\n"

    return str
  end

  def generate_ops_prototypes
    str = ""
    str << "namespace rubinius {\n"
    str << "  class VMMethod;\n"
    str << "  class Task;\n"
    str << "  class MethodContext;\n"
    str << "}\n"

    str = "extern \"C\" {\n"
    methods = decode_methods()
    methods.each do |impl|
      str << impl.signature << ";\n"
    end
    str << "}\n"
    return str
  end

  def generate_implementation_info
    str = ""
    size = InstructionSet::OpCodes.size
    str << "const Implementation* implementation(int op) {\n"
    str << "static Implementation implementations[] = {\n"
    InstructionSet::OpCodes.each do |ins|
      str << "{ (void*)op_#{ins.opcode.to_s}, \"op_#{ins.opcode.to_s}\" },\n"
    end

    str << " { NULL, NULL} };\n"
    str << " if(op >= #{size}) return NULL;\n"
    str << " return &implementations[op]; }\n"
    str << "Status check_status(int op) {\n"
    str << "static Status check_status[] = {\n"
    methods = decode_methods()
    methods.each do |impl|
      if impl.custom_continue?
        str << "MightReturn,\n"
      elsif [:return, :raise].include?(impl.name.flow)
        str << "Terminate,\n"
      else
        str << "Unchanged,\n"
      end
    end
    str << " Unchanged };\n"
    str << "if(op >= #{size}) return Unchanged;\n"
    str << "return check_status[op]; }"
    str
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

    str << "const static unsigned int cTotal = #{InstructionSet::OpCodes.size};\n"

    str
  end
end
