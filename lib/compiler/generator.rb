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
      attr_reader :used
      alias_method :used?, :used

      def initialize(generator)
        @generator = generator
        @position  = nil
        @used      = false
        @location  = nil
        @locations = nil
      end

      def set!
        @position = @generator.ip
        if @locations
          @locations.each { |x| @generator.stream[x] = @position }
        elsif @location
          @generator.stream[@location] = @position
        end
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
    end

    attr_reader   :ip, :stream, :iseq, :literals
    attr_accessor :break, :redo, :next, :retry, :file, :name,
                  :required_args, :total_args, :splat_index,
                  :local_count, :local_names, :primitive, :for_block

    def execute(node)
      node.bytecode self
    end

    alias_method :run, :execute

    # Formalizers

    def encode(encoder, calculator)
      @iseq = InstructionSequence.from @stream.to_tuple

      sdc = calculator.new @iseq, @lines
      begin
        stack_size = sdc.run + @local_count
      rescue Exception => e
        @iseq.show
        raise e
      end

      stack_size += 1 if @for_block
      @stack_size = stack_size

      @generators.each { |d| d.encode encoder, calculator }
    end

    def package(klass)
      @literals.each_with_index do |literal, index|
        if literal.kind_of? self.class
          @literals[index] = literal.package klass
        end
      end

      cm = klass.new
      cm.iseq           = @iseq
      cm.literals       = @literals.to_tuple
      cm.lines          = @lines.to_tuple

      cm.required_args  = @required_args
      cm.total_args     = @total_args
      cm.splat          = @splat_index
      cm.local_count    = @local_count
      cm.local_names    = @local_names.to_tuple if @local_names

      cm.stack_size     = @stack_size + @stack_locals
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
      elsif line > @last_line
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


    # Helpers and overrides

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

    def push_int(int)
      # Integers greater than 256 are stored in the literals tuple.
      if int > 2 and int < 256
        super int
      else
        case int
        when -1
          meta_push_neg_1
        when 0
          meta_push_0
        when 1
          meta_push_1
        when 2
          meta_push_2
        else
          push_literal int
        end
      end
    end

    def push_generator(generator)
      @generators << generator
      push_literal generator
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
    def push_literal(literal_or_index, search=true)
      index = search ? find_literal(literal_or_index) : literal_or_index
      super index
      return index
    end

    # Puts +what+ is the literals tuple without trying to see if
    # something that is like +what+ is already there.
    def push_unique_literal(literal)
      index = add_literal literal
      push_literal index, false
      return index
    end

    # Pushes the literal value on the stack into the specified position in the
    # literals tuple. Most timees, push_literal should be used instead; this
    # method exists to support RegexLiteral, where the compiled literal value
    # (a Regex object) does not exist until runtime.
    def push_literal_at(index)
      push_literal index, false
      return index
    end

    def push_ivar(name)
      super find_literal(name)
    end

    def set_ivar(name)
      super find_literal(name)
    end

    def push_const(name)
      push_const_fast find_literal(name), add_literal(nil)
    end

    def find_const(name)
      super find_literal(name)
    end

    def dup
      dup_top
    end

    def swap
      swap_stack
    end

    def gif(label)
      goto_if_false label
      label.used_at @ip - 1
    end

    def git(label)
      goto_if_true label
      label.used_at @ip - 1
    end

    def goto(label)
      super label
      label.used_at @ip - 1
    end

    def setup_unwind(label, type)
      super label, type
      label.used_at @ip - 2
    end

    def cast_array
      unless @instruction == :cast_array or @instruction == :make_array
        super()
      end
    end

    def invoke_primitive(name, count)
      super find_literal(name), Integer(count)
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

    def zsuper(meth)
      super find_literal(meth)
    end

    def check_serial(sym, serial)
      super find_literal(sym), serial.to_i
    end

    def check_serial_private(sym, serial)
      super find_literal(sym), serial.to_i
    end

    def create_block(generator)
      @generators << generator
      index = add_literal generator
      super index
    end
  end
end
