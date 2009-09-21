module Rubinius
  module AST
    class Send < Node
      attr_accessor :receiver, :name, :privately, :block

      def initialize(line, receiver, name, privately=false)
        @line = line
        @receiver = receiver
        @name = name
        @privately = privately
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

      def initialize(line, receiver, name, arguments, privately=false)
        super line, receiver, name, privately
        @arguments = ActualArguments.new line, arguments
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

    class AttributeAssignment < SendWithArguments
      def initialize(line, receiver, name, arguments)
        @line = line

        @receiver = receiver
        @privately = receiver.kind_of?(Self) ? true : false

        @name = :"#{name}="

        @arguments = ActualArguments.new line, arguments
      end

      def in_masgn
        @in_masgn = true
      end

      def children
        [@receiver, @arguments]
      end

      def bytecode(g)
        @receiver.bytecode(g)
        if @in_masgn
          g.swap
          g.send @name, 1, @privately
        else
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
        end
      end
    end

    class ElementAssignment < SendWithArguments
      def initialize(line, receiver, arguments)
        @line = line

        @receiver = receiver
        @privately = receiver.kind_of?(Self) ? true : false

        @name = :[]=

        case arguments
        when PushArgs
          @arguments = arguments
        else
          @arguments = ActualArguments.new line, arguments
        end
      end

      def in_masgn
        @in_masgn = true
      end

      def children
        [@receiver, @arguments]
      end

      def masgn_bytecode(g)
        @receiver.bytecode(g)
        g.swap
        @arguments.masgn_bytecode(g)
        g.send @name, @arguments.size + 1, @privately
        # TODO: splat
      end

      def bytecode(g)
        return masgn_bytecode(g) if @in_masgn

        @receiver.bytecode(g)
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
      end
    end

    class PushArgs < Node
      attr_accessor :arguments, :value

      def initialize(line, arguments, value)
        @line = line
        @arguments = arguments
        @value = value
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

      def initialize(line, block)
        @line = line
        @block = block
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

      def initialize(line, arguments=nil)
        @line = line

        case arguments
        when SplatValue
          @splat = arguments
          @array = []
        when ConcatArgs
          @array = arguments.array.body
          @splat = SplatValue.new line, arguments.rest
        when ArrayLiteral
          @array = arguments.body
        when nil
          @array = []
        else
          @array = [arguments]
        end
      end

      def size
        @array.size
      end

      def splat?
        not @splat.nil?
      end

      def children
        @array.dup << @splat
      end

      def masgn_bytecode(g)
        @array.each do |x|
          x.bytecode(g)
          g.swap
        end
        # TODO: splat
      end

      def bytecode(g)
        @array.each { |x| x.bytecode(g) }
        @splat.bytecode(g) if @splat
      end
    end

    class Iter < Node
      attr_accessor :parent, :arguments, :body

      def initialize(line, arguments, body)
        @line = line
        @arguments = IterArguments.new line, arguments
        @body = body || Nil.new(line)
      end

      def module?
        false
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

        # TODO: remove MethodDescription and replace with constructor method
        desc = Compiler::MethodDescription.new g.class, @locals
        desc.name = :__block__
        desc.for_block = true
        desc.required = @arguments.arity
        desc.optional = @arguments.optional
        blk = desc.generator
        blk.file = g.file

        # Push line info down.
        pos(blk)

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

      def initialize(line, arguments)
        @line = line
        @optional = 0

        array = []
        case arguments
        when Fixnum
          @arity = 0
        when MAsgn
          arguments.iter_arguments

          if arguments.splat
            @optional = 1
            if arguments.left
              @prelude = :multi
              @arity = -(arguments.left.body.size + 1)
            else
              @prelude = :splat
              @arity = -1
            end
          else
            @prelude = :multi
            @arity = arguments.left.body.size
          end

          @arguments = arguments
        when nil
          @arity = -1
        else # Assignment
          @arguments = arguments
          @arity = 1
          @prelude = :single
        end
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
      def initialize(line, arguments, body)
        @line = line
        @arguments = IterArguments.new line, arguments
        @body = body || Nil.new(line)
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

      def initialize(line, value)
        @line = line
        @value = value
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

    class Super < SendWithArguments
      attr_accessor :name, :block

      def initialize(line, arguments)
        @line = line
        @arguments = ActualArguments.new line, arguments
      end

      def children
        [@arguments, @block]
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

    class Yield < SendWithArguments
      attr_accessor :flags

      def initialize(line, arguments, unwrap)
        @line = line

        if arguments.kind_of? ArrayLiteral and not unwrap
          arguments = ArrayLiteral.new line, [arguments]
        end

        @arguments = ActualArguments.new line, arguments
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
      def initialize(line)
        @line = line
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
