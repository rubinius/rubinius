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
# the type of control flow change will be specified in curly braces, e.g.
# {branch}. The control flow behavior defaults to next, but for those
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
      end
      @file.puts "        @ip += #{@opcode.arguments.size + 1}"
    end

    # Calculate the full stack affect of this opcode.
    def method_stack_effect
      if @opcode.extra or @opcode.produced_extra

        total = nil

        if @opcode.extra
          variable = "-arg#{@opcode.extra+1}"
          if @opcode.effect == 0
            total = variable
          else
            total = "#{@opcode.effect}#{variable}"
          end
        end


        if @opcode.produced_extra
          variable = "(arg#{@opcode.produced_extra+1} * #{@opcode.produced_times})"

          if total
            total = "(#{total}) + #{variable}"
          else
            total = "#{opcode.effect} + #{variable}"
          end
        end

        @file.puts "        @current_block.add_stack(#{total})"
      elsif @opcode.effect != 0
        @file.puts "        @current_block.add_stack(#{@opcode.effect})"
      end
    end

    def method_close
      @file.puts "        @instruction = #{@opcode.bytecode}"
      @file.puts "      end"
      @file.puts ""
    end

    def method_definition
      method_signature
      @before_stream.call if @before_stream
      method_append_stream
      @after_stream.call if @after_stream
      @before_stack.call if @before_stack
      method_stack_effect
      @after_stack.call if @after_stack
      method_close
    end

    def literal_method
      @before_stream = lambda { @file.puts "        arg1 = find_literal arg1" }
      method_definition
    end

    def literal_count_method
      @before_stream = lambda do
        @file.puts "        arg1 = find_literal arg1"
        @file.puts "        arg2 = Integer(arg2)"
      end
      method_definition
    end

    def method_missing(sym, *args)
      method_definition
    end

    # Basic block creation, closing, transitions
    def bb_unconditional_branch
      @file.puts "        @current_block.add_edge arg1.basic_block"
      @file.puts "        @current_block.close"
      @file.puts "        @current_block = new_basic_block"
    end

    def bb_conditional_branch
      @file.puts "        @current_block.add_edge arg1.basic_block"
      @file.puts "        @current_block.close"
      @file.puts "        block = new_basic_block"
      @file.puts "        @current_block.add_edge block"
      @file.puts "        @current_block = block"
    end

    def bb_exit
      @file.puts "        @current_block.close"
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

    def unconditional_exit
      @after_stack = lambda { bb_exit }
      method_definition
    end

    # Specific instruction methods

    def process_goto
      unconditional_branch
    end

    def process_goto_if_false
      @method_name = :gif
      conditional_branch
    end

    def process_goto_if_true
      @method_name = :git
      conditional_branch
    end

    def process_setup_unwind
      conditional_branch
    end

    def process_ret
      unconditional_exit
    end

    def process_raise_exc
      unconditional_exit
    end

    def process_raise_return
      unconditional_exit
    end

    def process_ensure_return
      unconditional_exit
    end

    def process_raise_break
      unconditional_exit
    end

    def process_reraise
      unconditional_exit
    end

    def process_push_int
      method_signature

      # Integers greater than 256 are stored in the literals tuple.
      @file.puts <<EOM
        if arg1 > 2 and arg1 < 256
          @stream << #{@opcode.bytecode} << arg1
          @current_block.add_stack(1)
          @ip += 2
          @instruction = #{@opcode.bytecode}
        else
          case arg1
          when -1
            meta_push_neg_1
          when 0
            meta_push_0
          when 1
            meta_push_1
          when 2
            meta_push_2
          else
            push_literal arg1
          end
        end
      end

EOM
    end

    def process_push_literal
      @method_name = :emit_push_literal
      method_definition
    end

    def process_dup_top
      @method_name = :dup
      method_definition
    end

    def process_swap_stack
      @method_name = :swap
      method_definition
    end

    def process_push_const
      # unused right now
    end

    def process_find_const
      literal_method
    end

    def process_push_ivar
      literal_method
    end

    def process_set_ivar
      literal_method
    end

    def process_check_serial
      literal_count_method
    end

    def process_check_serial_private
      literal_count_method
    end

    def process_create_block
      @before_stream = lambda do
        @file.puts "        arg1 = add_literal arg1"
        @file.puts "        @generators << arg1"
      end
      method_definition
    end

    def process_invoke_primitive
      literal_count_method
    end

    def process_call_custom
      literal_count_method
    end

    def process_zsuper
      literal_method
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
    def opcode_definition(file)
    end

    def opcode_method(parser, file)
    end

    def opcode_name
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
  end

  class Section < Definition
    attr_reader :heading

    def initialize(line)
      @heading = line.strip[1..-2]
    end

    def opcode_definition(file)
      file.print "\n    # "
      file.puts  @heading
    end
  end

  class Define < Definition
    attr_reader :name, :value

    def initialize(line)
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

  class Instruction < Definition
    attr_reader :name, :bytecode, :arguments, :consumed, :extra,
                :produced, :produced_extra, :effect, :body, :control_flow,
                :produced_times

    def self.bytecodes
      @bytecodes
    end

    def self.bytecode
      @bytecodes ||= -1
      @bytecodes += 1
    end

    def initialize(parser, header)
      @parser = parser
      @file = parser.file
      @header = header
      @body = []
      @extra = nil
      @produced_extra = nil
      @bytecode = self.class.bytecode
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

        arguments = last[1..-1]
        unless @produced_extra = @arguments.index(argument)
          raise ParseError, "no argument named '#{arg}' at #{@file.lineno}"
        end
        @produced.pop
      end

      @effect = @produced.size - @consumed.size

      @control_flow = m[6].to_sym if m[6]
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
      prototype = "Object* #{name}(rubinius::VM* state, rubinius::VMMethod* vmm,
#{indent}rubinius::InterpreterCallFrame* const call_frame,
#{indent}rubinius::VMMethod::InterpreterState& is"

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

    def opcode_visitor
      "HANDLE_INST#{@arguments.size}(#{@bytecode}, #{@name});"
    end

    def opcode_stack_effect(file)
      file.puts "case InstructionSequence::insn_#{@name}:"
      if @extra
        if @produced_extra
          file.puts "  return (#{@effect} - operand#{@extra+1}) + (operand#{@produced_extra+1} * #{@produced_times});"
        else
          file.puts "  return #{@effect} - operand#{@extra+1};"
        end
      else
        if @produced_extra
          file.puts "  return #{@effect} + (operand#{@produced_extra+1} * #{@produced_times});"
        else
          file.puts "  return #{@effect};"
        end
      end
    end
  end

  # InstructionParser methods

  attr_reader :file, :objects

  def initialize(filename)
    @filename = filename
    @objects = []
    @parsed = false
  end

  def process_define(parser, line)
    parser.objects << Define.new(line)
  end

  def process_section(parser, line)
    parser.objects << Section.new(line)
  end

  def process_instruction(parser, line)
    insn = Instruction.new parser, line
    insn.parse
    parser.objects << insn
  end

  def parse
    return if @parsed

    File.open @filename, "r" do |file|
      @file = file

      while line = @file.gets
        if index = line.index("#")
          line = line[0, index]
        end

        next if line.empty?

        m = line.match(/^(instruction|section|define) +(.*)$/)

        send :"process_#{m[1]}", self, m[2] if m
      end
    end

    @parsed = true
  end

  def find_opcode(name)
    @objects.find { |x| x.kind_of? Instruction and x.name == name }
  end

  # Generated output methods

  def generate_opcodes(filename)
    File.open filename, "w" do |file|
      file.puts "# *** This file is generated by InstructionParser ***"
      file.puts
      file.puts "module Rubinius"
      file.puts "  class InstructionSet"

      objects.each { |obj| obj.opcode_definition file }

      file.puts "  end"
      file.puts "end"
    end
  end

  def generate_generator_methods(filename)
    File.open filename, "w" do |file|
      file.puts "# *** This file is generated by InstructionParser ***"
      file.puts
      file.puts "module Rubinius"
      file.puts "  module GeneratorMethods"

      objects.each { |obj| obj.opcode_method self, file }

      file.puts "  end"
      file.puts "end"
    end
  end

  def generate_names(filename)
    File.open filename, "w" do |file|
      file.puts "const char *rubinius::InstructionSequence::get_instruction_name(int op) {"
      file.puts "  static const char instruction_names[] = {"

      offset = 0
      offsets = []
      objects.each do |obj|
        if name = obj.opcode_name
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
    File.open filename, "w" do |file|
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
    File.open filename, "w" do |file|
      file.puts "extern \"C\" {"

      objects.each do |obj|
        if prototype = obj.opcode_prototype
          file.puts prototype
        end
      end

      file.puts "} // extern \"C\""
    end
  end

  def generate_sizes(filename)
    File.open filename, "w" do |file|
      file.puts "size_t width = 1;"
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
    File.open filename, "w" do |file|
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
    File.open filename, "w" do |file|
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
    File.open filename, "w" do |file|
      objects.each do |obj|
        if define = obj.opcode_define
          file.puts define
        end
      end
    end
  end

  def generate_visitors(filename)
    File.open filename, "w" do |file|
      objects.each do |obj|
        if visitor = obj.opcode_visitor
          file.puts visitor
        end
      end
    end
  end

  def generate_stack_effects(filename)
    File.open filename, "w" do |file|
      file.puts "static inline int stack_difference(opcode op,"
      file.puts "                                   opcode operand1 = 0,"
      file.puts "                                   opcode operand2 = 0)"
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
end
