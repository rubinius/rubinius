require 'compiler/execute'

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
    
    def call_match(c, const, method)
      return false unless c.call?
      return false unless c.method == method
      return false unless c.object.kind_of? Compiler::Node::ConstFind
      return false unless c.object.name == const
      return true
    end
  end
  
  class BlockGiven < Plugin
    
    plugin :block_given
    
    def handle(g, call)
      if call.fcall?
        if call.method == :block_given? or call.method == :iterator?
          g.push_block
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
      
      exc = Compiler::ExecuteContext.new(g)
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
 
  class CurrentMethod < Plugin

    plugin :current_method

    def handle(g, call)
      return false unless call.kind_of? Compiler::Node::VCall
      if call.method == :__METHOD__
        g.push_context
        g.send :method, 0
        return true
      end

      return false
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
  
  class SafeMathOperators < Plugin
    
    plugin :safemath
    
    MathOps = {
      :/ => :divide
    }
    
    def handle(g, call)
      name = MathOps[call.method]
      if name and call.argcount == 1
        call.emit_args(g)
        call.receiver_bytecode(g)
        g.send name, 1, false
        return true
      end
      return false
    end
  end

  class CompilerInlining < Plugin
    plugin :inline

    Methods = {
      :times => :fixnum_times
    }

    def handle(g, call)
      return false unless call.block.kind_of? Compiler::Node::Iter
      if handler = Methods[call.method]
        return __send__(handler, g, call)
      end

      return false
    end

    def fixnum_times(g, call)
      return false unless call.no_args?
      return false unless call.block.arguments.names.empty?

      do_call = g.new_label

      # Since there are no args, this just makes sure that the internals
      # are setup properly.
      call.emit_args(g)

      call.receiver_bytecode(g)
      g.dup
      g.is_fixnum
      g.gif do_call
      g.dup
      g.check_serial :times, 0
      g.gif do_call

      done = g.new_label
      
      desc = Compiler::MethodDescription.new @compiler.generator_class, call.block.locals
      desc.name = :__inlined_block__
      desc.required, desc.optional = call.block.argument_info
      sub = desc.generator

      sub.set_line g.line, g.file

      @compiler.show_errors(sub) do
        sub.push_modifiers

        top = sub.new_label
        fin = sub.new_label

        sub.next = top
        sub.break = fin
        sub.redo = sub.new_label
        
        # Get rid of the block args.
        sub.unshift_tuple

        # To the times logic now, calling block.body
       
        top.set!

        # Check if the value is 0
        sub.dup
        sub.push 0
        sub.equal
        sub.git fin

        sub.redo.set!

        call.block.body.bytecode(sub)

        # Subtract 1 from the value
        sub.pop
        sub.push 1
        sub.swap
        sub.meta_send_op_minus

        sub.goto top

        sub.pop_modifiers
        fin.set!
        sub.push :self
        sub.soft_return
        sub.close
      end

      g.push_literal desc
      g.create_block2
      g.send :call, 1

      g.goto done
      do_call.set!
      
      call.block.bytecode(g)
      g.swap
      call.block_bytecode(g)

      done.set!
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
