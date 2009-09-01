module Rubinius
  module AST
    class Send < Node
      attr_accessor :receiver, :name, :privately, :block

      def self.from(p, receiver, name, privately=true)
        node = Send.new p.compiler
        node.receiver = receiver
        node.name = name
        node.privately = privately
        node
      end

      def children
        [@receiver, @block]
      end

      def bytecode(g)
        pos(g)

        @receiver.bytecode(g)

        if @block
          @block.bytecode(g)
          g.send_with_block @name, 0, @privately
        else
          g.send @name, 0, @privately
        end
      end
    end

    class SendWithArguments < Send
      attr_accessor :arguments

      def self.from(p, receiver, name, arguments, privately=true)
        node = SendWithArguments.new p.compiler
        node.receiver = receiver
        node.name = name
        node.arguments = ActualArguments.from p, arguments
        node.privately = privately
        node
      end

      def children
        [@receiver, @arguments, @block]
      end

      def bytecode(g)
        pos(g)

        @receiver.bytecode(g)
        @arguments.bytecode(g)

        if @arguments.splat?
          @block ? @block.bytecode(g) : g.push(:nil)
          g.send_with_splat @name, @arguments.size, @privately, false
        elsif @block
          @block.bytecode(g)
          g.send_with_block @name, @arguments.size, @privately
        else
          g.send @name, @arguments.size, @privately
        end
      end
    end

    class AttrAssign < SendWithArguments
      attr_accessor :receiver, :name, :arguments, :privately

      def self.from(p, receiver, name, arguments)
        node = AttrAssign.new p.compiler

        node.receiver = receiver
        if receiver.kind_of? Self
          node.privately = true
        else
          node.privately = false
        end

        name_str = name.to_s
        if name_str[-1] == ?=
          node.name = name
        else
          node.name = :"#{name_str}="
        end

        case arguments
        when PushArgs
          node.arguments = arguments
        when nil
          # TODO: in masgn
        else
          node.arguments = ActualArguments.from p, arguments
        end

        node
      end

      def children
        [@receiver, @arguments]
      end

      def bytecode(g)
        @receiver.bytecode(g)
        if @arguments
          @arguments.bytecode(g)
          g.dup

          if @arguments.splat?
            g.move_down @arguments.size + 2
            g.swap
            g.push :nil
            g.send_with_splat @name, @arguments.size, @privately, true
          else
            g.move_down @arguments.size + 1
            g.send @name, @arguments.size, @privately
          end

          g.pop
        else
          g.swap
          g.send @name, 1, @privately
        end
      end
    end

    class PushArgs < Node
      attr_accessor :arguments, :value

      def self.from(p, arguments, value)
        node = PushArgs.new p.compiler
        node.arguments = arguments
        node.value = value
        node
      end

      def size
        splat? ? 1 : @arguments.size + 1
      end

      def splat?
        @arguments.kind_of? SplatValue
      end

      def children
        [@arguments, @value]
      end

      def bytecode(g)
        @arguments.bytecode(g)
        @value.bytecode(g)
      end
    end

    class BlockPass < Node
      attr_accessor :block

      def self.from(p, block)
        node = BlockPass.new p.compiler
        node.block = block
        node
      end

      def children
        [@block]
      end

      def bytecode(g)
        nil_block = g.new_label

        @block.bytecode(g)

        g.dup
        g.is_nil
        g.git nil_block

        g.push_cpath_top
        g.find_const :Proc

        g.swap
        g.send :__from_block__, 1

        nil_block.set!
      end
    end

    class ActualArguments < Node
      attr_accessor :array, :splat

      def self.from(p, arguments)
        node = ActualArguments.new p.compiler

        case arguments
        when SplatValue
          node.splat = arguments
          node.array = []
        when ConcatArgs
          node.array = arguments.array.body
          node.splat = SplatValue.from p, arguments.rest
        when ArrayLiteral
          node.array = arguments.body
        when nil
          node.array = []
        else
          node.array = [arguments]
        end

        node
      end

      def size
        @array.size
      end

      def splat?
        not @splat.nil?
      end

      def children
        children = @array.dup
        children << @splat if splat?
        children
      end

      def bytecode(g)
        @array.each { |x| x.bytecode(g) }
        @splat.bytecode(g) if @splat
      end
    end

    class Iter < Node
      attr_accessor :parent, :arguments, :body

      def self.from(p, arguments, body)
        node = Iter.new p.compiler
        node.arguments = IterArguments.from p, arguments
        node.body = body || Nil.from(p)
        node
      end

      # TODO: fix
      def argument_info
        [@arguments.arity, @arguments.optional.size, nil]
      end

      def variables
        @variables ||= {}
      end

      def allocate_slot
        variables.size
      end

      def nest_scope(scope)
        scope.parent = self
      end

      # A nested scope is looking up a local variable. If the variable exists
      # in our local variables hash, return a nested reference to it. If it
      # exists in an enclosing scope, increment the depth of the reference
      # when it passes through this nested scope (i.e. the depth of a
      # reference is a function of the nested scopes it passes through from
      # the scope it is defined in to the scope it is used in).
      def search_local(name)
        if variable = variables[name]
          CompilerNG::NestedLocalReference.new variable
        elsif reference = @parent.search_local(name)
          reference.depth += 1
          reference
        end
      end

      # If the local variable exists in this scope, set the local variable
      # node attribute to a reference to the local variable. If the variable
      # exists in an enclosing scope, set the local variable node attribute to
      # a nested reference to the local variable. Otherwise, create a local
      # variable in this scope and set the local variable node attribute.
      def assign_local_reference(var)
        if variable = variables[var.name]
          var.variable = CompilerNG::LocalReference.new variable
        elsif reference = @parent.search_local(var.name)
          reference.depth += 1
          var.variable = reference
        else
          variable = CompilerNG::LocalVariable.new allocate_slot
          variables[var.name] = variable
          var.variable = CompilerNG::LocalReference.new variable
        end
      end

      def map_iter
        visit do |result, node|
          case node
          when ClosedScope, Iter
            result = nil
          else
            node.in_block
          end

          result
        end
      end

      def children
        [@arguments, @body]
      end

      def bytecode(g)
        pos(g)

        map_iter

        desc = Compiler::MethodDescription.new @compiler.generator_class, @locals
        desc.name = :__block__
        desc.for_block = true
        desc.required = @arguments.arity
        desc.optional = @arguments.optional
        blk = desc.generator

        # Push line info down.
        blk.set_line g.line, g.file

        @arguments.bytecode(blk)

        blk.push_modifiers
        blk.break = nil
        blk.next = nil
        blk.redo = blk.new_label
        blk.redo.set!
        @body.bytecode(blk)
        blk.pop_modifiers
        blk.ret
        blk.close

        g.create_block desc
      end
    end

    class IterArguments < Node
      attr_accessor :prelude, :arity, :optional, :arguments

      def self.from(p, arguments)
        node = IterArguments.new p.compiler
        node.optional = 0

        array = []
        case arguments
        when Fixnum
          node.arity = 0
        when MAsgn
          node.arguments = arguments

          if arguments.splat
            node.optional = 1
            if arguments.left
              node.prelude = :multi
              node.arity = -(arguments.left.body.size + 1)
            else
              node.prelude = :splat
              node.arity = -1
            end
          else
            node.prelude = :multi
            node.arity = arguments.left.body.size
          end
        when nil
          node.arity = -1
        else # Assignment
          node.arguments = arguments
          node.arity = 1
          node.prelude = :single
        end

        node
      end

      def children
        @arguments ? [@arguments] : []
      end

      def arguments_bytecode(g)
        @arguments.bytecode(g) if @arguments
      end

      def bytecode(g)
        case @prelude
        when :single
          g.cast_for_single_block_arg
          arguments_bytecode(g)
          g.pop
        when :multi
          g.cast_for_multi_block_arg
          g.cast_array
          arguments_bytecode(g)
          g.pop
        when :splat
          g.cast_for_splat_block_arg
          g.cast_array
          arguments_bytecode(g)
          g.pop
        end
      end
    end

    class For < Iter
      def self.from(p, iter, arguments, body)
        node = For.new p.compiler

        node.arguments = IterArguments.from p, arguments
        node.body = body || Nil.from(p)

        method_send = Send.from p, iter, :each, false
        method_send.block = node

        method_send
      end

      def variables
        @parent.variables
      end

      def allocate_slot
        @parent.allocate_slot
      end

      def nest_scope(scope)
        scope.parent = @parent
      end

      def search_local(name)
        if reference = @parent.search_local(name)
          reference.depth += 1
          reference
        end
      end

      def assign_local_reference(var)
        unless reference = search_local(var.name)
          variable = CompilerNG::LocalVariable.new allocate_slot
          variables[var.name] = variable

          reference = CompilerNG::NestedLocalReference.new variable
          reference.depth += 1
        end

        var.variable = reference
      end
    end

    class Negate < Node
      attr_accessor :value

      def self.from(p, expr)
        node = Negate.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        pos(g)

        if @value.kind_of? NumberLiteral
          g.push(-@value.value)
        else
          @value.bytecode(g)
          g.send :"-@", 0
        end
      end
    end

    # TODO: fix superclass
    class Super < SendWithArguments
      attr_accessor :name, :block

      def self.from(p, arguments)
        node = Super.new p.compiler
        node.arguments = ActualArguments.from p, arguments
        node
      end

      def children
        [@arguments]
      end

      def block_bytecode(g)
        if @block
          @block.bytecode(g)
        else
          g.push_block
        end
      end

      def bytecode(g)
        pos(g)

        @arguments.bytecode(g)

        block_bytecode(g)

        if @arguments.splat?
          g.send_super @name, @arguments.size, true
        else
          g.send_super @name, @arguments.size
        end
      end
    end

    # TODO: fix superclass
    class Yield < SendWithArguments
      attr_accessor :flags

      def self.from(p, arguments, unwrap)
        node = Yield.new p.compiler

        if arguments.kind_of? ArrayLiteral and not unwrap
          arguments = ArrayLiteral.from p, [arguments]
        end

        node.arguments = ActualArguments.from p, arguments

        node
      end

      def children
        [@arguments]
      end

      def bytecode(g)
        pos(g)

        @arguments.bytecode(g)

        if @arguments.splat?
          g.yield_splat @arguments.size
        else
          g.yield_stack @arguments.size
        end
      end
    end

    class ZSuper < Super
      def self.from(p)
        ZSuper.new p.compiler
      end

      def block_bytecode(g)
        g.push_block
      end

      def bytecode(g)
        super(g)
      end
    end
  end
end
