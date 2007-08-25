class Bytecode::Compiler
  class Plugin
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
  end
end

class MetaOperatorPlugin < Bytecode::Compiler::Plugin
  
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
    
    if flag?(:fast_math)
      name = MetaMath[meth]
      if name and args and args.size == 2
        process args.last
        process recv
        add name
        return true
      end
    end
    
    return false
  end
  
end

class SystemMethodPlugin < Bytecode::Compiler::Plugin
  
  Methods = {
    :kind_of? => "kind_of",
    :instance_of? => "instance_of"
  }
  
  def handle(recv, meth, args, block)
    return false if block
    
    name = Methods[meth]
    
    if name and args and args.size == 2
      process args.last
      process recv
      add name
      return true
    end
    
    return false
  end
end

class NamedSendPlugin < Bytecode::Compiler::Plugin
  
  def handle(recv, meth, args, block)
    return false if block
    return false unless meth == :__send__
    count = args.size - 1
    args.shift # remove :array
    args.reverse.each do |a|
      process a
    end
    
    process recv
    add "push nil"
    add "push :#{meth}"
    add "push #{count}"
    add "set_args"
    add "send_off_stack"
    return true
    
  end
end

class FastTimesPlugin < Bytecode::Compiler::Plugin
  
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
    else
      
    end
  end
end

=begin

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
