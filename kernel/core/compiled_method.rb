# depends on: class.rb

class NativeMethod
  def lines
    nil
  end
  
  def exceptions
    nil
  end
  
  def literals
    nil
  end
  
  def line_from_ip(i)
    0
  end
end

class StaticScope
  ivar_as_index :__ivars__ => 0, :module => 1, :parent => 2
  
  def module
    @module
  end
  
  def parent
    @parent
  end  
end

class CompiledMethod
  ivar_as_index :__ivars__ => 0, :primitive => 1, :required => 2, :serial => 3, :bytecodes => 4, :name => 5, :file => 6, :locals => 7, :literals => 8, :arguments => 9, :scope => 10, :exceptions => 11, :lines => 12, :path => 13, :cache => 14, :bonus => 15, :compiled => 16, :staticscope => 17
  def __ivars__ ; @__ivars__  ; end
  def primitive ; @primitive  ; end
  def required  ; @required   ; end
  def serial    ; @serial     ; end
  def bytecodes ; @bytecodes  ; end
  def name      ; @name       ; end
  def file      ; @file       ; end
  def locals    ; @locals     ; end
  def literals  ; @literals   ; end
  def arguments ; @arguments  ; end
  def scope     ; @scope      ; end
  def exceptions; @exceptions ; end
  def lines     ; @lines      ; end
  def path      ; @path       ; end
  def cache     ; @cache      ; end
  def bonus     ; @bonus      ; end
  def compiled  ; @compiled   ; end
  def staticscope; @staticscope; end
  
  def inspect
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)} name=#{@name} file=#{@file}>"
  end
  
  def from_string(bc, lcls, req)
    @bytecodes = bc
    @primitive = -1
    @locals = lcls
    @literals = Tuple.new(0)
    @required = 0
    @arguments = Tuple.new(0)
    @exceptions = nil
    @lines = nil
    @file = nil
    @name = nil
    @path = nil
    @required = req
    return self
  end
  
  def exceptions=(tup)
    @exceptions = tup
  end
  
  def literals=(tup)
    @literals = tup
  end
  
  def file=(val)
    @file = val
  end
  
  def name=(val)
    @name = val
  end

  # Lines consists of an array of tuples, with each tuple representing a line.
  # The tuple for a line has fields for the first ip, last ip, and line number.
  def lines=(val)
    @lines = val
  end
  
  def path=(val)
    @path = val
  end
  
  def primitive=(idx)
    @primitive = idx
  end
  
  def cache=(tup)
    @cache = tup
  end
  
  def serial=(ser)
    @serial = ser
  end

  def bonus=(tup)
    @bonus = tup
  end
  
  def activate(recv, mod, args, locals=nil, &prc)
    sz = args.total
    if prc
      block = prc.block
    else
      block = nil
    end
    
    out = Rubinius.asm(args, block, locals, sz, mod, recv) do |a,b,l,s,m,r|
      run a
      push_array
      run b
      run l
      run s
      run m
      push :self
      run r
      activate_method 0
    end
        
    return out
  end
    
  def line_from_ip(i)
    @lines.each do |t|
      start = t.at(0)
      nd = t.at(1)
      op = t.at(2)
      if i >= start and i <= nd
        return op
      end
    end
    return 0
  end
  
  def first_ip_on_line(line)
    @lines.each do |t|
      if t.at(2) >= line
        return t.at(0)
      end
    end
    
    return -1
  end
  
  def bytecodes=(other)
    @bytecodes = other
  end
  
  def first_line
    @lines.each do |ent|
      return ent[2] if ent[2] > 0
    end
    
    return -1
  end
  
  # Decodes the instruction sequence that is represented by this compileed
  # method. Delegates to +InstructionSequence+ to do the instruction decoding,
  # but then converts opcode literal arguments to their actual values by looking
  # them up in the literals tuple.
  def decode
    stream = @bytecodes.decode
    ip = 0
    stream.map! do |inst|
      instruct = Instruction.new(inst, self, ip)
      ip += instruct.size
      instruct
    end
  end


  class Instruction
    def initialize(inst, cm, ip)
      @op = inst[0]
      @args = inst[1..-1]
      @args.each_index do |i|
        case @op.args[i]
        when :literal
          @args[i] = cm.literals[@args[i]]
        when :slot_local
          @args[i] = cm.bonus[0].at(args[i]) if cm.bonus && cm.bonus[0]
        when :stack_local
          @args[i] = cm.bonus[1].at(args[i]) if cm.bonus && cm.bonus[1]
        end
      end
      @ip = ip
      @line = cm.line_from_ip(ip)
    end

    # Returns the OpCode object
    def instruction
      @op
    end

    # Returns the symbol representing the opcode for this instruction
    def opcode
      @op.opcode
    end

    # Returns an array of 0 to 2 arguments, depending on the opcode
    def args
      @args
    end

    def size
      @args.size + 1
    end

    attr_reader :ip
    attr_reader :line

    def to_s
      str = "%04d:  %-27s" % [@ip, opcode]
      str << @args.map{|a| a.inspect}.join(', ')
    end
  end
end
