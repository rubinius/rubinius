# A simple parser to transform a description of the VM instructions into
# various implementation files.
#
# The parser recognizes three forms presently:
#
#   1. define NAME <value>
#   2. section "Some description"
#   3. instruction opcode_name(arg1, arg2) [ consumed -- produced ]
#        body
#      end
#
# Once the forms are parsed, each type (a subclass of Definition) implements
# methods to emit its form in the context of a particular generator. In other
# words, the generated files are produced by a set of methods, each of which
# simply iterate the array of definitions in order calling a method for that
# generator.
#
# For example, to generate the opcode definitions file, the #opcode_definition
# method is called on each definition parsed from the 'instructions.def' file.
#
# The format of the instruction definition needs addition explanation. The
# instructions can take 0, 1, or 2 arguments. The arguments are listed in
# parenthesis after the instruction name.
#
# Each instruction has a stack effect, or the stack items consumed and the
# stack items produced. These are described in the square brackets after the
# argument list a la Forth. The '--' symbol separates the consumed from the
# produced.
#
# The names of the stack items are purely for documentation except in the
# special case of an item named like '+count'. This special form refers to one
# of the instruction's arguments and means that the total number of stack
# items consumed is the sum of the explicit stack items listed and the value
# of the instruction's argument.
#
# For example, the 'make_array' instruction consumes 'count' items on the
# stack, but no additional items. While the 'send_stack' instruction consumes
# the 'receiver' and 'count' additional items.
#
# Finally, for instructions that alter the normal sequential flow of execution,
# the type of control flow change will be specified with a hashrocket e.g.
# '=> branch'. The control flow behavior defaults to next, but for those
# instructions where control is (or may) change, one of the following flow
# types must be specified:
#   branch - for instructions that branch within the same method
#   send   - for instructions that invoke another method
#   yield  - for instructions that yield to a block
#   return - for instructions that return to the calling method
#   raise  - for instructions that invoke exception handling

class InstructionParser
  # Processes an opcode definition from the instruction parser into a method
  # called by the bytecode compiler's Generator to emit the instruction
  # stream.
  class GeneratorMethod
    def initialize(parser, file, opcode)
      @parser        = parser
      @file          = file
      @opcode        = opcode
      @method_name   = nil
      @before_stream = nil
      @after_stream  = nil
      @before_stack  = nil
      @after_stack   = nil
    end

    def method_name
      @method_name || @opcode.name
    end

    def process
      __send__ :"process_#{@opcode.name}"
    end

    # Define the method signature.
    def method_signature
      case @opcode.arguments.size
      when 0
        @file.puts "      def #{method_name}"
      when 1
        @file.puts "      def #{method_name}(arg1)"
      when 2
        @file.puts "      def #{method_name}(arg1, arg2)"
      when 3
        @file.puts "      def #{method_name}(arg1, arg2, arg3)"
      end
    end

    # Emit find_literal for literal methods.
    def method_find_literal
      @opcode.arguments.each_with_index do |arg, index|
        if arg == "literal"
          @file.puts "        arg#{index+1} = find_literal(arg#{index+1})"
        elsif arg == "count"
          @file.puts "        arg#{index+1} = Integer(arg#{index+1})"
        end
      end
    end

    # Append to the opcode stream.
    def method_append_stream
      case @opcode.arguments.size
      when 0
        @file.puts "        @stream << #{@opcode.bytecode}"
      when 1
        @file.puts "        @stream << #{@opcode.bytecode} << arg1"
      when 2
        @file.puts "        @stream << #{@opcode.bytecode} << arg1 << arg2"
      when 3
        @file.puts "        @stream << #{@opcode.bytecode} << arg1 << arg2 << arg3"
      end
      @file.puts "        @ip += #{@opcode.arguments.size + 1}"
    end

    # Calculate the full stack affect of this opcode.
    def method_stack_effect
      read = @opcode.static_read_effect
      write = @opcode.static_write_effect

      if @opcode.extra
        if read > 0
          read = "arg#{@opcode.extra+1}+#{read}"
        else
          read = "arg#{@opcode.extra+1}"
        end
      end

      if @opcode.produced_extra
        if write > 0
          write = "(arg#{@opcode.produced_extra+1} * #{@opcode.produced_times})+#{write}"
        else
          write = "(arg#{@opcode.produced_extra+1} * #{@opcode.produced_times})"
        end
      end

      @file.puts "        @current_block.add_stack(#{read}, #{write})"
    end

    def method_close
      @file.puts "        @instruction = #{@opcode.bytecode}"
      @file.puts "      end"
      @file.puts ""
    end

    def method_definition
      method_signature
      @before_stream.call if @before_stream
      method_find_literal
      method_append_stream
      @after_stream.call if @after_stream
      @before_stack.call if @before_stack
      method_stack_effect
      @after_stack.call if @after_stack
      method_close
    end

    def method_missing(sym, *args)
      method_definition
    end

    # Basic block creation, closing, transitions
    def bb_unconditional_branch
      @file.puts "        @current_block.left = arg1.basic_block"
      @file.puts "        @current_block.close"
      @file.puts "        @current_block = new_basic_block"
    end

    def bb_conditional_branch
      @file.puts "        @current_block.left = arg1.basic_block"
      @file.puts "        @current_block.close"
      @file.puts "        block = new_basic_block"
      @file.puts "        @current_block.right = block"
      @file.puts "        @current_block = block"
    end

    def bb_exit(check_stack)
      @file.puts "        @current_block.close #{check_stack}"
      @file.puts "        @current_block = new_basic_block"
    end

    # Categories of instructions. These could be done based on the control
    # flow attributes added to the instructions for the debugger, but that
    # would require more complicated parsing and contortions to work around
    # potential special cases. Instead, the simple #process_<opcode> method
    # is used and redundancy is factored out as needed.

    def unconditional_branch
      @before_stream = lambda { @file.puts "        location = @ip + 1" }
      @after_stream = lambda { @file.puts "        arg1.used_at location" }
      @after_stack = lambda { bb_unconditional_branch }
      method_definition
    end

    def conditional_branch
      @before_stream = lambda { @file.puts "        location = @ip + 1" }
      @after_stream = lambda { @file.puts "        arg1.used_at location" }
      @after_stack = lambda { bb_conditional_branch }
      method_definition
    end

    def unconditional_exit(check_stack=false)
      @after_stack = lambda { bb_exit check_stack }
      method_definition
    end

    # Specific instruction methods

    def process_push_literal
      @method_name = :emit_push_literal
      method_signature
      method_append_stream
      method_stack_effect
      method_close
    end

    def process_create_block
      method_signature
      @file.puts "        arg1 = add_generator(arg1)"
      method_append_stream
      method_stack_effect
      method_close
    end

    def process_goto
      unconditional_branch
    end

    def process_goto_if_false
      conditional_branch
    end

    def process_goto_if_true
      conditional_branch
    end

    def process_goto_if_nil
      conditional_branch
    end

    def process_goto_if_not_nil
      conditional_branch
    end

    def process_goto_if_undefined
      conditional_branch
    end

    def process_goto_if_not_undefined
      conditional_branch
    end

    def process_goto_if_equal
      conditional_branch
    end

    def process_goto_if_not_equal
      conditional_branch
    end

    def process_setup_unwind
      conditional_branch
    end

    def process_ret
      unconditional_exit true
    end

    def process_raise_exc
      unconditional_exit
    end

    def process_raise_return
      unconditional_exit true
    end

    def process_ensure_return
      unconditional_exit true
    end

    def process_raise_break
      unconditional_exit
    end

    def process_reraise
      unconditional_exit
    end

    def process_cast_array
      method_signature
      make_array = @parser.find_opcode "make_array"
      @file.puts "        unless @instruction == #{@opcode.bytecode} or @instruction == #{make_array.bytecode}"
      @file.puts "          @stream << #{@opcode.bytecode}"
      @file.puts "          @ip += 1"
      @file.puts "        end"
      method_close
    end
  end

  class ParseError < Exception; end

  class Definition
    attr_accessor :bytecode

    def opcode_definition(file)
    end

    def opcode_method(parser, file)
    end

    def opcode_name
      ""
    end

    def opcode_width
    end

    def opcode_prototype
    end

    def opcode_location
    end

    def opcode_implementation(file)
    end

    def opcode_enum
    end

    def opcode_define
    end

    def opcode_visitor
    end

    def opcode_stack_effect(file)
    end

    def opcode_documentation(file)
    end
  end

  class Section < Definition
    attr_reader :heading

    def initialize(line, doc)
      @heading = line.strip[1..-2]
    end

    def opcode_definition(file)
      file.print "\n    # "
      file.puts  @heading
    end
  end

  class Define < Definition
    attr_reader :name, :value

    def initialize(line, doc)
      @name, @value = line.strip.split
    end

    def opcode_definition(file)
      file.puts
      file.puts "    #{@name} = #{@value}"
      file.puts
    end

    def opcode_define
      "#define #{@name} #{@value}"
    end
  end

  class InstructionDocumentation
    def initialize(instruction)
      @instruction = instruction
      @description = []
      @consumed = nil
      @produced = nil
      @see_also = []
      @notes = []
      @example = []
    end

    def parse(text)
      section = []

      text.each do |line|
        case line
        when /\[Description\]/
          section = @description
        when /\[Stack Before\]/
          section = @consumed = []
        when /\[Stack After\]/
          section = @produced = []
        when /\[See Also\]/
          section = @see_also
        when /\[Notes\]/
          section = @notes
        when /\[Example\]/
          section = @example
        else
          section << line[1..-1]
        end
      end

      self
    end

    def consumed
      # TODO: account for extra consumed
      @consumed || @instruction.consumed + ["..."]
    end

    def produced
      # TODO: account for extra produced
      @produced || @instruction.produced + ["..."]
    end

    def format(file)
      file.puts %[<h3><a class="instruction" name="#{name}">#{name}(#{arguments})</a></h3>]
    end

    def stack_effect(file)
      c = consumed
      p = produced
      n = c.size > p.size ? c.size : p.size

      file.puts %[\n<table class="stack_effect">]
      file.puts "<thead>"
      file.puts "<tr><th>Before</th><th>After</th></tr>"
      file.puts "</thead>"
      file.puts "<tbody>"
      n.times do |i|
        file.puts "<tr><td>#{c[i]}</td><td>#{p[i]}</td></tr>"
      end
      file.puts "</tbody>"
      file.puts "</table>"
    end

    def description(file)
      file.puts ""
      file.puts @description
      file.puts ""
    end

    def example(file)
      return if @example.empty?

      file.puts ""
      file.puts '#### Example'
      file.puts @example
      file.puts ""
    end

    def see_also(file)
      return if @see_also.empty?

      file.puts "\n<h4>See Also</h4>"
      file.puts %[<ul class="insn_cross_ref">]
      @see_also.each do |x|
        x = x.strip
        file.puts %[<li><a href="\##{x}">#{x}</a></li>]
      end
      file.puts "</ul>"
    end

    def notes(file)
      return if @notes.empty?

      file.puts ""
      file.puts '#### Notes'
      file.puts @notes
      file.puts ""
    end

    def name
      @instruction.name
    end

    def arguments
      @instruction.arguments.join ", "
    end

    def render(file)
      format file
      description file
      stack_effect file
      example file
      notes file
      see_also file
    end
  end

  class Instruction < Definition
    attr_reader :name, :arguments, :consumed, :extra, :produced,
                :produced_extra, :effect, :body, :control_flow, :produced_times
    attr_accessor :bytecode

    @bytecodes = 0

    def self.bytecodes
      @bytecodes
    end

    def self.bytecodes=(total)
      @bytecodes = total
    end

    def initialize(parser, header, doc)
      @parser = parser
      @file = parser.file
      @header = header
      @body = []
      @doc = InstructionDocumentation.new(self).parse(doc)
      @extra = nil
      @produced_extra = nil
      @bytecode = 0
      @control_flow = :next
    end

    def parse
      parse_header
      parse_body
    end

    def parse_header
      m = @header.match(/(\w+)\(([^)]*)\) \[ ([\w+ ]*)-- ([\w+ ]*)\]( =>\s*(\w+))?/)
      unless m
        raise ParseError, "invalid instruction header '#{@header}' at #{@file.lineno}"
      end

      @name = m[1]

      @arguments = m[2].strip.split

      consumed = m[3].strip.split
      last = consumed.last
      if last and last[0] == ?+
        argument = last[1..-1]
        unless @extra = @arguments.index(argument)
          raise ParseError, "no argument named '#{arg}' at #{@file.lineno}"
        end
        consumed.pop
      end
      @consumed = consumed

      @produced = m[4].strip.split
      last = @produced.last
      if last and last[0] == ?+
        @produced_times = 0

        while last[0] == ?+
          @produced_times += 1
          last = last[1..-1]
        end

        unless @produced_extra = @arguments.index(argument)
          raise ParseError, "no argument named '#{arg}' at #{@file.lineno}"
        end
        @produced.pop
      end

      @effect = @produced.size - @consumed.size

      @control_flow = m[6].to_sym if m[6]
    end

    def static_read_effect
      @consumed.size
    end

    def static_write_effect
      @produced.size
    end

    def parse_body
      while line = @file.gets
        return if line =~ /^end\b/
        @body << line
      end
    end

    def opcode_definition(file)
      if @extra
        consumed = "[#{@consumed.size},#{@extra+1}]"
      else
        consumed = "#{@consumed.size}"
      end

      if @produced_extra
        produced = "[#{@produced.size}, #{@produced_extra+1}, #{@produced_times}]"
      else
        produced = "#{@produced.size}"
      end

      file.print "    opcode %2d, :%-28s " % [@bytecode, @name + ","]

      stack = "[#{consumed}, #{produced}],"
      file.print ":stack => %-12s" % stack

      syms = @arguments.map { |x| ":#{x}" }.join(", ")
      args = "[#{syms}],"
      file.print ":args => %-20s" % args

      file.puts  "              :control_flow => :#{@control_flow}"
    end

    def opcode_method(parser, file)
      GeneratorMethod.new(parser, file, self).process
    end

    def opcode_name
      "op_#{@name}"
    end

    def opcode_width
      @arguments.size + 1
    end

    def opcode_prototype
      name = opcode_name
      indent = " " * (name.size + 9)
      prototype = "Object* #{name}(rubinius::VM* state, rubinius::MachineCode* mcode,
#{indent}rubinius::InterpreterCallFrame* const call_frame,
#{indent}rubinius::MachineCode::InterpreterState& is"

      unless @arguments.empty?
        prototype << ",\n#{indent}"
        prototype << @arguments.map { |a| "int #{a}" }.join(", ")
      end

      prototype << ");\n"
    end

    def opcode_enum
      "insn_#{@name}"
    end

    def opcode_location
      "op_impl_#{@name}"
    end

    def opcode_implementation(file)
      file.puts "  #{opcode_location}: {"

      @arguments.each do |arg|
        file.puts "    intptr_t #{arg} = next_int;"
      end

      @body.each do |line|
        file.puts "  #{line}".rstrip
      end
    end

    def instruction_implementation(file)
      @arguments.each do |arg|
        file.puts "  intptr_t #{arg} = next_int;"
      end

      @body.each do |line|
        file.puts line.rstrip
      end
    end

    def opcode_visitor
      "HANDLE_INST#{@arguments.size}(#{@bytecode}, #{@name});"
    end

    def opcode_stack_effect(file)
      file.puts "case InstructionSequence::insn_#{@name}:"
      read = static_read_effect
      write = static_write_effect

      if @extra
        if read > 0
          read = "operand#{@extra+1}+#{read}"
        else
          read = "operand#{@extra+1}"
        end
      end

      if @produced_extra
        if write > 0
          write = "(operand#{@produced_extra+1} * #{@produced_times})+#{write}"
        else
          write = "(operand#{@produced_extra+1} * #{@produced_times})"
        end
      end

      file.puts "if(read_effect)  { *read_effect  = (#{read}); }"
      file.puts "if(write_effect) { *write_effect = (#{write}); }"

      file.puts "return (#{write}) - (#{read});"
    end

    def opcode_documentation(file)
      @doc.render file
    end
  end

  # InstructionParser methods

  attr_reader :file, :objects

  def initialize(filename)
    @filename = filename
    @objects = []
    @parsed = false
    @blank_line = false
    @doc = []
  end

  def process_define(parser, line, doc)
    parser.objects << Define.new(line, doc)
  end

  def process_section(parser, line, doc)
    parser.objects << Section.new(line, doc)
  end

  def process_instruction(parser, line, doc)
    insn = Instruction.new parser, line, doc
    insn.parse
    parser.objects << insn
  end

  def parse
    return if @parsed

    File.open @filename, "rb" do |file|
      @file = file

      while line = @file.gets
        if index = line.index("#")
          if index == 0
            if @blank_line
              @doc.clear
              @blank_line = false
            end

            @doc << line
          end

          line = line[0, index]
        elsif line.strip.empty?
          @blank_line = true
          next
        end

        if m = line.match(/^(instruction|section|define) +(.*)$/)
          send :"process_#{m[1]}", self, m[2], @doc
          @doc.clear
        end
      end
    end

    # Assign Instruction bytecode index
    total_opcodes = 0
    objects.sort! { |a, b| a.opcode_name <=> b.opcode_name }
      .select { |x| x.kind_of? Instruction }
      .each.with_index { |x, i| total_opcodes = x.bytecode = i }
    Instruction.bytecodes = total_opcodes + 1

    @parsed = true
  end

  def find_opcode(name)
    @objects.find { |x| x.kind_of? Instruction and x.name == name }
  end

  # Generated output methods

  def generate_opcodes(filename)
    File.open filename, "wb" do |file|
      file.puts "# *** This file is generated by InstructionParser ***"
      file.puts
      file.puts "module Rubinius"
      file.puts "  class InstructionSet"

      objects.select { |obj| obj.kind_of? Instruction or obj.kind_of? Define }
        .each { |obj| obj.opcode_definition file }

      file.puts "  end"
      file.puts "end"
    end
  end

  def generate_generator_methods(filename)
    File.open filename, "wb" do |file|
      file.puts "# *** This file is generated by InstructionParser ***"
      file.puts
      file.puts "module CodeTools"
      file.puts "  module GeneratorMethods"

      objects.each { |obj| obj.opcode_method self, file }

      file.puts "  end"
      file.puts "end"
    end
  end

  def generate_names(filename)
    File.open filename, "wb" do |file|
      file.puts "const char *rubinius::InstructionSequence::get_instruction_name(int op) {"
      file.puts "  static const char instruction_names[] = {"

      offset = 0
      offsets = []
      objects.each do |obj|
        name = obj.opcode_name
        unless name.empty?
          file.puts "   \"#{name}\\0\""
          offsets << offset
          offset += name.size + 1
        end
      end

      file.puts "  };"
      file.puts

      file.puts  "  static const unsigned int instruction_name_offsets[] = {"
      file.print "    "
      file.puts offsets.join(",\n    ")
      file.puts "  };"
      file.puts

      file.puts "  return instruction_names + instruction_name_offsets[op];"
      file.puts "}"
    end
  end

  def generate_names_header(filename)
    File.open filename, "wb" do |file|
      file.puts "static const char *get_instruction_name(int op);"
      file.puts "typedef enum {"

      objects.each do |obj|
        if enum = obj.opcode_enum
          file.puts "  #{enum} = #{obj.bytecode},"
        end
      end

      file.puts "} instruction_names;"
      file.puts "const static unsigned int cTotal = #{Instruction.bytecodes};"
    end
  end

  def generate_prototypes(filename)
    File.open filename, "wb" do |file|
      file.puts "extern \"C\" {"

      objects.each do |obj|
        if prototype = obj.opcode_prototype
          file.puts prototype
        end
      end

      file.puts "} // extern \"C\""
    end
  end

  def generate_interpreter_tests(filename)
    insns = objects.select { |x| x.kind_of? Instruction }
    File.open filename, "wb" do |file|
      file.puts <<-EOD
\#include "machine/test/test.hpp"

\#include "call_frame.hpp"
\#include "object_utils.hpp"

\#include "interpreter.hpp"

class TestInterpreter : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }
      EOD

      insns.each do |obj|
        file.puts <<-EOD

  void test_interpreter_#{obj.name}() {
    CallFrame* call_frame = ALLOCA_CALL_FRAME(1);
    StackVariables* scope = ALLOCA_STACKVARIABLES(0);
    setup_call_frame(call_frame, scope, 1);

    intptr_t opcodes[#{obj.arguments.size+2}];
    opcodes[0] =
      reinterpret_cast<intptr_t>(instructions::data_#{obj.name}.interpreter_address);#{obj.arguments.map.with_index { |x, i| "\n    opcodes[#{i+1}] = 0; // #{x}" }.join}
    opcodes[#{obj.arguments.size+1}] =
      reinterpret_cast<intptr_t>(instructions::data_ret.interpreter_address);

    // TODO: instructions
    // interpreter::#{obj.name}(state, call_frame, opcodes);

    TS_ASSERT(true);
    // TS_ASSERT_EQUALS(call_frame->ip(), instructions::data_#{obj.name}.width);
  }
        EOD
      end

      file.puts "};"
    end
  end

  def generate_instruction_data(filename)
    File.open filename, "wb" do |file|
      file.puts '#ifndef RBX_INSTRUCTIONS_DATA_HPP'
      file.puts '#define RBX_INSTRUCTIONS_DATA_HPP'
      file.puts ""
      file.puts "namespace rubinius {"
      file.puts "  namespace instructions {"
      file.puts <<-EOD
    struct InstructionData {
      const char* name;
      const int id;
      const int width;
      const int read;
      const int read_arg0;
      const int read_arg1;
      const int read_arg2;
      const int write;
      const int write_arg0;
      const int write_arg1;
      const int write_arg2;
      intptr_t (*interpreter_address)(STATE, CallFrame*, intptr_t const[]);

      int consumed(intptr_t arg0, intptr_t arg1, intptr_t arg2) const {
        return read + (arg0*read_arg0) + (arg1*read_arg1) + (arg2*read_arg2);
      }

      int produced(intptr_t arg0, intptr_t arg1, intptr_t arg2) const {
        return write + (arg0*write_arg0) + (arg1*write_arg1) + (arg2*write_arg2);
      }

      int stack_effect(intptr_t arg0, intptr_t arg1, intptr_t arg2) const {
        return produced(arg0, arg1, arg2) - consumed(arg0, arg1, arg2);
      }
    };

      EOD

      objects.select { |x| x.kind_of? Instruction }.each do |obj|
        file.puts "    const InstructionData constexpr data_#{obj.name} = {"
        file.puts %[      #{obj.name.inspect}, #{obj.bytecode}, #{obj.opcode_width}, #{obj.static_read_effect}, #{obj.extra == 0 ? 1 : 0}, #{obj.extra == 1 ? 1 : 0}, #{obj.extra == 2 ? 1 : 0}, #{obj.static_write_effect}, (#{obj.produced_extra == 0 ? 1 : 0} * #{obj.produced_times || 0}), (#{obj.produced_extra == 1 ? 1 : 0} * #{obj.produced_times || 0}), (#{obj.produced_extra == 2 ? 1 : 0} * #{obj.produced_extra || 0}),\n      rubinius::interpreter::#{obj.name}]
        file.puts "    };"
      end

      file.puts "  }"
      file.puts "}"
      file.puts ""
      file.puts '#endif'
    end
  end

  def generate_instruction_data_array(filename)
    File.open filename, "wb" do |file|
      file.puts "namespace rubinius {"
      file.puts "  const instructions::InstructionData Interpreter::instruction_data[] = {"

      objects.each do |obj|
        file.puts "    rubinius::instructions::data_#{obj.name},"
      end

      file.puts "  };"
      file.puts "}"
    end
  end

  def generate_sizes(filename)
    File.open filename, "wb" do |file|
      file.puts "size_t width = 0;"
      file.puts "switch(op) {"

      objects.each do |obj|
        if width = obj.opcode_width
          file.puts "  case #{obj.bytecode}:"
          file.puts "    width = #{width}; break;"
        end
      end

      file.puts "}"
    end
  end

  def generate_locations(filename)
    File.open filename, "wb" do |file|
      file.puts "  static const void* insn_locations[] = {"

      objects.each do |obj|
        if location = obj.opcode_location
          file.puts "    &&#{location},"
        end
      end

      file.puts "    NULL"
      file.puts "  };"
    end
  end

  def generate_implementations(filename)
    File.open filename, "wb" do |file|
      file.puts "  DISPATCH;"

      objects.each do |obj|
        if obj.opcode_implementation(file)
          file.puts "    DISPATCH;"
          file.puts "  }"
        end
      end
    end
  end

  def generate_defines(filename)
    File.open filename, "wb" do |file|
      objects.each do |obj|
        if define = obj.opcode_define
          file.puts define
        end
      end
    end
  end

  def generate_visitors(filename)
    File.open filename, "wb" do |file|
      objects.each do |obj|
        if visitor = obj.opcode_visitor
          file.puts visitor
        end
      end
    end
  end

  def generate_stack_effects(filename)
    File.open filename, "wb" do |file|
      file.puts "static inline int stack_difference(opcode op,"
      file.puts "                                   opcode operand1 = 0,"
      file.puts "                                   opcode operand2 = 0,"
      file.puts "                                   opcode operand3 = 0,"
      file.puts "                                   int* read_effect = 0, int* write_effect = 0)"
      file.puts "{"
      file.puts "  switch(op) {"

      objects.each do |obj|
        obj.opcode_stack_effect file
      end

      file.puts "  }"
      file.puts "  abort();"
      file.puts "  return 0;"
      file.puts "}"
    end
  end

  def generate_documentation(filename)
    File.open filename, "wb" do |file|
      objects.each do |obj|
        obj.opcode_documentation file
      end
    end
  end
end
