module Rubinius
  # Temporary
  class InstructionSequence
    attr_accessor :opcodes

    def self.from(opcodes)
      is = allocate
      is.opcodes = opcodes
      is
    end
  end

  class Generator
    include GeneratorMethods

    CALL_FLAG_PRIVATE = 1
    CALL_FLAG_CONCAT = 2

    ##
    # Jump label for the branch instructions. The use scenarios for labels:
    #   1. Used and then set
    #        g.gif label
    #        ...
    #        label.set!
    #   2. Set and then used
    #        label.set!
    #        ...
    #        g.git label
    #   3. 1, 2
    #
    # Many labels are only used once. This class employs two small
    # optimizations. First, for the case where a label is used once then set,
    # the label merely records the point it was used and updates that location
    # to the concrete IP when the label is set. In the case where the label is
    # used multiple times, it records each location and updates them to an IP
    # when the label is set. In both cases, once the label is set, each use
    # after that updates the instruction stream with a concrete IP at the
    # point the label is used. This avoids the need to ever record all the
    # labels or search through the stream later to change symbolic labels into
    # concrete IP's.

    class Label
      attr_accessor :position
      attr_reader :used, :basic_block
      alias_method :used?, :used

      def initialize(generator)
        @generator   = generator
        @basic_block = generator.new_basic_block
        @position    = nil
        @used        = false
        @location    = nil
        @locations   = nil
      end

      def set!
        @position = @generator.ip
        if @locations
          @locations.each { |x| @generator.stream[x] = @position }
        elsif @location
          @generator.stream[@location] = @position
        end

        @generator.current_block.add_edge @basic_block
        @generator.current_block.close
        @generator.current_block = @basic_block
        @basic_block.open
      end

      def used_at(ip)
        if @position
          @generator.stream[ip] = @position
        elsif !@location
          @location = ip
        elsif @locations
          @locations << ip
        else
          @locations = [@location, ip]
        end
        @used = true
      end
    end

    class BasicBlock
      def initialize(generator)
        @generator  = generator
        @ip         = generator.ip
        @closed_ip  = 0
        @enter_size = 0
        @max_size   = 0
        @stack      = 0
        @edges      = nil
        @visited    = false
        @closed     = false
      end

      def add_stack(size)
        @stack += size
        @max_size = @stack if @stack > @max_size
      end

      def add_edge(block)
        @edges ||= []
        @edges << block
      end

      def open
        @ip = @generator.ip
      end

      def close
        @closed = true
        @closed_ip = @generator.ip
      end

      def location
        line = @generator.ip_to_line @ip
        "#{@generator.name}: line: #{line}, IP: #{@ip}"
      end

      def visit(stack_size)
        if @visited
          unless stack_size == @enter_size
            raise CompileError, "unbalanced stack at #{location}: #{stack_size} != #{@enter_size}"
          end
        else
          @visited = true
          @enter_size = stack_size

          if not @closed
            raise CompileError, "control fails to exit properly at #{location}"
          end

          @generator.accumulate_stack(@enter_size + @max_size)

          if @edges
            net_size = stack_size + @stack
            @edges.each { |e| e.visit net_size }
          end
        end
      end
    end

    def initialize
      @stream = []
      @literals_map = Hash.new { |h,k| h[k] = add_literal(k) }
      @literals = []
      @ip = 0
      @modstack = []
      @break = nil
      @redo = nil
      @next = nil
      @retry = nil
      @last_line = nil
      @file = nil
      @lines = []
      @primitive = nil
      @instruction = nil
      @for_block = nil

      @required_args = 0
      @total_args = 0
      @splat_index = nil
      @local_names = nil
      @local_count = 0

      @state = []
      @generators = []

      @stack_locals = 0

      @enter_block = new_basic_block
      @current_block = @enter_block
      @max_stack = 0
    end

    attr_reader   :ip, :stream, :iseq, :literals
    attr_accessor :break, :redo, :next, :retry, :file, :name,
                  :required_args, :total_args, :splat_index,
                  :local_count, :local_names, :primitive, :for_block, :current_block

    def execute(node)
      node.bytecode self
    end

    alias_method :run, :execute

    # Formalizers

    def encode(encoder)
      @iseq = InstructionSequence.from @stream.to_tuple

      begin
        # Validate the stack and calculate the max depth
        @enter_block.visit 0
      rescue Exception => e
        if $DEBUG
          puts "Error computing stack for #{@name}: #{e.message} (#{e.class})"
          @iseq.show
        end
        raise e
      end

      @generators.each { |x| @literals[x].encode encoder }
    end

    def package(klass)
      @generators.each { |x| @literals[x] = @literals[x].package klass }

      cm = klass.new
      cm.iseq           = @iseq
      cm.literals       = @literals.to_tuple
      cm.lines          = @lines.to_tuple

      cm.required_args  = @required_args
      cm.total_args     = @total_args
      cm.splat          = @splat_index
      cm.local_count    = @local_count
      cm.local_names    = @local_names.to_tuple if @local_names

      cm.stack_size     = max_stack_size
      cm.file           = @file
      cm.name           = @name
      cm.primitive      = @primitive

      cm
    end


    # Commands (these don't generate data in the stream)

    def state
      @state.last
    end

    def push_state(scope)
      @state << AST::State.new(scope)
    end

    def pop_state
      @state.pop
    end

    def push_modifiers
      @modstack << [@break, @redo, @next, @retry]
    end

    def pop_modifiers
      @break, @redo, @next, @retry = @modstack.pop
    end

    def set_line(line)
      raise Exception, "source code line cannot be nil" unless line

      if !@last_line
        @lines << @ip
        @lines << line
        @last_line = line
      elsif line != @last_line
        # Fold redundent line changes on the same ip into the same
        # entry, except for in the case where @ip is 0. Here's why:
        #
        #   def some_method
        #   end
        #
        # There is nothing in the bytecode stream that corresponds
        # to 'def some_method' so the first line of the method will
        # be recorded as the line 'end' is on.

        if @ip > 0 and @lines[-2] == @ip
          @lines[-1] = line
        else
          @lines << @ip
          @lines << line
        end

        @last_line = line
      end
    end

    def line
      @last_line
    end

    def ip_to_line(ip)
      total = @lines.size - 2
      i = 0

      while i < total
        if ip >= @lines[i] and ip <= @lines[i+2]
          return @lines[i+1]
        end

        i += 2
      end
    end

    def close
      if @lines.empty?
        msg = "closing a method definition with no line info: #{file}:#{line}"
        raise Exception, msg
      end

      @lines << @ip
    end

    def send_primitive(name)
      @primitive = name
    end

    def new_label
      Label.new(self)
    end


    # Helpers

    def new_basic_block
      BasicBlock.new self
    end

    def accumulate_stack(size)
      @max_stack = size if size > @max_stack
    end

    def max_stack_size
      size = @max_stack + @local_count + @stack_locals
      size += 1 if @for_block
      size
    end

    def new_stack_local
      idx = @stack_locals
      @stack_locals += 1
      return idx
    end

    def push(what)
      case what
      when :true
        push_true
      when :false
        push_false
      when :self
        push_self
      when :nil
        push_nil
      when Integer
        push_int what
      else
        raise Error, "Unknown push argument '#{what.inspect}'"
      end
    end

    def push_generator(generator)
      index = push_literal generator
      @generators << index
      index
    end

    # Find the index for the specified literal, or create a new slot if the
    # literal has not been encountered previously.
    def find_literal(literal)
      @literals_map[literal]
    end

    # Add literal exists to allow RegexLiteral's to create a new regex literal
    # object at run-time. All other literals should be added via find_literal,
    # which re-use an existing matching literal if one exists.
    def add_literal(literal)
      index = @literals.size
      @literals << literal
      return index
    end

    # Pushes the specified literal value into the literal's tuple
    def push_literal(literal)
      index = find_literal literal
      emit_push_literal index
      return index
    end

    # Puts +what+ is the literals tuple without trying to see if
    # something that is like +what+ is already there.
    def push_unique_literal(literal)
      index = add_literal literal
      emit_push_literal index
      return index
    end

    # Pushes the literal value on the stack into the specified position in the
    # literals tuple. Most timees, push_literal should be used instead; this
    # method exists to support RegexLiteral, where the compiled literal value
    # (a Regex object) does not exist until runtime.
    def push_literal_at(index)
      emit_push_literal index
      return index
    end

    # The push_const instruction itself is unused right now. The instruction
    # parser does not emit a GeneratorMethods#push_const. This method/opcode
    # was used in the compiler before the push_const_fast instruction. Rather
    # than changing the compiler code, this helper was used.
    def push_const(name)
      push_const_fast find_literal(name), add_literal(nil)
    end

    def last_match(mode, which)
      push_int Integer(mode)
      push_int Integer(which)
      invoke_primitive :regexp_last_match_result, 2
    end

    def send(meth, count, priv=false)
      allow_private if priv

      unless count.kind_of? Fixnum
        raise Error, "count must be a number"
      end

      idx = find_literal(meth)

      if count == 0
        send_method idx
      else
        send_stack idx, count
      end
    end

    def send_with_block(meth, count, priv=false)
      allow_private if priv

      unless count.kind_of? Fixnum
        raise Error, "count must be a number"
      end

      idx = find_literal(meth)

      send_stack_with_block idx, count
    end

    def send_with_splat(meth, args, priv=false, concat=false)
      val = 0
      val |= CALL_FLAG_CONCAT  if concat
      set_call_flags val unless val == 0

      allow_private if priv

      idx = find_literal(meth)
      send_stack_with_splat idx, args
    end

    def send_super(meth, args, splat=false)
      idx = find_literal(meth)

      if splat
        send_super_stack_with_splat idx, args
      else
        send_super_stack_with_block idx, args
      end
    end
  end
end
