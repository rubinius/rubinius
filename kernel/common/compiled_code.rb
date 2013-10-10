module Rubinius
  class CompiledCode < Executable

    ##
    # Any CompiledCode with this value in it's serial slot
    # is expected to be the default, kernel version
    KernelMethodSerial = 47

    ##
    # Ivars: instance_variables, primitive, serial, name, iseq, stack_size,
    # local_count, required_args, total_args, splat, literals, exceptions,
    # lines, file, compiled, scope
    ##
    # This is runtime hints, added to the method by the VM to indicate how it's
    # being used.

    attr_accessor :hints         # added by the VM to indicate how it's being used.
    attr_accessor :metadata      # [Tuple]   extra data
    attr_accessor :name          # [Symbol]  name of the method
    attr_accessor :iseq          # [Tuple]   instructions to execute
    attr_accessor :stack_size    # [Integer] size of stack at compile time
    attr_accessor :local_count   # [Integer] number of local vars
    attr_accessor :required_args # [Integer] number of required args
    attr_accessor :post_args     # [Integer] number of args after splat
    attr_accessor :total_args    # [Integer] number of total args
    attr_accessor :splat         # [Integer] POSITION of the splat arg
    attr_accessor :literals      # [Tuple]   tuple of the literals
    attr_accessor :lines         # [Tuple]   tuple of the lines where its found
    attr_accessor :file          # [Symbol]  the file where this comes from
    attr_accessor :local_names   # [Array<Symbol>] names of the local vars
    attr_accessor :scope

    ##
    # Compare this method with +other+. Instead of bugging out if +other+
    # isn't a {CompiledCode}, this returns +false+ immediately unless
    # we're comparing two apples, AKA, {CompiledCode}s. The methods have
    # to be the exact same in implementation, but their scoping (location)
    # can differ.
    #
    # For instance:
    #
    # (module A; def m; 5; end; end) == (def m; 5; end)
    #
    # and
    #
    # def monkey; 5; end
    #
    # and
    #
    # module B
    #   def monkey; 5; end
    # end
    #
    # would all be the same, despite their access to different ivars and scopes
    # (which {CompiledCode}s DO keep track of)
    #
    # @todo Make example (in method documentation) match reality
    # @param [Rubinius::CompiledCode] other the other part to compare
    # @param [Boolean]
    #
    def ==(other)
      return false unless other.kind_of? CompiledCode
      equivalent_body?(other) and
        @lines == other.lines and
        @name == other.name
    end

    def equivalent_body?(other)
      @primitive       == other.primitive     and
        @iseq          == other.iseq          and
        @stack_size    == other.stack_size    and
        @local_count   == other.local_count   and
        @required_args == other.required_args and
        @total_args    == other.total_args    and
        @splat         == other.splat         and
        block_index   == other.block_index    and
        @literals      == other.literals      and
        @file          == other.file          and
        @local_names   == other.local_names
    end

    ##
    # Returns the index of local +name+ or nil if there is no local.
    #
    def local_slot(name)
      slot = 0
      while slot < @local_count
        return slot if @local_names[slot] == name
        slot += 1
      end
    end

    ##
    # Stores the index of the block argument in the metadata storage.
    #
    # @param [Fixnum] position
    #
    def block_index=(position)
      if position
        add_metadata(:block_index, position)
      end
    end

    ##
    # @return [Fixnum|NilClass]
    #
    def block_index
      return get_metadata(:block_index)
    end

    ##
    # Return a human readable interpretation of this method.
    #
    # @return [String]
    def inspect
      "#<#{self.class.name} #{@name} file=#{@file}>"
    end

    ##
    # Make the method change its scope so that it can act as though
    # it's from somewhere else. You can pass in a method and +self+
    # will borrow its scope.
    #
    # @param [#scope] other the other method that has a scope we can borrow
    def inherit_scope(other)
      @scope = other.scope
    end

    ##
    # Set a breakpoint on +ip+. +obj+ can be any object. When the breakpoint
    # is hit +obj+ is sent to the debugger so it can distinguish which breakpoint
    # this is.
    #
    def set_breakpoint(ip, obj)
      Rubinius.primitive :compiledcode_set_breakpoint
      raise ArgumentError, "Unable to set breakpoint on #{inspect} at invalid bytecode address #{ip}"
    end

    ##
    # Erase a breakpoint at +ip+
    #
    def clear_breakpoint(ip)
      Rubinius.primitive :compiledcode_clear_breakpoint
      raise ArgumentError, "Unable to clear breakpoint on #{inspect} at invalid bytecode address #{ip}"
    end

    ##
    # Indicate if there is a breakpoint set at +ip+
    #
    def breakpoint?(ip)
      Rubinius.primitive :compiledcode_is_breakpoint
      raise ArgumentError, "Unable to retrieve breakpoint status on #{inspect} at bytecode address #{ip}"
    end

    class Script
      attr_accessor :compiled_code
      attr_accessor :file_path
      attr_accessor :data_path
      attr_accessor :eval_source

      def initialize(method, path=nil, for_eval=false)
        @compiled_code = method
        @file_path = path
        @for_eval = for_eval
        @eval_source = nil
        @main = false
      end

      def eval?
        @for_eval
      end

      def make_main!
        @main = true
      end

      def main?
        @main
      end
    end

    # Creates the Script instance for a toplevel compiled method.
    def create_script(wrap=false)
      script = CompiledCode::Script.new(self)

      # Setup the scoping.
      cs = ConstantScope.new(Object)
      cs.script = script

      if wrap
        @scope = ConstantScope.new(Module.new, cs)
      else
        @scope = cs
      end

      sc = Rubinius::Type.object_singleton_class(MAIN)
      sc.method_table.store :__script__, self, :public
      VM.reset_method_cache sc, :__script__

      script
    end

    def active_path
      if @scope and script = @scope.current_script
        if fp = script.file_path
          return fp.dup
        end
      end

      @file.to_s
    end

    def eval_source
      if @scope and script = @scope.current_script
        return script.eval_source
      end

      return nil
    end

    ##
    # Return the line of source code at +ip+.
    #
    # @param  [Fixnum] ip
    # @return [Fixnum] line
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

    ##
    # Returns the address (IP) of the first instruction in this CompiledCode
    # that is on the specified line, or the address of the first instruction on
    # the next code line after the specified line if there are no instructions
    # on the requested line.
    # This method only looks at instructions within the current CompiledCode;
    # see #locate_line for an alternate method that also searches inside the child
    # CompiledCodes.
    #
    # Optionally only consider ip's greater than +start+
    #
    # @return [Fixnum] the address of the first instruction
    #                  OR nil if there is no ip for the given line
    def first_ip_on_line(line, start=nil)
      i = 1
      total = @lines.size

      while i < total
        cur_line = @lines.at(i)
        if cur_line >= line
          ip = @lines.at(i-1)

          if !start or ip > start
            # matched the definition line, return 0
            return 0 if ip == -1
            return ip
          end
        end

        i += 2
      end

      nil
    end

    ##
    # The first line where instructions are located.
    #
    # @return [Fixnum]
    def first_line
      line_from_ip(0)
    end

    ##
    # Indicate the line in the source code that this
    # was defined on.
    #
    def defined_line
      # Detect a -1 ip, which indicates a definition entry.
      return @lines[1] if @lines[0] == -1
      first_line
    end

    ##
    #
    # Given all CompiledCodes in the system, find one that
    # was defined in file +file+ and encompasses +line+
    #
    def self.locate(file, line=nil)
      file = StringValue file

      if line
        line = Integer line
      elsif m = /\A(.*):(\d+)\Z/.match(file)
        file = m[1]
        line = m[2].to_i
      end

      ary = []
      ObjectSpace.find_object([:kind_of, Rubinius::CompiledCode], ary)

      methods = ary.find_all do |x|
        x.scope and path = x.scope.absolute_active_path and \
                    path.suffix?(file)
      end

      return methods unless line

      methods.find_all { |x| x.first_ip_on_line(line) }
    end

    ##
    # Is this actually a block of code?
    #
    # @return [Boolean]
    def is_block?
      get_metadata(:for_block)
    end

    def for_eval?
      get_metadata(:for_eval)
    end

    def for_module_body?
      get_metadata(:for_module_body)
    end

    def describe
      str = "method #{@name}: #{@total_args} arg(s), #{@required_args} required"
      if @splat
        str << ", splatted."
      end
      str
    end

    ##
    # Convenience method to return an array of the child CompiledCodes from
    # this CompiledCode's literals.
    #
    # @return [Tuple]
    def child_methods
      literals.select { |lit| lit.kind_of? CompiledCode }
    end

    def change_name(name)
      code = dup
      code.name = name

      lits = Tuple.new(code.literals.size)
      code.literals.each_with_index do |lit, idx|
        if lit.kind_of? CompiledCode and lit.is_block?
          lit = lit.change_name name
        end

        lits[idx] = lit
      end

      code.literals = lits
      return code
    end

    ##
    # Locates the CompiledCode and instruction address (IP) of the first
    # instruction on the specified line. This method recursively examines child
    # compiled methods until an exact match for the searched line is found.
    # It returns both the matching CompiledCode and the IP of the first
    # instruction on the requested line, or nil if no match for the specified line
    # is found.
    #
    # @return [(Rubinius::CompiledCode, Fixnum), NilClass] returns
    #   nil if nothing is found, else an array of size 2 containing the method
    #   the line was found in and the IP pointing there.
    def locate_line(line)
      i = 1
      total = @lines.size
      while i < total
        cur_line = @lines.at(i)
        if cur_line == line
          ip = @lines.at(i-1)
          return nil if ip < 0
          return [self, ip]
        elsif cur_line > line
          break
        end

        i += 2
      end

      # Didn't find line in this CM, so check if a contained
      # CM encompasses the line searched for
      child_methods.each do |child|
        if res = child.locate_line(line)
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
    # be decoded using this CompiledCode's locals and literals. This is provided
    # for use by the debugger, where the bytecode sequence to be decoded may not
    # exactly match the bytecode currently held by the CompiledCode, typically
    # as a result of substituting yield_debugger instructions into the bytecode.
    def decode(bytecodes = @iseq)
      decoder = Rubinius::InstructionDecoder.new(bytecodes)
      stream = decoder.decode(false)
      ip = 0

      stream.map do |inst|
        instruct = Instruction.new(inst, self, ip)
        ip += instruct.size
        instruct
      end
    end

    def arity
      if @required_args == @total_args and (@splat.nil? or @splat == -3)
        @required_args
      else
        -(@required_args + 1)
      end
    end

    def add_metadata(key, val)
      raise TypeError, "key must be a symbol" unless key.kind_of? Symbol

      case val
      when true, false, Symbol, Fixnum, String
        # ok
      else
        raise TypeError, "invalid type of value"
      end

      @metadata ||= nil # to deal with MRI seeing @metadata as not set

      unless @metadata
        @metadata = Tuple.new(2)
        @metadata[0] = key
        @metadata[1] = val
        return val
      end

      i = 0
      fin = @metadata.size

      while i < fin
        if @metadata[i] == key
          @metadata[i + 1] = val
          return val
        end

        i += 2
      end

      tup = Tuple.new(fin + 2)
      tup.copy_from @metadata, 0, fin, 0
      tup[fin] = key
      tup[fin + 1] = val

      @metadata = tup

      return val
    end

    def get_metadata(key)
      return nil unless @metadata.kind_of? Tuple

      i = 0
      while i < @metadata.size
        if @metadata[i] == key
          return @metadata[i + 1]
        end

        i += 2
      end

      return nil
    end

    ##
    # For Method#parameters
    def parameters
      params = []

      return params unless respond_to?(:local_names)

      m = required_args - post_args
      o = m + total_args - required_args
      p = o + post_args
      p += 1 if splat

      local_names.each_with_index do |name, i|
        if i < m
          params << [:req, name]
        elsif i < o
          params << [:opt, name]
        elsif splat == i
          if name == :*
            params << [:rest]
          else
            params << [:rest, name]
          end
        elsif i < p
          params << [:req, name]
        elsif block_index == i
          params << [:block, name]
        end
      end

      params
    end

    ##
    # Represents virtual machine's CPU instruction.
    # Instructions are organized into instruction
    # sequences known as iSeq, forming body
    # of CompiledCodes.
    #
    # To generate VM opcodes documentation
    # use rake doc:vm task.
    class Instruction
      class Association
        def initialize(index)
          @index = index
        end

        def inspect
          "literals[#{@index}]"
        end
      end

      class Location
        FORMAT = "%04d:"

        def initialize(location)
          @location = location
        end

        def inspect
          FORMAT % @location
        end
      end

      def initialize(inst, code, ip)
        @instruction = inst[0]
        @args = inst[1..-1]
        @comment = nil

        @args.each_index do |i|
          case @instruction.args[i]
          when :literal
            @args[i] = code.literals[@args[i]]
          when :local
            # TODO: Blocks should be able to retrieve local names as well,
            # but need access to method corresponding to home context
            if code.local_names and !code.is_block?
              @comment = code.local_names[args[i]].to_s
            end
          when :association
            @args[i] = Association.new(args[i])
          when :location
            @args[i] = Location.new(args[i])
          end
        end

        @compiled_code = code
        @ip = ip
      end

      # Instruction pointer
      attr_reader :ip

      ##
      # Return the line that this instruction is on in the method
      #
      def line
        @compiled_code.line_from_ip(ip)
      end

      ##
      # Returns the OpCode object
      #
      attr_reader :instruction

      ##
      # Returns the symbol representing the opcode for this instruction.
      #
      def opcode
        @instruction.opcode
      end

      ##
      # Returns an array of 0 to 2 arguments, depending on the opcode.
      #
      attr_reader :args

      ##
      # Returns a Fixnum indicating how wide the instruction takes up
      # in the instruction stream
      #
      def size
        @args.size + 1
      end

      ##
      # A nice human readable interpretation of this set of instructions
      def to_s
        str = "#{Location::FORMAT}  %-27s" % [@ip, opcode]
        str << @args.map{ |a| a.inspect }.join(', ')
        if @comment
          str << "    # #{@comment}"
        end

        return str
      end
    end
  end
end
