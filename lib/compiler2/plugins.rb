require 'compiler2/execute'

module Compiler2::Plugins
  
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
      Compiler2::Plugins.add_plugin name, self
    end
    
    def call_match(c, const, method)
      return false unless c.call?
      return false unless c.method == method
      return false unless c.object.kind_of? Compiler2::Node::ConstFind
      return false unless c.object.name == const
      return true
    end
  end
  
  class BlockGiven < Plugin
    
    plugin :block_given
    
    def handle(g, call)
      if call.fcall?
        if call.method == :block_given? or call.method == :iterator?
          g.push :true
          g.push_block
          g.is_nil
          g.equal
          return true
        end
      end
      
      return false
    end
  end
  
  class PrimitiveDeclaration < Plugin
    
    plugin :primitive
    
    def handle(g, call)
      return false unless call_match(call, :Ruby, :primitive)
      
      prim = call.arguments.first.value
      
      g.as_primitive prim
      
      return true
    end
  end
  
  class InlineAssembly < Plugin
    
    plugin :assembly
    
    def handle(g, call)
      return false unless call_match(call, :Rubinius, :asm)
      return false unless call.block
      
      exc = Compiler2::ExecuteContext.new(g)
      i = 0
      args = call.arguments
      
      call.block.arguments.names.each do |name|
        exc.set_local name, args[i]
        i += 1
      end
      
      exc.execute call.block.body
      
      return true
    end
    
  end
  
  class VMOperation < Plugin
    plugin :vmops
    
    def handle(g, call)
      return false unless call_match(call, :Rubinius, :vmop)
      
      name = call.arguments.shift.value
      
      send("op_#{name}", g, call)      
    end
    
    def op_kind_of(g, call)
      call.arguments.shift.bytecode(g)
      g.push :self
      g.kind_of
    end
    
    def op_locate(g, call)
      g.push :self
      call.arguments[0].bytecode(g)
      call.arguments[1].bytecode(g)
      g.locate_method
    end
    
    def op_push_block(g, call)
      g.push_block
    end
    
    def op_send(g, call)
      call.arguments[0].bytecode(g)
      g.push_array
      g.push :self
      call.arguments[1].bytecode(g)
      call.arguments[2].bytecode(g)
      call.arguments[3].bytecode(g)
      g.set_args
      g.set_call_flags 1
      g.send_off_stack
    end
    
    def op_activate(g, call)
      
    end
    
    def op_string_append(g, call)
      call.arguments[1].bytecode(g)
      call.arguments[0].bytecode(g)
      g.string_append
    end
    
    def op_string_dup(g, call)
      call.arguments[0].bytecode(g)
      g.string_dup
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
