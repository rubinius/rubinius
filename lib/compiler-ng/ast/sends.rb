module Rubinius
  module AST
    class Send < Node
      attr_accessor :receiver, :name, :privately, :block, :variable

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

        return @variable.get_bytecode(g) if @variable

        @receiver.bytecode(g)

        if @block
          @block.bytecode(g)
          g.send_with_block @name, 0, @privately
        else
          g.send @name, 0, @privately
        end
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        @receiver.receiver_defined(g, f)

        g.push_literal @name
        g.push :true
        g.send :respond_to?, 2
        g.gif f
        g.push_literal "method"
        g.goto done

        f.set!
        g.push :nil

        done.set!
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

      def children
        [@receiver, @arguments]
      end

      def bytecode(g)
        @receiver.bytecode(g)
        if g.state.masgn?
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
        return masgn_bytecode(g) if g.state.masgn?

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
      include CompilerNG::LocalVariables

      attr_accessor :parent, :arguments, :body

      def initialize(line, arguments, body)
        @line = line
        @arguments = IterArguments.new line, arguments
        @body = body || Nil.new(line)
      end

      def module?
        false
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
          variable.nested_reference
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
          var.variable = variable.reference
        elsif reference = @parent.search_local(var.name)
          reference.depth += 1
          var.variable = reference
        else
          variable = CompilerNG::LocalVariable.new allocate_slot
          variables[var.name] = variable
          var.variable = variable.reference
        end
      end

      def children
        [@arguments, @body]
      end

      def bytecode(g)
        pos(g)

        state = g.state
        state.scope.nest_scope self

        # TODO: remove MethodDescription and replace with constructor method
        desc = Compiler::MethodDescription.new g.class, @locals
        desc.name = :__block__
        desc.for_block = true
        desc.required = @arguments.arity
        desc.optional = @arguments.optional

        blk = desc.generator
        blk.push_state self
        blk.state.push_super state.super
        blk.file = g.file
        blk.name = :__block__

        blk.for_block = true

        blk.required_args = @arguments.required_args
        blk.total_args = @arguments.total_args
        blk.splat_index = @arguments.splat_index

        # Push line info down.
        pos(blk)

        @arguments.bytecode(blk)

        blk.state.push_block
        blk.push_modifiers
        blk.break = nil
        blk.next = nil
        blk.redo = blk.new_label
        blk.redo.set!
        @body.bytecode(blk)
        blk.pop_modifiers
        blk.state.pop_block
        blk.ret
        blk.close
        blk.pop_state

        blk.local_count = local_count
        blk.local_names = local_names

        g.create_block desc
      end
    end

    class IterArguments < Node
      attr_accessor :prelude, :arity, :optional, :arguments, :splat_index

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
            arguments.splat = arguments.splat.value

            @optional = 1
            if arguments.left
              @prelude = :multi
              @arity = -(arguments.left.body.size + 1)
            else
              @prelude = :splat
              @arity = -1
            end
          elsif arguments.left
            @prelude = :multi
            @arity = arguments.left.body.size
          else
            @prelude = :multi
            @arity = -1
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

      # TODO: decide whether to use #arity or #required_args uniformly
      # see FormalArguments
      alias_method :required_args, :arity
      alias_method :total_args, :arity

      def children
        @arguments ? [@arguments] : []
      end

      def names
        case @arguments
        when MAsgn
          if arguments = @arguments.left.body
            array = arguments.map { |x| x.name }
          else
            array = []
          end

          if @arguments.splat.kind_of? SplatAssignment
            array << @arguments.splat.name
          end

          array
        when nil
          []
        else
          [@arguments.name]
        end
      end

      def arguments_bytecode(g)
        g.state.push_masgn
        @arguments.bytecode(g) if @arguments
        g.state.pop_masgn
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

          reference = variable.nested_reference
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

        @name = g.state.super.name if g.state.super?

        @arguments.bytecode(g)

        block_bytecode(g)

        if @arguments.splat?
          g.send_super @name, @arguments.size, true
        else
          g.send_super @name, @arguments.size
        end
      end

      def defined(g)
        g.push :nil
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
        @argument_count = @arguments.size
        @yield_splat = false

        if @arguments.splat?
          splat = @arguments.splat.value
          if (splat.kind_of? ArrayLiteral or splat.kind_of? EmptyArray) and not unwrap
            @argument_count += 1
          else
            @yield_splat = true
          end
        end
      end

      def children
        [@arguments]
      end

      def bytecode(g)
        pos(g)

        @arguments.bytecode(g)

        if @yield_splat
          g.yield_splat @argument_count
        else
          g.yield_stack @argument_count
        end
      end

      def defined(g)
        t = g.new_label
        f = g.new_label

        g.push_block
        g.git t
        g.push :nil
        g.goto f

        t.set!
        g.push_literal "yield"

        f.set!
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
        if g.state.super?
          arguments = g.state.super.arguments
          @arguments = arguments.to_actual @line
          @block = arguments.block_arg
        end

        super(g)
      end
    end
  end
end
