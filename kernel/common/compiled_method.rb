# TODO - Comment!!!

module Rubinius
  class CompiledMethod < Executable
    # Any CompiledMethod with this value in it's serial slot
    # is expected to be the default, kernel version
    KernelMethodSerial = 47

    # Ivars: instance_variables, primitive, serial, name, iseq, stack_size,
    # local_count, required_args, total_args, splat, literals, exceptions,
    # lines, file, compiled, scope
    ##
    # This is runtime hints, added to the method by the VM to indicate how it's
    # being used.

    attr_accessor :hints
    attr_accessor :__ivars__
    attr_accessor :name
    attr_accessor :iseq
    attr_accessor :stack_size
    attr_accessor :local_count
    attr_accessor :required_args
    attr_accessor :total_args
    attr_accessor :splat
    attr_accessor :literals
    attr_accessor :exceptions
    attr_accessor :lines
    attr_accessor :file
    attr_accessor :local_names
    attr_accessor :scope

    def ==(other)
      return false unless other.kind_of?(CompiledMethod)
      @primitive == other.primitive and
        @name == other.name and
        @iseq == other.iseq and
        @stack_size == other.stack_size and
        @local_count == other.local_count and
        @required_args == other.required_args and
        @total_args == other.total_args and
        @splat == other.splat and
        @literals == other.literals and
        @exceptions == other.exceptions and
        @lines == other.lines and
        @file == other.file and
        @local_names == other.local_names
    end

    def inspect
      "#<#{self.class.name} #{@name} file=#{@file}>"
    end

    def from_string(bc, lcls, req)
      @iseq = bc
      @primitive = nil
      @local_count = lcls
      @literals = Tuple.new(0)
      @exceptions = nil
      @lines = nil
      @file = nil
      @name = nil
      @path = nil
      @required_args = req
      return self
    end

    def inherit_scope(other)
      @scope = other.scope
    end

    # Invoke method directly.
    #
    # @note The explicit block argument is unnecessary, but
    #       present for completeness.
    #
    def activate(recv, mod, args, locals = nil, &prc)
      Ruby.primitive :compiledmethod_activate
      raise PrimitiveFailure, "Unable to call #{@name} on #{recv.inspect}"
    end

    def set_breakpoint(ip)
      Ruby.primitive :compiledmethod_set_breakpoint
      raise ArgumentError, "Unable to set breakpoint on #{inspect} at invalid bytecode address #{ip}"
    end

    def clear_breakpoint(ip)
      Ruby.primitive :compiledmethod_clear_breakpoint
      raise ArgumentError, "Unable to clear breakpoint on #{inspect} at invalid bytecode address #{ip}"
    end

    def breakpoint?(ip)
      Ruby.primitive :compiledmethod_is_breakpoint
      raise ArgumentError, "Unable to retrieve breakpoint status on #{inspect} at bytecode address #{ip}"
    end

    # Accessor for a hash of filenames (as per $" / $LOADED_FEATURES) to the
    # script CompiledMethod.
    def self.scripts
      @scripts ||= {}
    end

    # Helper function for searching for a CM given a file name; applies similar
    # search and path expansion rules as load/require, so that the full path to
    # the file need not be specified.
    def self.script_for_file(filename)
      if cm = self.scripts[filename]
        return cm
      end
      # ./ ../ ~/ /
      if filename =~ %r{\A(?:(\.\.?)|(~))?/}
        if $2    # ~
          filename.slice! '~/'
          return scripts["#{ENV['HOME']}/#{filename}"]
        else    # . or ..
          return scripts["#{File.expand_path filename}"]
        end
      # Unqualified
      else
        scripts = self.scripts
        $LOAD_PATH.each do |dir|
          if cm = scripts["#{dir}/#{filename}"]
            return cm
          end
        end
      end
      nil
    end

    class Script
      attr_accessor :path
    end

    def as_script(script=nil)
      script ||= CompiledMethod::Script.new
      yield script if block_given?

      # Setup the scoping.
      ss = StaticScope.new(Object)
      ss.script = script
      @scope = ss

      mc = MAIN.metaclass
      mc.method_table.store :__script__, self, :public
      compile
      VM.reset_method_cache :__script__
      MAIN.__script__
    end

    def line_from_ip(ip)
      return -1 unless @lines
      return 0 if @lines.size < 2

      low = 0
      high = @lines.size / 2 - 1

      while low <= high
        # the chance that we're going from a fixnum to a bignum
        # here is low, but we still try to prevent that.
        mid = low + ((high - low) / 2)

        line_index = mid * 2 + 1

        if ip < @lines.at(line_index - 1)
          high = mid - 1
        elsif ip >= @lines.at(line_index + 1)
          low = mid + 1
        else
          return @lines.at(line_index)
        end
      end

      @lines.at(@lines.size - 2)
    end

    # Returns the address (IP) of the first instruction in this CompiledMethod
    # that is on the specified line, or the address of the first instruction on
    # the next code line after the specified line if there are no instructions
    # on the requested line.
    # This method only looks at instructions within the current CompiledMethod;
    # see #locate_line for an alternate method that also searches inside the child
    # CompiledMethods.

    def first_ip_on_line(line)
      i = 1
      total = @lines.size
      while i < total
        cur_line = @lines.at(i)
        if cur_line >= line
          return @lines.at(i-1)
        end

        i += 2
      end

      return -1
    end

    def first_line
      @lines.each do |ent|
        return ent[2] if ent[2] > 0
      end

      return -1
    end

    def is_block?
      @name =~ /__(?:(?:\w|_)+)?block__/
    end

    def describe
      str = "method #{@name}: #{@total_args} arg(s), #{@required_args} required"
      if @splat
        str << ", splatted."
      end
      str
    end

    # Convenience method to return an array of the child CompiledMethods from
    # this CompiledMethod's literals.

    def child_methods
      literals.select {|lit| lit.kind_of? CompiledMethod}
    end

    # Convenience method to return an array of the SendSites from
    # this CompiledMethod's literals.

    def send_sites
      literals.select {|lit| lit.kind_of? SendSite}
    end

    # Locates the CompiledMethod and instruction address (IP) of the first
    # instruction on the specified line. This method recursively examines child
    # compiled methods until an exact match for the searched line is found.
    # It returns both the matching CompiledMethod and the IP of the first
    # instruction on the requested line, or nil if no match for the specified line
    # is found.
    # TODO: Update this to work with new lines representation

    def locate_line(line, cm=self)
      cm.lines.each do |t|
        if (l = t.at(2)) == line
          # Found target line - return first IP
          return cm, t.at(0)
        elsif l > line
          break
        end
      end
      # Didn't find line in this CM, so check if a contained
      # CM encompasses the line searched for
      cm.child_methods.each do |child|
        if res = locate_line(line, child)
          return res
        end
      end

      # No child method is a match - fail
      return nil
    end

    ##
    # Decodes the instruction sequence that is represented by this compileed
    # method. Delegates to InstructionSequence to do the instruction decoding,
    # but then converts opcode literal arguments to their actual values by looking
    # them up in the literals tuple.
    # Takes an optional bytecodes argument representing the bytecode that is to
    # be decoded using this CompiledMethod's locals and literals. This is provided
    # for use by the debugger, where the bytecode sequence to be decoded may not
    # exactly match the bytecode currently held by the CompiledMethod, typically
    # as a result of substituting yield_debugger instructions into the bytecode.
    def decode(bytecodes = @iseq)
      stream = bytecodes.decode(false)
      ip = 0
      args_reg = 0
      stream.map! do |inst|
        instruct = Instruction.new(inst, self, ip, args_reg)
        ip += instruct.size
        if instruct.opcode == :set_args
          args_reg = 0
        elsif instruct.opcode == :cast_array_for_args
          args_reg = instruct.args.first
        end
        instruct
      end

      # Add a convenience method to the array containing the decoded instructions
      # to convert an IP address to the index of the corresponding instruction
      def stream.ip_to_index(ip)
        if ip < 0 or ip > last.ip
          raise ArgumentError, "IP address is outside valid range of 0 to #{last.ip} (got #{ip})"
        end
        each_with_index do |inst, i|
          return i if ip <= inst.ip
        end
      end
      stream
    end

    ##
    # Calculates the minimum stack size required for this method.
    def min_stack_size
      require 'compiler/stack'
      sdc = Compiler::StackDepthCalculator.new(@iseq)
      sdc.run
    end

    # Graphs the control flow of this method
    def graph_control(file, open_now=false)
      require 'compiler/blocks'
      require 'compiler/blocks_graph'

      be = Compiler::BlockExtractor.new(@iseq)
      entry = be.run
      grapher = Compiler::BlockGrapher.new(entry)
      grapher.run(file)

      # :) for OS X
      if open_now
        `open #{file}`
      end

      file
    end

    def arity()
      if @required_args == @total_args and
         @splat.nil?
        @required_args
      else
        -(@required_args + 1)
      end
    end

    # Represents virtual machine's CPU instruction.
    # Instructions are organized into instruction
    # sequences known as iSeq, forming body
    # of CompiledMethods.
    #
    # To generate VM opcodes documentation
    # use rake doc:vm task.
    class Instruction
      def initialize(inst, cm, ip, args_reg)
        @op = inst[0]
        @args = inst[1..-1]
        @comment = nil
        @args.each_index do |i|
          case @op.args[i]
          when :literal
            @args[i] = cm.literals[@args[i]]
          when :local
            # TODO: Blocks should be able to retrieve local names as well,
            # but need access to method corresponding to home context
            if cm.local_names and cm.name != :__block__
              @comment = cm.local_names[args[i]].to_s
            end
          when :block_local
            # TODO: Blocks should be able to retrieve enclosing block local names as well,
            # but need access to static scope
            @args[i] = cm.local_names[args[i]] if cm.local_names and args[0] == 0
          end
        end
        @ip = ip
        @line = cm.line_from_ip(ip)

        @stack_consumed = calculate_stack_usage(@op.stack_consumed, args_reg)
        @stack_produced = calculate_stack_usage(@op.stack_produced)
      end

      # Instruction pointer
      attr_reader :ip
      attr_reader :line

      ##
      # Returns the OpCode object

      # Associated OptCode instance.
      def instruction
        @op
      end

      ##
      # Returns the symbol representing the opcode for this instruction.

      def opcode
        @op.opcode
      end

      ##
      # Returns an array of 0 to 2 arguments, depending on the opcode.

      def args
        @args
      end

      def size
        @args.size + 1
      end

      ##
      # Returns the stack operands consumed by this instruction, as well as a flag
      # indicating whether this is an exact value (true) or a minimum (false).
      attr_reader :stack_consumed

      ##
      # Returns the stack operands produced by this instruction, as well as a flag
      # indicating whether this is an exact value (true) or a minimum (false).
      attr_reader :stack_produced

      ##
      # Calculate the stack usage (pushes or pops) of this instruction.

      def calculate_stack_usage(code, args_reg=0)
        usage = code
        if code < 0
          usage = 0
          # Stack usage depends on opcode args
          code *= -1
          arg, code = code.divmod(10)
          if arg >= 1 and arg <= 2
            # Opcode consumes/produces a multiple of the value in the specified
            # opcode arg
            usage += args[arg-1]
          elsif arg == 3
            # Opcode consumes number of args specified in args register
            usage += args_reg
          end
          usage += code
        end
        return usage
      end

      def to_s
        str = "%04d:  %-27s" % [@ip, opcode]
        str << @args.map{|a| a.inspect}.join(', ')
        if @comment
          str << "    # #{@comment}"
        end

        return str
      end
    end
  end
end
