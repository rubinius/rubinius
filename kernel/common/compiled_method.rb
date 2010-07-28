module Rubinius
  class CompiledMethod < Executable

    ##
    # Any CompiledMethod with this value in it's serial slot
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
    attr_accessor :total_args    # [Integer] number of total args
    attr_accessor :splat         # [Integer] POSITION of the splat arg
    attr_accessor :literals      # [Tuple]   tuple of the literals
    attr_accessor :lines         # [Tuple]   tuple of the lines where its found
    attr_accessor :file          # [Symbol]  the file where this comes from
    attr_accessor :local_names   # [Array<Symbol>] names of the local vars
    attr_accessor :scope

    ##
    # Compare this method with +other+. Instead of bugging out if +other+
    # isn't a {CompiledMethod}, this returns +false+ immediately unless
    # we're comparing two apples, AKA, {CompiledMethod}s. The methods have
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
    # (which {CompiledMethod}s DO keep track of)
    #
    # @todo Make example (in method documentation) match reality
    # @param [Rubinius::CompiledMethod] other the other part to compare
    # @param [Boolean]
    def ==(other)
      return false unless other.kind_of? CompiledMethod
      @primitive       == other.primitive     and # [Symbol]  the instructions to be run by the VM
        @name          == other.name          and # [Symbol]  the name of the method
        @iseq          == other.iseq          and # [Tuple]   the instructions to execute
        @stack_size    == other.stack_size    and # [Integer] the size of the stack from compile time
        @local_count   == other.local_count   and # [Integer] the number of local vars
        @required_args == other.required_args and # [Integer] the number of required args
        @total_args    == other.total_args    and # [Integer] the number of total args
        @splat         == other.splat         and # [Integer] the POSITION of the splat arg
        @literals      == other.literals      and # [Tuple]   a tuple of the literals
        @lines         == other.lines         and # [Tuple]   a tuple of the lines where its found
        @file          == other.file          and # [Symbol]  the file where this comes from
        @local_names   == other.local_names       # [Array<Symbol>] the names of the local vars. used by eval
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
    # Return a human readable interpretation of this method.
    #
    # @return [String]
    def inspect
      "#<#{self.class.name} #{@name} file=#{@file}>"
    end

    ##
    # Make the method change its scope so that it can act as though
    # it's frome somewhere else. You can pass in a method and +self+
    # will borrow its scope.
    #
    # @param [#scope] other the other method that has a scope we can borrow
    def inherit_scope(other)
      @scope = other.scope
    end

    ##
    # Invoke method directly.
    #
    # @note The explicit block argument is unnecessary, but
    #       present for completeness.
    #
    def activate(recv, mod, args, locals = nil, &prc)
      Ruby.primitive :compiledmethod_activate
      raise PrimitiveFailure, "Unable to call #{@name} on #{recv.inspect}"
    end

    ##
    # Set a breakpoint here.
    #
    # @raise [ArgumentError]
    # @param [Fixnum] ip where exactly to place the breakpoint
    def set_breakpoint(ip, obj)
      Ruby.primitive :compiledmethod_set_breakpoint
      raise ArgumentError, "Unable to set breakpoint on #{inspect} at invalid bytecode address #{ip}"
    end

    ##
    # Erase a breakpoint from being here
    #
    # @raise [ArgumentError]
    # @param [Fixnum] ip where exactly to remove the breakpoint
    def clear_breakpoint(ip)
      Ruby.primitive :compiledmethod_clear_breakpoint
      raise ArgumentError, "Unable to clear breakpoint on #{inspect} at invalid bytecode address #{ip}"
    end

    ##
    # Is there a breakpoint set in this method at +ip+?
    #
    # @raise  [ArgumentError]
    # @param  [Fixnum] ip where exactly the breakpoint supposedly is
    # @return [Boolean] is it really there?
    def breakpoint?(ip)
      Ruby.primitive :compiledmethod_is_breakpoint
      raise ArgumentError, "Unable to retrieve breakpoint status on #{inspect} at bytecode address #{ip}"
    end

    ##
    # Accessor for a hash of filenames (as per $" / $LOADED_FEATURES) to the
    # script CompiledMethod.
    def self.scripts
      @scripts ||= {}
    end

    ##
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
      attr_accessor :compiled_method
      attr_accessor :file_path
      attr_accessor :data_path
      attr_accessor :eval_binding

      def initialize(method, path=nil, for_eval=false)
        @compiled_method = method
        @file_path = path
        @for_eval = for_eval
        @eval_binding = nil
      end

      def eval?
        @for_eval
      end
    end

    # Creates the Script instance for a toplevel compiled method.
    def create_script(wrap=false)
      script = CompiledMethod::Script.new(self)

      # Setup the scoping.
      ss = StaticScope.new(Object)
      ss.script = script

      if wrap
        @scope = StaticScope.new(Module.new, ss)
      else
        @scope = ss
      end

      compile

      mc = Rubinius.object_metaclass(MAIN)
      mc.method_table.store :__script__, self, :public
      VM.reset_method_cache :__script__

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
    # Returns the address (IP) of the first instruction in this CompiledMethod
    # that is on the specified line, or the address of the first instruction on
    # the next code line after the specified line if there are no instructions
    # on the requested line.
    # This method only looks at instructions within the current CompiledMethod;
    # see #locate_line for an alternate method that also searches inside the child
    # CompiledMethods.
    #
    # @return [Fixnum] the address of the first instruction
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

      -1
    end

    ##
    # The first line of source code.
    #
    # @return [Fixnum]
    def first_line
      if @lines.size > 1
        @lines[1]
      else
        -1
      end
    end

    ##
    # Is this actually a block of code?
    #
    # @return [Boolean]
    def is_block?
      get_metadata(:for_block)
    end

    def describe
      str = "method #{@name}: #{@total_args} arg(s), #{@required_args} required"
      if @splat
        str << ", splatted."
      end
      str
    end

    ##
    # Convenience method to return an array of the child CompiledMethods from
    # this CompiledMethod's literals.
    #
    # @return [Tuple]
    def child_methods
      literals.select {|lit| lit.kind_of? CompiledMethod }
    end

    def change_name(name)
      cm = dup
      cm.name = name

      lits = Tuple.new(cm.literals.size)
      cm.literals.each_with_index do |lit, idx|
        if lit.kind_of? CompiledMethod and lit.is_block?
          lit = lit.change_name name
        end

        lits[idx] = lit
      end

      cm.literals = lits
      return cm
    end

    ##
    # Convenience method to return an array of the SendSites from
    # this CompiledMethod's literals.
    #
    # @return [Tuple]
    def send_sites
      literals.select {|lit| lit.kind_of? SendSite }
    end

    ##
    # Locates the CompiledMethod and instruction address (IP) of the first
    # instruction on the specified line. This method recursively examines child
    # compiled methods until an exact match for the searched line is found.
    # It returns both the matching CompiledMethod and the IP of the first
    # instruction on the requested line, or nil if no match for the specified line
    # is found.
    # TODO: Update this to work with new lines representation
    #
    # @return [(Rubinius::CompiledMethod, Fixnum), NilClass] returns
    #   nil if nothing is found, else an array of size 2 containing the method
    #   the line was found in and the IP pointing there.
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
      require 'compiler/iseq'

      decoder = Rubinius::InstructionDecoder.new(bytecodes)
      stream = decoder.decode(false)
      ip = 0

      stream.map do |inst|
        instruct = Instruction.new(inst, self, ip)
        ip += instruct.size
        instruct
      end
    end

    ##
    # Graphs the control flow of this method
    def graph_control(file, open_now=false)
      raise "We're sorry, this is out of order. Check back soon."

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
    # Represents virtual machine's CPU instruction.
    # Instructions are organized into instruction
    # sequences known as iSeq, forming body
    # of CompiledMethods.
    #
    # To generate VM opcodes documentation
    # use rake doc:vm task.
    class Instruction
      def initialize(inst, cm, ip)
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
            if cm.local_names and !cm.is_block?
              @comment = cm.local_names[args[i]].to_s
            end
          when :block_local
            # TODO: Blocks should be able to retrieve enclosing block local names as well,
            # but need access to static scope
            #@args[i] = cm.local_names[args[i]] if cm.local_names and args[0] == 0
          end
        end

        @ip = ip
        @line = cm.line_from_ip(ip)
      end

      # Instruction pointer
      attr_reader :ip
      attr_reader :line

      ##
      # Returns the OpCode object
      #
      # Associated OpCode instance.
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
      # A nice human readable interpretation of this set of instructions
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
