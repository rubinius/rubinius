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

    CALL_FLAG_PRIVATE = 1
    CALL_FLAG_CONCAT = 2

    ##
    # Jump label for the goto instructions.

    class Label
      def initialize(generator)
        @generator = generator
        @position = nil
        @used = false
      end

      attr_accessor :position

      def set!
        @position = @generator.ip
      end

      attr_reader :used
      alias_method :used?, :used

      def used!
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
      @last_line = 0
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

    # Temporary
    attr_accessor :desc

    def ===(pattern)

      return false unless @stream.size == pattern.size

      i = 0
      @stream.each do |part|
        pat = pattern[i]
        j = 0

        if pat == :any
          j += 1
          next
        end

        if part.kind_of? Array
          part.each do |e|
            s = pat[j]
            next if s == :any
            return false unless e == s

            j += 1
          end
        else
          next if pat[j] == :any
          return false unless pat[j] == part
          j += 1
        end

        i += 1
      end

      return true
    end

    def execute(node)
      node.bytecode self
    end

    alias_method :run, :execute

    # Formalizers

    def encode(encoder, calculator)
      set_label_positions

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

    # Replace all Labels in the stream by dereferencing them to their integer
    # position.
    def set_label_positions
      @stream.each_with_index do |op, index|
        if op.kind_of? Label
          unless position = op.position
            raise "Label used but position is not set"
          else
            @stream[index] = position
          end
        end
      end
    end

    # Helpers

    def new_stack_local
      idx = @stack_locals
      @stack_locals += 1
      return idx
    end

    def add(instruction, arg1=nil, arg2=nil)
      @stream << InstructionSet[instruction].bytecode
      length = 1

      if arg1
        @stream << arg1
        length = 2

        if arg2
          @stream << arg2
          length = 3
        end
      end

      @ip += length

      @instruction = instruction
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

      if line > @last_line
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


    # Operations

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
        push_int(what)
      else
        raise Error, "Unknown push argument '#{what.inspect}'"
      end
    end

    def push_symbol(what)
      push_literal what
    end

    def push_int(int)
      case int
      when -1
        add :meta_push_neg_1
      when 0
        add :meta_push_0
      when 1
        add :meta_push_1
      when 2
        add :meta_push_2
      else
        # The max value we use for inline ints. Above this, they're
        # stored in the literals tuple.
        inline_cutoff = 256
        if int > 0 and int < inline_cutoff
          add :push_int, int
        else
          push_literal int
        end
      end
    end

    def push_generator(generator)
      @generators << generator
      push_literal generator
    end

    # Pushes the specified literal value into the literal's tuple
    def push_literal(literal)
      index = find_literal literal
      add :push_literal, index
      return index
    end

    # Puts +what+ is the literals tuple without trying to see if
    # something that is like +what+ is already there.
    def push_unique_literal(literal)
      index = add_literal literal
      add :push_literal, index
      return index
    end

    # Pushes the literal value on the stack into the specified position in the
    # literals tuple. Most timees, push_literal should be used instead; this
    # method exists to support RegexLiteral, where the compiled literal value
    # (a Regex object) does not exist until runtime.
    def push_literal_at(index)
      add :push_literal, index
      return index
    end

    def set_literal(index)
      add :set_literal, index
    end

    def push_ivar(name)
      add :push_ivar, find_literal(name)
    end

    def set_ivar(name)
      add :set_ivar, find_literal(name)
    end

    def push_const(name)
      add :push_const_fast, find_literal(name), add_literal(nil)
    end

    def find_const(name)
      add :find_const, find_literal(name)
    end

    def dup
      add :dup_top
    end

    def swap
      add :swap_stack
    end

    def gif(lbl)
      lbl.used!
      add :goto_if_false, lbl
    end

    def git(lbl)
      lbl.used!
      add :goto_if_true, lbl
    end

    def goto(lbl)
      lbl.used!
      add :goto, lbl
    end

    def set_local(index)
      add :set_local, index
    end

    def push_local(index)
      add :push_local, index
    end

    def set_local_depth(depth, index)
      add :set_local_depth, depth, index
    end

    def push_local_depth(depth, index)
      add :push_local_depth, depth, index
    end

    def cast_array
      unless @instruction == :cast_array or @instruction == :make_array
        add :cast_array
      end
    end

    def send(meth, count, priv=false)
      add :allow_private if priv

      unless count.kind_of? Fixnum
        raise Error, "count must be a number"
      end

      idx = find_literal(meth)

      if count == 0
        add :send_method, idx
      else
        add :send_stack, idx, count
      end
    end

    def send_with_block(meth, count, priv=false)
      add :allow_private if priv

      unless count.kind_of? Fixnum
        raise Error, "count must be a number"
      end

      idx = find_literal(meth)

      add :send_stack_with_block, idx, count
    end

    def send_with_splat(meth, args, priv=false, concat=false)
      val = 0
      val |= CALL_FLAG_CONCAT  if concat
      add :set_call_flags, val unless val == 0

      add :allow_private if priv

      idx = find_literal(meth)
      add :send_stack_with_splat, idx, args
    end

    def send_super(meth, args, splat=false)
      idx = find_literal(meth)

      if splat
        add :send_super_stack_with_splat, idx, args
      else
        add :send_super_stack_with_block, idx, args
      end
    end

    def zsuper(meth)
      add :zsuper, find_literal(meth)
    end

    def check_serial(sym, serial)
      add :check_serial, find_literal(sym), serial.to_i
    end

    def check_serial_private(sym, serial)
      add :check_serial_private, find_literal(sym), serial.to_i
    end

    def create_block(generator)
      @generators << generator
      index = add_literal generator
      add :create_block, index
    end

    def method_missing(instruction, arg1=nil, arg2=nil)
      add instruction, arg1, arg2
    end
  end
end
