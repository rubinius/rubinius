require 'compiler/execute'

class Compiler

  ##
  # A namespace for compiler plugins.  A plugin emits custom bytecode for an
  # expression

  module Plugins

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

      def self.plugin(name, kind=:call)
        @kind = kind
        Plugins.add_plugin name, self
      end

      def self.kind
        @kind
      end

      def call_match(c, const, method)
        return false unless c.call?
        return false unless c.method == method
        return false unless c.object.kind_of? Node::ConstFind
        return false unless c.object.name == const
        return true
      end
    end

    ##
    # Handles block_given?

    class BlockGiven < Plugin

      plugin :block_given

      def handle(g, call)
        if call.call? and call.object.nil?
          if call.method == :block_given? or call.method == :iterator?
            g.push_block
            return true
          end
        end

        return false
      end
    end

    ##
    # Handles Ruby.primitive

    class PrimitiveDeclaration < Plugin

      plugin :primitive

      def handle(g, call)
        return false unless call_match(call, :Ruby, :primitive)

        prim = call.arguments.first.value

        g.as_primitive prim

        return true
      end
    end

    ##
    # Handles Rubinius.asm

    class InlineAssembly < Plugin

      plugin :assembly

      def handle(g, call)
        return false unless call_match(call, :Rubinius, :asm)
        return false unless call.block

        exc = ExecuteContext.new(g)
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

    ##
    # Handles __METHOD__

    class CurrentMethod < Plugin

      plugin :current_method

      def handle(g, call)
        return false # HACK: should test for empty arguments and nil reciever
#         return false unless call.kind_of? Node::Call
#         if call.method == :__METHOD__
#           g.push_context
#           g.send :method, 0
#           return true
#         end

#         return false
      end
    end

    ##
    # Handles emitting fast VM instructions for certain math operations.

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
          call.receiver_bytecode(g)
          call.emit_args(g)
          g.add name
          return true
        end

        return false
      end
    end

    ##
    # Handles emitting fast VM instructions for certain methods.

    class FastGenericMethods < Plugin

      plugin :fastgeneric

      Methods = {
        :call => :meta_send_call
      }

      def handle(g, call)
        return false unless call.method == :call

        # Don't handle send's with a block or non static args.
        return false if call.block or !call.static_args?

        if name = Methods[call.method]
          call.receiver_bytecode(g)
          call.emit_args(g)
          g.add name, call.argcount
          return true
        end

        return false
      end
    end

    ##
    # Handles emitting save VM instructions for redifinition of math operators.

    class SafeMathOperators < Plugin

      plugin :safemath

      MathOps = {
        :/ => :divide
      }

      def handle(g, call)
        name = MathOps[call.method]
        if name and call.argcount == 1
          call.receiver_bytecode(g)
          call.emit_args(g)
          g.send name, 1, false
          return true
        end
        return false
      end
    end

    ##
    # Handles constant folding

    class ConstantExpressions < Plugin

      plugin :const_epxr

      MathOps = [:+, :-, :*, :/, :**]

      def handle(g, call)
        op = call.method
        return false unless MathOps.include? op and call.argcount == 1

        obj = call.object
        arg = call.arguments.first
        if call.object.kind_of? Node::NumberLiteral and call.arguments.first.kind_of? Node::NumberLiteral
          res = obj.value.__send__(op, arg.value)
          if res.kind_of? Fixnum
            g.push_int res
          else
            g.push_literal res
          end
          return true
        end
        return false
      end
    end

    ##
    # Prototype plugin for handling inlining.  Not in use.

    class CompilerInlining < Plugin
      plugin :inline

      Methods = {
        :times => :fixnum_times
      }

      def handle(g, call)
        return false unless call.block.kind_of? Node::Iter
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

        desc = MethodDescription.new @compiler.generator_class, call.block.locals
        desc.name = :__inlined_block__
        desc.required, desc.optional = call.block.argument_info
        sub = desc.generator

        sub.set_line g.line, g.file

        @compiler.show_errors(sub) do
          sub.push_modifiers

          top = sub.new_label
          fin = sub.new_label
          nxt = sub.new_label

          sub.next = nxt
          # TODO: break needs to be handled differently. See the specs for
          # a nested while with a complex break expression in the Integer#times specs.
          sub.break = fin
          sub.redo = sub.new_label

          # Get rid of the block args.
          sub.unshift_tuple
          sub.dup

          # Create the loop counter
          sub.dup
          sub.push 1
          sub.meta_send_op_plus
          sub.push :nil

          # Loop starts here
          top.set!

          # Descrement the loop counter
          sub.pop
          sub.push 1
          sub.swap
          sub.meta_send_op_minus

          # Check if the loop counter is 0
          sub.dup
          sub.push 0
          sub.equal
          sub.git fin

          # To the times logic now, calling block.body
          sub.redo.set!

          call.block.body.bytecode(sub)

          # Loop ends here
          sub.goto top

          nxt.set!
          sub.push :nil
          sub.goto top

          sub.pop_modifiers
          fin.set!
          sub.pop
          sub.ret
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

    ##
    # Maps various methods to VM instructions

    class SystemMethods < Plugin

      plugin :fastsystem

      Methods = {
        :__kind_of__ =>      :kind_of,
        :__instance_of__ =>  :instance_of,
        :__nil__ =>          :is_nil,
        :__equal__ =>        :equal,
        :__class__ =>        :class,
        :__fixnum__ =>       :is_fixnum,
        :__symbol__ =>       :is_symbol,
        :__nil__ =>          :is_nil
      }

      # How many arguments each method takes.
      Args = {
        :__kind_of__     => 1,
        :__instance_of__ => 1,
        :__nil__         => 0,
        :__equal__       => 1,
        :__class__       => 0,
        :__fixnum__      => 0,
        :__symbol__      => 0,
        :__nil__         => 0
      }

      def handle(g, call)
        return false if call.block

        name = Methods[call.method]

        return false unless name
        return false unless Args[call.method] == call.argcount

        call.emit_args(g)
        call.receiver_bytecode(g)
        g.add name
        return true
      end

    end

    ##
    # Detects common simple expressions and simplifies them

    class AutoPrimitiveDetection < Plugin
      plugin :auto_primitive, :method

      SingleInt = [[:check_argcount, 0, 0], [:push_int, :any], [:ret]]
      Literal = [[:check_argcount, 0, 0], [:push_literal, 0], [:ret]]
      Self = [[:check_argcount, 0, 0], [:push_self], [:ret]]
      Ivar = [[:check_argcount, 0, 0], [:push_ivar, 0], [:ret]]
      Field = [[:check_argcount, 0, 0], [:push_my_field, :any], [:ret]]

      def handle(g, obj, meth)
        ss = meth.generator.stream

        return true unless ss.size == 3

        gen = meth.generator

        if gen === SingleInt
          meth.generator.literals[0] = ss[1][1]
          meth.generator.as_primitive :opt_push_literal
        elsif gen === Literal
          # The value we want is already in literal 0
          meth.generator.as_primitive :opt_push_literal
        elsif gen === Self
          meth.generator.as_primitive :opt_push_self
        elsif gen === Ivar
          meth.generator.as_primitive :opt_push_ivar
        elsif gen === Field
          meth.generator.literals[0] = ss[1][1]
          meth.generator.as_primitive :opt_push_my_field
        else
          cmd = (ss[1].kind_of?(Array) ? ss[1].first : ss[1])
          case cmd
          when :push_nil
            lit = nil
          when :push_true
            lit = true
          when :push_false
            lit = false
          when :meta_push_0
            lit = 0
          when :meta_push_1
            lit = 1
          when :meta_push_2
            lit = 2
          else
            return true
          end

          meth.generator.literals[0] = lit
          meth.generator.as_primitive :opt_push_literal
        end

        true
      end
    end
  end # Plugins
end # Compiler
