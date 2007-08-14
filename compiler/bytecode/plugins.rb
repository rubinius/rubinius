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
  
  def handle(recv, meth, args)
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
