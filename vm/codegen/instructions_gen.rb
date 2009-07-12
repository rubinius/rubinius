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

module Rubinius
  LookupTable = Hash
end

require 'yaml'
require "#{File.dirname(__FILE__)}/../../kernel/compiler/iseq"
require 'rubygems'
require 'parse_tree'

class Instructions

  # Wraps instruction implementation code provided by instructions.rb.
  # It also captures some meta info using parsetree to generate
  # better output.
  #
  Implementation = Struct.new(:name, :sexp, :line, :body)

  class Implementation

    attr_accessor :super_insn

    def super_insn?
      defined? @super_insn and @super_insn
    end

    # Return a list of symbols, containing the names of the arguments
    # this code takes. The list is extracted from the ruby method signature.
    #
    def args
      return @args if defined? @args
      return [] unless sexp

      args = sexp[2][1][1]
      args.shift
      ret = []
      while sym = args.shift
        break unless sym.kind_of? Symbol
        ret << sym
      end

      @args = ret
    end

    attr_writer :args

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
      /RETURN\(/.match(body) || /HANDLE_EXCEPTION\(/.match(body) || /RUN_EXCEPTION\(/.match(body)
    end

    # Generate a C signature for the implementation code, to be used as a
    # function.
    #
    def signature
      av = args.map { |name| "int #{name}" }.join(", ")
      av = ", #{av}" unless av.empty?
      "Object* op_#{name.opcode}(rubinius::VM* state, rubinius::VMMethod* vmm, rubinius::InterpreterCallFrame* const call_frame, rubinius::VMMethod::InterpreterState& is #{av})"
    end
  end

  def initialize
    @superinsns = YAML.load File.read("#{File.dirname __FILE__}/super-instructions.yml")
  end

  def get_code(impl,which)
    meth = method(impl.name.opcode) rescue nil
    if meth
      # Be sure to call it with the right number of args, to get the code
      # out.
      args = [nil] * meth.arity
      body = meth.call(*args)
      args = impl.args
      case args.size
      when 2
        names = [args[0], args[1]]
      when 1
        names = [args[0]]
      else
        names = []
      end

      code = "  { // #{impl.name.opcode}\n"
      names.each do |name|
        code << "    intptr_t #{name}_s#{which} = next_int;\n"
        code << "    #define #{name} #{name}_s#{which}\n"
      end
      code << "#{body}\n"
      names.each do |name|
        code << "    #undef #{name}\n"
      end
      code << "    }\n"
      return code
    end

    return nil
  end

  # Using InstructionSet.opcodes as a key, gather up all the implementation
  # code into Implementation objects from instructions.rb and return it.
  #
  def decode_methods
    pt = ParseTree.new(true)

    basic = Rubinius::InstructionSet.opcodes.map do |ins|
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

    return basic
  end

  def inject_superops(impls)
    start = impls.size

    @superinsns.each_with_index do |combo, superop|
      args = []
      combo.each do |op|
        args << :opcode
        args += Rubinius::InstructionSet[op].args
      end

      args.shift

      stack = [0,0]
      combo.each do |op|
        insn_stack = Rubinius::InstructionSet[op].stack
        stack[0] += insn_stack[0]
        stack[1] += insn_stack[1]
      end

      info = {
        :opcode => "superop_#{superop}".to_sym,
        :bytecode => start + superop,
        :args => args,
        :stack => stack
      }

      ins = Rubinius::InstructionSet::OpCode.new info[:opcode], info[:bytecode], info
      composite = combo.map { |op| impls[Rubinius::InstructionSet[op].bytecode] }
      code = construct_code(composite)

      impl = Implementation.new(ins, nil, 0, code)
      impl.args = args
      impl.super_insn = true
      impls << impl
    end
  end

  def construct_code(combo)
    code = ""
    first = true
    combo.each do |insn|
      code << "    next_int; // eat next instruction\n" unless first
      code << get_code(insn , 1)
      first = false
    end
    return code
  end

  # Using an array of Implementation objects, +methods+, print one function
  # per object into +io+.
  #
  def generate_functions(methods, io)
    methods.each do |impl|
      io.puts "#{impl.signature} {"
      io.puts impl.body
      io.puts "return stack_top();"
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
        io.puts "  intptr_t #{args[0]} = next_int;"
        io.puts "  intptr_t #{args[1]} = next_int;"
        io.puts "  #{impl.body}"
      when 1
        io.puts "  intptr_t #{args[0]} = next_int;"
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
  def generate_jump_implementations(methods, io)
    io.puts "DISPATCH;"

    methods.each do |impl|
      io.puts "  op_impl_#{impl.name.opcode}: {"

      if impl.super_insn?
        io.puts "  #{impl.body}"
      else
        args = impl.args
        case args.size
        when 2
          io.puts "  intptr_t #{args[0]} = next_int;"
          io.puts "  intptr_t #{args[1]} = next_int;"
          io.puts "  #{impl.body}"
        when 1
          io.puts "  intptr_t #{args[0]} = next_int;"
          io.puts "  #{impl.body}"
        when 0
          io.puts "  #{impl.body}"
        end
      end

      #if impl.name.check_interrupts?
      #  io.puts "    if(unlikely(state->interrupts.check)) return;"
      #end

      io.puts "  DISPATCH;"
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

    Rubinius::InstructionSet.opcodes.each do |ins|
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
  def generate_size(methods)
    code = "size_t width = 1; switch(op) {\n"
    methods.each do |impl|
      code << "  case #{impl.name.bytecode}:\n"
      code << "    width = #{impl.args.size + 1}; break; \n"
    end

    code << "}\n"
  end

  # Generate a function (get_instruction_name) which, given +op+, returns
  # a char* that is the name of the function that implements the instruction.
  #
  def generate_names(methods)
    str =  "const char *rubinius::InstructionSequence::get_instruction_name(int op) {\n"
    str << "static const char instruction_names[] = {\n"
    methods.each do |impl|
      str << "  \"op_#{impl.name.opcode.to_s}\\0\"\n"
    end
    str << "};\n\n"
    offset = 0
    str << "static const unsigned int instruction_name_offsets[] = {\n"
    methods.each_with_index do |impl, index|
      str << ",\n" if index > 0
      str << "  #{offset}"
      offset += impl.name.opcode.to_s.length + 4
    end
    str << "\n};\n\n"
    str << <<CODE
  return instruction_names + instruction_name_offsets[op];
}
CODE
  end

  def generate_jump_table(methods, in_vmmethod=false)
    str = "static const void* insn_locations[] = {\n"
    methods.each do |impl|
      str << "  &&op_impl_#{impl.name.opcode},\n"
    end
    str << "  NULL\n};\n"

    if in_vmmethod
      str << "if(unlikely(state == 0)) { VMMethod::instructions = const_cast<void**>(insn_locations); return NULL; }\n"
    end

    return str
  end

  def output_node(indent, code, node, distance)
    regular = Rubinius::InstructionSet.opcodes.size

    if node.size == 1 and node.key?(:__superop__)
      code << "#{' ' * indent}return #{node[:__superop__] + regular};\n"
      return
    end

    term = false
    code << "#{' ' * indent}switch(stream[#{distance}]) {\n"
    node.each do |name, sub|
      if name == :__superop__
        term = true
        code << "#{' ' * indent}default: return #{sub + regular};\n"
      else
        inst = Rubinius::InstructionSet[name]
        code << "#{' ' * indent}case #{inst.bytecode}: // #{name}\n"
        output_node(indent + 2, code, sub, distance + inst.width)
      end
    end

    unless term
      code << "#{' ' * indent}default: return -1;\n"
    end

    code << "#{' ' * indent}}\n"

    return code
  end

  def generate_superinst_finder
    create = proc { |h,k| h[k] = Hash.new(&create) }
    tree = Hash.new(&create)

    @superinsns.each_with_index do |combo, superop|
      node = tree[combo.first]
      1.upto(combo.size - 1) do |idx|
        node = node[combo[idx]]
      end

      node[:__superop__] = superop
    end

    code = ""
    code << "int find_superop(opcode* stream) {\n"
    code << "  switch(stream[0]) {\n"
    tree.each do |name, node|
      insn = Rubinius::InstructionSet[name]
      code << "  case #{insn.bytecode}: // #{name}\n"
      output_node 4, code, node, insn.width
    end
    code << "  }\n"
    code << "  return -1;\n"
    code << "}\n"

    regular = Rubinius::InstructionSet.opcodes.size
    code << "int reverse_superop(opcode superop) {\n"
    code << "  static int superops[] = {"
    @superinsns.each_with_index do |combo, superop|
      insn = Rubinius::InstructionSet[combo.first]
      code << " #{insn.bytecode},"
    end
    code << "};\n"
    code << "  if(superop < #{regular}) return superop;\n"
    code << "  return superops[superop - #{regular}];\n"
    code << "}\n"
=begin
    code << "int find_superop(opcode* stream) {\n"
    code << "  opcode one = stream[0];\n"
    code << "  opcode two = 0;\n"
    code << "  switch(one) {\n"
    tree.each do |name, node|
      inst = InstructionSet[name]
      code << "  case #{inst.bytecode}: // #{insn}\n"
      code << "    two = stream[#{inst.width}];\n"
      code << "    switch(two) {\n"
      v.each do |sub, op|
        inst2 = InstructionSet[sub].bytecode
        code << "    case #{inst2}: // #{sub}\n"
        code << "      return #{op + InstructionSet.opcodes.size};\n"
      end
      code << "    default: return -1;\n"
      code << "    }\n"
    end
    code << "  default: return -1;\n"
    code << "  }\n"
    code << "}\n"
=end
    return code
  end

  def generate_ops_prototypes(methods)
    str = ""
    str << "namespace rubinius {\n"
    str << "  class VMMethod;\n"
    str << "  class Task;\n"
    str << "  class MethodContext;\n"
    str << "}\n"

    str = "extern \"C\" {\n"
    methods.each do |impl|
      str << impl.signature << ";\n"
    end
    str << "}\n"
    return str
  end

  def generate_implementation_info
    str = ""
    size = Rubinius::InstructionSet.opcodes.size
    str << "const Implementation* implementation(int op) {\n"
    str << "static Implementation implementations[] = {\n"
    Rubinius::InstructionSet.opcodes.each do |ins|
      str << "{ (void*)op_#{ins.opcode.to_s}, \"op_#{ins.opcode.to_s}\" },\n"
    end

    str << " { NULL, NULL} };\n"
    str << " if(op >= #{size}) return NULL;\n"
    str << " return &implementations[op]; }\n"
    str << "Status check_status(int op) {\n"
    str << "static Status check_status[] = {\n"
    methods = decode_methods()
    methods.each do |impl|
      if [:return, :raise].include?(impl.name.flow)
        str << "Terminate,\n"
      elsif impl.custom_continue?
        str << "MightReturn,\n"
      else
        str << "Unchanged,\n"
      end
    end
    str << " Unchanged };\n"
    str << "if(op >= #{size}) return Unchanged;\n"
    str << "return check_status[op]; }"
    str << "\n"
    str << generate_superinst_finder()
    str
  end

  # Generate header information for instruction functions and other
  # info.
  #
  def generate_names_header(methods)
    str = "static const char *get_instruction_name(int op);\n"

    str << "typedef enum {\n"
    methods.each do |impl|
      ins = impl.name
      str << "insn_#{ins.opcode.to_s} = #{ins.bytecode},\n"
    end
    str << "} instruction_names;\n"

    str << "const static unsigned int cTotal = #{methods.size};\n"

    str
  end
end
