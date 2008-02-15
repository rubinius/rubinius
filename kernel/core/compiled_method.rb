# depends on: class.rb array.rb

##
# A wrapper for a calling a function in a shared library.

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

  def initialize(mod, par=nil)
    @module = mod
    @parent = par
  end

  attr_accessor :script

  def script
    @script
  end

  def module
    @module
  end
  
  def parent
    @parent
  end  
end

class CompiledMethod
  # TODO: Delete cache field (field 14) from C structure
  ivar_as_index :__ivars__ => 0, :primitive => 1, :required => 2, :serial => 3, :bytecodes => 4, :name => 5, :file => 6, :locals => 7, :literals => 8, :arguments => 9, :scope => 10, :exceptions => 11, :lines => 12, :path => 13, :bonus => 15, :compiled => 16, :staticscope => 17, :args => 18
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
  def bonus     ; @bonus      ; end
  def compiled  ; @compiled   ; end
  def staticscope; @staticscope; end
  def args      ; @args       ; end

  ##
  # This is runtime hints, added to the method by the VM to indicate how it's
  # being used.

  attr_accessor :hints
  
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
  
  def inherit_scope(other)
    if ss = other.staticscope
      @staticscope = ss
    else
      @staticscope = StaticScope.new(Object)
    end
  end

  def staticscope=(val)
    raise TypeError, "not a static scope: #{val.inspect}" unless val.kind_of? StaticScope
    @staticscope = val
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

  ##
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
  
  def serial=(ser)
    @serial = ser
  end

  def bonus=(tup)
    @bonus = tup
  end
  
  def args=(ary)
    @args = ary
  end
  
  def local_names
    return nil unless @bonus
    @bonus[0]
  end
  
  def local_names=(names)
    return if names.nil?
    
    unless names.kind_of? Tuple
      raise ArgumentError, "only accepts a Tuple"
    end
    
    names.each do |n|
      unless n.kind_of? Symbol
        raise ArgumentError, "must be a tuple of symbols: #{n.inspect}"
      end
    end
    
    @bonus = Tuple.new(1) unless @bonus
    @bonus[0] = names
    return names
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

  class Script
    attr_accessor :path
  end
  
  def as_script(script=nil)
    script ||= CompiledMethod::Script.new
    yield script if block_given?

    Rubinius::VM.save_encloser_path
   
    # Setup the scoping.
    ss = StaticScope.new(Object)
    ss.script = script
    @staticscope = ss

    activate_as_script
    Rubinius::VM.restore_encloser_path
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

  ##
  # Decodes the instruction sequence that is represented by this compileed
  # method. Delegates to InstructionSequence to do the instruction decoding,
  # but then converts opcode literal arguments to their actual values by looking
  # them up in the literals tuple.
  def decode
    stream = @bytecodes.decode
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
  end

  ##
  # Calculates the minimum stack size required for this method.
  #
  # Returns two values:
  # * The minimum size stack required
  # * A flag indicating whether this is an exact size, or a minimum
  def min_stack_size
    dc = decode
    high_mark = 0
    exact = true
    dc.inject(0) do |sz,op|
      i,flg = op.stack_produced
      sz += i
      exact &&= flg
      i,flg = op.stack_consumed
      sz -= i
      exact &&= flg
      high_mark = sz if sz > high_mark
      sz
    end
    return high_mark, exact
  end

  class Instruction
    def initialize(inst, cm, ip, args_reg)
      @op = inst[0]
      @args = inst[1..-1]
      @args.each_index do |i|
        case @op.args[i]
        when :literal
          @args[i] = cm.literals[@args[i]]
        when :local
          # TODO: Blocks should be able to retrieve local names as well,
          # but need access to method corresponding to home context
          @args[i] = cm.local_names[args[i]] if cm.local_names and cm.name != :__block__
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

    attr_reader :ip
    attr_reader :line

    ##
    # Returns the OpCode object

    def instruction
      @op
    end

    ##
    # Returns the symbol representing the opcode for this instruction

    def opcode
      @op.opcode
    end

    ##
    # Returns an array of 0 to 2 arguments, depending on the opcode

    def args
      @args
    end

    def size
      @args.size + 1
    end

    ##
    # Returns the stack operands consumed by this instruction, as well as a flag
    # indicating whether this is an exact value (true) or a minimum (false).

    def stack_consumed
      @stack_consumed
    end

    ##
    # Returns the stack operands produced by this instruction, as well as a flag
    # indicating whether this is an exact value (true) or a minimum (false)..

    def stack_produced
      @stack_produced
    end

    ##
    # Calculate the stack usage (pushes or pops) of this instruction.

    def calculate_stack_usage(code, args_reg=0)
      usage = code
      exact = true
      if code < 0
        usage = 0
        if code == -999
          exact = false
        else
          # Stack usage depends on opcode args
          code *= -1
          mult, code = code.divmod(100)
          arg, code = code.divmod(10)
          if arg >= 1 and arg <= 2
            # Opcode consumes/produces a multiple of the value in the specified
            # opcode arg
            usage += mult * args[arg-1]
          elsif arg == 3
            # Opcode consumes number of args specified in args register
            usage += mult * args_reg
            exact = false
          end
          usage += code
        end
      end
      return usage, exact
    end

    def to_s
      str = "%04d:  %-27s" % [@ip, opcode]
      str << @args.map{|a| a.inspect}.join(', ')
    end
  end
end
