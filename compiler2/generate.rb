require 'encoder'

class Compiler
  class Generator
    
    class Label
      def initialize(generator)
        @generator = generator
        @position = nil
      end
      
      attr_accessor :position
      
      def set!
        @position = @generator.ip + 1
      end
      
    end
        
    def initialize
      @stream = []
      @encoder = Compiler::Encoder.new
      @literals = []
      @ip = 0
      @modstack = []
      @break = nil
      @redo = nil
      @next = nil
      @retry = nil
      @last_line = nil
      @lines = []
    end
    
    attr_reader :ip
    attr_accessor :break, :redo, :next, :retry
    
    def run(node)
      node.bytecode self
    end
    
    def new_label
      Label.new(self)
    end
    
    def collapse_labels
      @stream.each do |part|
        if part.kind_of? Array
          part.map! do |x|
            if x.kind_of? Label
              x.position
            else
              x
            end
          end
        end
      end
    end
    
    def push_modifiers
      @modstack << [@break, @redo, @next, @retry]
    end
    
    def pop_modifiers
      @break, @redo, @next, @retry = @modstack.pop
    end
    
    def set_line(line, file)
      if line != @last_line
        @lines << [@ip, nil, line]
        @file = file
        
        # Update the last entry to complete it
        if @last_line
          @lines.last[1] = @ip - 1
        end
        @last_line = line
      end
    end
    
    def close
      if @lines and @lines.last[1].nil?
        @lines.last[1] = @ip
      end
    end
    
    def add(*what)
      if what.size == 1
        @stream << what.first
      else
        @stream << what
      end
      
      @ip += what.size
    end
    
    def find_literal(what)
      idx = @literals.index(what)
      return idx if idx
      idx = @literals.size
      @literals << what
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
        push_int(what)
      when String, Bignum, Float
        push_literal what
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
        add :push_int, int
      end
    end
    
    def push_literal(what)
      idx = find_literal(what)
      add :push_literals, idx
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
    
    def dup
      add :dup
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
    
    def send(meth, count, priv=false)
      if priv
        add :set_call_flags, 1
      end
      
      idx = find_literal(meth)
      if count == 0
        add :send_method, idx
      elsif count < 5
        add "meta_send_stack_#{count}".to_sym, idx
      else
        add :send, idx, count
      end
    end
    
    def method_missing(op, *args)
      if @encoder.instruction?(op)
        if args.size > 0
          raise Error, "No args supported."
        end
        
        @stream << op
      else
        raise Error, "Unknown instruction '#{op}'"
      end
    end    
  end
end