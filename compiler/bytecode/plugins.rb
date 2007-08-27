class Bytecode::Compiler
  class Plugin
    
    def self.flag(n=nil)
      if n
        @flag = n
      else
        @flag
      end
    end

    def initialize(compiler)
      @compiler = compiler
    end
    
    def flag?(val)
      @compiler.flag?(val)
    end
    
    def process(what)
      @compiler.process(what)
    end
    
    def add(what)
      @compiler.add(what)
    end
    
    def capture(&b)
      @compiler.capture(&b)
    end
    
    def temp
      name = "+temp_#{@compiler.unique_id}"
      
    end
    
    def run(recv, meth, args, block)
      return false unless @compiler.flag?(self.class.flag)
      handle(recv, meth, args, block)
    end
  end
end

class MetaOperatorPlugin < Bytecode::Compiler::Plugin
  
  flag :fast_math
  
  MetaMath = {
    :+ =>   "meta_send_op_plus",
    :- =>   "meta_send_op_minus", 
    :== =>  "meta_send_op_equal",
    :"!=" =>  "meta_send_op_nequal",
    :=== => "meta_send_op_tequal",
    :< =>   "meta_send_op_lt", 
    :> =>   "meta_send_op_gt"
  }
  
  def handle(recv, meth, args, block)
    return false if block
    
    name = MetaMath[meth]
    if name and args and args.size == 2
      process args.last
      process recv
      add name
      return true
    end
    
    return false
  end
  
end

class SystemMethodPlugin < Bytecode::Compiler::Plugin
  
  flag :fast_system
  
  Methods = {
    :kind_of? => "kind_of",
    :instance_of? => "instance_of",
    :nil? => "is_nil",
    :equal? => "equal",
    :class => "class"
  }
  
  # How many arguments each method takes.
  Args = {
    :kind_of?     => 1,
    :instance_of? => 1,
    :nil?         => 0,
    :equal?       => 1,
    :class        => 0
  }
  
  # How to map class checks directly to instructions.
  # I highly doubt anyone does the last one, but it's here
  # for completeness.
  KindOf = {
    :Fixnum =>    "is_fixnum",
    :Symbol =>    "is_symbol",
    :NilClass =>  "is_nil"
  }
  
  def handle(recv, meth, args, block)
    return false if block
    
    name = Methods[meth]
    
    return false unless name and args
    
    # Check that the arg count is correct for this method.
    return false unless Args[meth] == args.size - 1
    
    if args.size == 1    
      process recv
      add name
      return true
    end
    
    if args.size == 2
      cls = args.last
      # Special case for obj.kind_of?(Fixnum)
      if meth == :kind_of? and cls.first == :const
        if op = KindOf[cls.last]
          process recv
          add op
          return true
        end
      end

      process cls
      process recv
      add name
      return true
    end
    
    return false
  end
end

class NamedSendPlugin < Bytecode::Compiler::Plugin
  
  flag :fast_send_method
  
  def handle(recv, meth, args, block)
    return false if block
    return false unless meth == :__send__
    
    args.shift
    name = args.shift
    return false unless name
    return false if name.first != :lit
    
    count = args.size
    args.reverse.each do |a|
      process a
    end
    
    process recv
    add "push nil"
    add "push :#{name.last}"
    add "push #{count}"
    add "set_args"
    add "send_off_stack"
    return true
    
  end
end

=begin

class FastTimesPlugin < Bytecode::Compiler::Plugin
  
  flag :fast_times
  
  UnrollThreshold = 5
  
  def handle(recv, meth, args, block)
    # Only do this if you're sending times to a literal number.
    if meth != :times or recv.first := :lit and !recv.last.kind_of?(Fixnum)
      return false
    end
    
    count = recv.last
    if count < UnrollThreshold
      code = capture do
        process block
      end
      
      count.times do
        add code
      end
    end
    
    return false
  end
end


this is premature

class FastSplatAccess < Bytecode::Compiler::Plugin
  def process(recv, meth, args)
    return false if meth != :[] or args.size != 2
    return false if recv.first != :lvar
    
    lv = recv[2]    
    
    return false unless lv.magic == :splat
    return false if lv.assigned? or lv.passed_as_argument?
    
    if 
    return false if     
  end
end
=end
