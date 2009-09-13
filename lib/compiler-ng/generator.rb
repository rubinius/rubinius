module Rubinius
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
      @literals = []
      @ip = 0
      @modstack = []
      @break = nil
      @redo = nil
      @next = nil
      @retry = nil
      @ensure_return = nil
      @last_line = nil
      @file = nil
      @lines = []
      @primitive = nil
      @last = nil

      @required_args = 0
      @total_args = 0
      @splat_index = nil
      @local_names = nil
      @local_count = 0

      @children = []
    end

    attr_reader :cache_size, :literals, :iseq
    attr_accessor :break, :redo, :next, :retry, :ensure_return
    attr_accessor :file, :name, :required_args, :total_args, :splat_index,
                  :local_count, :local_names, :primitive

    # Temporary
    attr_accessor :desc

    def stream
      flush if @last
      @stream
    end

    def ip
      flush if @last
      @ip
    end

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

    def run(node)
      node.bytecode self if node # HACK
    end

    # Formalizers

    def encode(encoder, calculator)
      flush
      collapse_labels

      @iseq = encoder.new.encode_stream @stream

      sdc = calculator.new @iseq
      begin
        sdc_stack = sdc.run
      rescue RuntimeError => e
        puts "ERROR in #{@name}, #{@lines.first}"
        raise e
      end

      stack_size = sdc_stack + @local_count
      stack_size += 1 if @for_block
      @stack_size = stack_size

      @children.each { |d| d.encode encoder, calculator }
    end

    def package(klass)
      @literals.each_with_index do |literal, index|
        if literal.kind_of? Compiler::MethodDescription
          @literals[index] = literal.generator.package klass
        end
      end

      cm = klass.new
      cm.iseq           = @iseq
      cm.literals       = literals.to_tuple
      cm.lines          = @lines.to_tuple
      cm.exceptions     = [].to_tuple # TODO: remove

      cm.required_args  = @required_args
      cm.total_args     = @total_args
      cm.splat          = @splat_index
      cm.local_count    = @local_count
      cm.local_names    = @local_names.to_tuple if @local_names

      cm.stack_size     = @stack_size
      cm.file           = @file # TODO
      cm.name           = @name # TODO
      cm.primitive      = @primitive # TODO

      cm
    end

    def collapse_labels
      @stream.each do |part|
        if part.kind_of? Array
          part.map! do |x|
            if x.kind_of? Label
              pos = x.position
              if pos.nil?
                raise Error, "Label never set, but used"
              end
              pos
            else
              x
            end
          end
        end
      end
    end


    # Helpers

    def add(*instruction)
      what = @last
      @last = instruction

      return unless what

      @ip += what.size
      if what.size == 1
        @stream << what.first
      else
        @stream << what
      end
    end

    def flush
      return unless @last
      @ip += @last.size
      if @last.size == 1
        @stream << @last.first
      else
        @stream << @last
      end

      @last = nil
    end

    # Find the index for the specified literal, or create a new slot if the
    # literal has not been encountered previously.
    def find_literal(what)
      idx = @literals.index(what)
      return idx if idx
      add_literal(what)
    end

    # Add literal exists to allow RegexLiteral's to create a new regex literal
    # object at run-time. All other literals should be added via find_literal,
    # which re-use an existing matching literal if one exists.
    def add_literal(val)
      idx = @literals.size
      @literals << val
      return idx
    end

    # Commands (these don't generate data in the stream)

    def advanced_since?(ip)
      return true if @last
      @ip > ip
    end

    def push_modifiers
      @modstack << [@break, @redo, @next, @retry]
    end

    def pop_modifiers
      @break, @redo, @next, @retry = @modstack.pop
    end

    def set_line(line)
      if line and line > 0 and line != @last_line
        # Fold redundent line changes on the same ip into the same
        # entry
        if @lines[-2] == @ip
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

    attr_reader :file

    def close
      if @lines.empty?
        @lines << @ip
        @lines << @last_line
      end

      @lines << @ip
    end

    def as_primitive(name)
      @primitive = name
    end

    def new_label
      Label.new(self)
    end


    # Operations

    SideEffectFreePushes = [:push_nil, :push_true, :push_false,
      :push_int, :meta_push_0, :meta_push_1, :meta_push_neg_1,
      :meta_push_2, :push_self]

    def pop
      if @last and SideEffectFreePushes.include? @last.first
        @last = nil
      else
        flush
        add :pop
      end
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

    # Pushes the specified literal value into the literal's tuple
    def push_literal(what)
      idx = find_literal(what)
      add :push_literal, idx

      # TODO: fix
      @children << what.generator if what.kind_of? Compiler::MethodDescription

      return idx
    end

    # Puts +what+ is the literals tuple without trying to see if
    # something that is like +what+ is already there.
    def push_unique_literal(what)
      idx = add_literal(what)
      add :push_literal, idx
      return idx
    end

    # Pushes the literal value on the stack into the specified position in the
    # literals tuple. Most timees, push_literal should be used instead; this
    # method exists to support RegexLiteral, where the compiled literal value
    # (a Regex object) does not exist until runtime.
    def push_literal_at(idx)
      add :push_literal, idx
      return idx
    end

    def set_literal(which)
      add :set_literal, which
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

    def find_cpath_top_const(name)
      push_cpath_top
      find_const(name)
    end

    def find_const(name)
      add :find_const, find_literal(name)
    end

    def set_const(name, sub=false)
      if sub
        add :set_const_at, find_literal(name)
      else
        add :set_const, find_literal(name)
      end
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

    def equal()
      add :equal
    end

    def goto(lbl)
      lbl.used!
      add :goto, lbl
    end

    def attach_method(name)
      add :attach_method, find_literal(name)
    end

    def add_method(name)
      add :add_method, find_literal(name)
    end

    def check_argcount(a, b)
      add :check_argcount, a, b
    end

    def set_local(a)
      add :set_local, a
    end

    def push_local(a)
      add :push_local, a
    end

    def set_local_depth(a, b)
      add :set_local_depth, a, b
    end

    def push_local_depth(a, b)
      add :push_local_depth, a, b
    end

    def allocate_stack(a)
      add :allocate_stack, a
    end

    def set_local_fp(a)
      add :set_local_fp, a
    end

    def get_local_fp(a)
      add :get_local_fp, a
    end

    def make_array(count)
      add :make_array, count
    end

    def cast_array
      unless @last and [:cast_array, :make_array].include? @last.first
        add :cast_array
      end
    end

    def send(meth, count, priv=false)
      add :allow_private if priv

      unless count.kind_of? Fixnum
        raise Error, "count must be a number"
      end

      idx = add_literal(meth)

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

      idx = add_literal(meth)

      add :send_stack_with_block, idx, count
    end

    def send_with_splat(meth, args, priv=false, concat=false)
      val = 0
      val |= CALL_FLAG_CONCAT  if concat
      add :set_call_flags, val unless val == 0

      add :allow_private if priv

      idx = add_literal(meth)
      add :send_stack_with_splat, idx, args
    end

    def send_super(meth, args, splat=false)
      idx = add_literal(meth)

      if splat
        add :send_super_stack_with_splat, idx, args
      else
        add :send_super_stack_with_block, idx, args
      end
    end

    def check_serial(sym, serial)
      add :check_serial, add_literal(sym), serial.to_i
    end

    def check_serial_private(sym, serial)
      add :check_serial_private, add_literal(sym), serial.to_i
    end

    def create_block(desc)
      idx = add_literal(desc)
      add :create_block, idx
    end

    def method_missing(*op)
      if op[0] == :val
        raise "Passed incorrect op to method_missing in generator.rb: #{op.inspect}"
      end

      add(*op)
    end
  end
end
