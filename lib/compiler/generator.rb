class Compiler
    
  class Generator
    
    class Label
      def initialize(generator)
        @generator = generator
        @position = nil
      end
      
      attr_accessor :position
      
      def set!
        @position = @generator.ip
      end
      
    end
        
    def initialize
      @stream = []
      @encoder = InstructionSequence::Encoder.new
      @literals = []
      @ip = 0
      @cache_size = 0
      @enable_method_cache = false
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
      
      @exceptions = []
    end
    
    attr_reader :ip, :cache_size, :exceptions
    attr_accessor :break, :redo, :next, :retry, :ensure_return
    
    def run(node)
      node.bytecode self
    end

    # Formalizers
        
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
    
    def encode_lines
      tup = Tuple.new(@lines.size)
      i = 0
      @lines.each do |ent|
        tup[i] = Tuple[ent[0], ent[1], ent[2]]
        i += 1
      end
      
      return tup
    end
    
    def encode_literals
      tup = Tuple.new(@literals.size)
      i = 0
      @literals.each do |lit|
        if lit.kind_of? Compiler::MethodDescription
          lit = lit.to_cmethod
        end
        tup[i] = lit
        i += 1
      end
      
      return tup
    end
    
    def encode_exceptions
      @exceptions.sort!
  
      tup = Tuple.new(@exceptions.size)
      i = 0
      @exceptions.each do |e|
        tup[i] = e.as_tuple
        i += 1
      end
      
      return tup
    end
    
    def to_cmethod(desc)
      collapse_labels
      iseq = @encoder.encode_stream @stream
      cm = CompiledMethod.new.from_string iseq, desc.locals.size, desc.required

      if @file
        cm.file = @file.to_sym
      else
        cm.file = nil
      end
      
      cm.name = desc.name

      if @primitive
        cm.primitive = @primitive
      else
        cm.primitive = -1
      end
      
      cm.literals = encode_literals()
      cm.lines = encode_lines()
      cm.exceptions = encode_exceptions()
      cm.serial = 0
      cm.cache = @cache_size
      cm.local_names = desc.locals.encoded_order
      return cm
    end
    
    # Helpers
    
    def add(*what)
      @ip += what.size
      @stream << what
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
      @ip > ip
    end
    
    def push_modifiers
      @modstack << [@break, @redo, @next, @retry]
    end
    
    def pop_modifiers
      @break, @redo, @next, @retry = @modstack.pop
    end
    
    def set_line(line, file)
      if line != @last_line
        # Update the last entry to complete it
        if @last_line
          @lines.last[1] = @ip - 1
        end
        @last_line = line
        
        @lines << [@ip, nil, line]
        @file = file
      end
    end
    
    def line
      @last_line
    end
    
    attr_reader :file

    def import_position(g)
      set_line g.line, g.file
    end
    
    def close
      if @lines and !@lines.empty? and @lines.last[1].nil?
        @lines.last[1] = @ip
      end
    end
    
    def as_primitive(name)
      @primitive = name
    end
    
    def new_label
      Label.new(self)
    end
    
    class ExceptionBlock
      def initialize(gen)
        @generator = gen
      end
      
      def start!
        @start = @generator.ip
      end
      
      def handle!
        @handler = @generator.ip
        @end = @handler - 1
      end
      
      def range
        [@start, @end]
      end
      
      def as_tuple
        Tuple[@start, @end, @handler]
      end
      
      def <=>(other)
        return 0 if self.equal?(other)
        
        os, oe = other.range
        
        # Make sure that the 2 blocks are valid
        if os == @start and oe == @end
          raise Compiler::Error, "Invalid exception blocking detected"
        end
        
        if @start < os and @end >= os and @end <= oe
          raise Compiler::Error, "Overlapping exception ranges"
        end
        
        if os < @start and oe >= @start and oe <= @end
          raise Compiler::Error, "Overlapping exception ranges"
        end
        
        # Now, they're either disjoined or one is a subrange.
        
        # If self is a sub-region of other, then it's
        # less than other.
        return -1 if @start >= os and @end <= oe
        return  1 if os >= @start and oe <= @end
        
        # Ok, they're disjoined.
        
        @start <=> os        
      end
      
      def inspect
        "#<#{self.class}:0x#{object_id.to_s(16)} start=#{@start} handler=#{@handler}>"
      end
    end
    
    def exceptions
      ex = ExceptionBlock.new(self)
      ex.start!
      @exceptions << ex
      yield ex
    end

    def next_cache_index
      # Reserve first slot for call sites that are not cached
      @cache_size = 1 if @cache_size == 0
      i = @cache_size
      @cache_size += 1
      i
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

    # The max value we use for inline ints. Above this, they're
    # stored in the literals tuple.
    InlineIntCutoff = (2 ** 27)
    
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
        if int.abs < InlineIntCutoff
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
      add :push_const, find_literal(name)
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
    
    def open_class(name)
      add :open_class, find_literal(name)
    end
    
    def open_class_under(name)
      add :open_class_under, find_literal(name)
    end
    
    def open_module(name)
      add :open_module, find_literal(name)      
    end
    
    def open_module_under(name)
      add :open_module_under, find_literal(name)      
    end
    
    def dup
      add :dup_top
    end
    
    def swap
      add :swap_stack
    end
    
    def gif(lbl)
      add :goto_if_false, lbl
    end
    
    def git(lbl)
      add :goto_if_true, lbl
    end
    
    def goto(lbl)
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
    
    def send(meth, count, priv=false)
      add :set_call_flags, 1 if priv

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
      add :set_call_flags, 1 if priv
      
      idx = find_literal(meth)
      add :send_stack_with_block, idx, count
    end
    
    def send_with_register(meth, priv=false)
      add :set_call_flags, 1 if priv
      
      idx = find_literal(meth)
      add :send_with_arg_register, idx      
    end
    
    def send_super(meth, args=nil)
      idx = find_literal(meth)
      if args
        add :send_super_stack_with_block, idx, args
      else
        add :send_super_with_arg_register, idx
      end
    end

    def check_serial(sym, serial)
      idx = find_literal(sym)
      add :check_serial, idx, serial.to_i
    end
    
    def method_missing(*op)
      add *op
    end    
  end
end
