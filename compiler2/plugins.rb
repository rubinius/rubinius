module Compiler::Plugins
  
  @plugins = {}
  
  def self.add_plugin(name, cls)
    @plugins[name] = cls
  end
  
  def self.find_plugin(name)
    @plugins[name]
  end
  
  class Plugin
    def initialize(compiler)
      @compiler = compiler
    end
    
    def self.plugin(name)
      Compiler::Plugins.add_plugin name, self
    end
  end
  
  class BlockGiven < Plugin
    
    plugin :block_given
    
    def handle(g, call)
      if call.fcall?
        if call.method == :block_given? or call.method == :iterator?
          g.push :true
          g.send_primitive :block_given, 0
          return true
        end
      end
      
      return false
    end
  end
  
  class PrimitiveDeclaration < Plugin
    
    plugin :primitive
    
    def handle(g, call)
      return false unless g.ip == 0
      return false unless call.method == :primitive
      return false unless call.call?
      return false unless call.object.kind_of? Compiler::Node::ConstFind
      return false unless call.object.name == :Ruby
      
      prim = call.arguments.first.value
      
      g.as_primitive prim
      
      return true
    end
  end
  
  class FastMathOperators < Plugin
    
    plugin :fastmath
    
    MetaMath = {
      :+ =>    :meta_send_op_plus,
      :- =>    :meta_send_op_minus,
      :== =>   :meta_send_op_equal,
      :"!=" => :meta_send_op_nequal,
      :=== =>  :meta_send_op_tequal,
      :< =>    :meta_send_op_lt,
      :> =>    :meta_send_op_gt
    }
    
    def handle(g, call)
      name = MetaMath[call.method]
      
      if name and call.argcount == 1
        call.emit_args(g)
        call.receiver_bytecode(g)
        g.add name
        return true
      end
      
      return false
    end
  end
  
  # This are not currently used.
  class SystemMethods < Plugin
    
    plugin :fastsystem
    
    Methods = {
      :kind_of? =>      :kind_of,
      :instance_of? =>  :instance_of,
      :nil? =>          :is_nil,
      :equal? =>        :equal,
      :class =>         :class
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
      :Fixnum =>    :is_fixnum,
      :Symbol =>    :is_symbol,
      :NilClass =>  :is_nil
    }
    
    def handle(g, call)
      return false if call.block
      
      name = Methods[call.method]
                  
      return false unless name
      return false unless Args[call.method] == call.argcount
            
      if call.argcount == 0
        call.receiver_bytecode(g)
        g.add name
        return true
      end
      
      if call.argcount == 1
        cls = call.arguments.last.kind_of? Const
      end
    end
    
  end
end