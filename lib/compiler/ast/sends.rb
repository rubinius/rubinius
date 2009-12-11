module Rubinius
  module AST
    class Send < Node
      attr_accessor :receiver, :name, :privately, :block, :variable
      attr_accessor :check_for_local

      def initialize(line, receiver, name, privately=false)
        @line = line
        @receiver = receiver
        @name = name
        @privately = privately
        @block = nil
        @check_for_local = false
      end

      def bytecode(g)
        pos(g)

        if @receiver.kind_of? Self and (@check_for_local or g.state.eval?)
          if reference = g.state.scope.search_local(@name)
            return reference.get_bytecode(g)
          end
        end

        @receiver.bytecode(g)

        if @block
          @block.bytecode(g)
          g.send_with_block @name, 0, @privately
        else
          g.send @name, 0, @privately
        end
      end

      def receiver_defined(g, f)
        bytecode(g)
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
        @block = nil
        @arguments = ActualArguments.new line, arguments
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

      def bytecode(g)
        @arguments.bytecode(g)
        @value.bytecode(g)
      end
    end

    class BlockPass < Node
      attr_accessor :body

      def initialize(line, body)
        @line = line
        @body = body
      end

      def convert(g)
        nil_block = g.new_label
        g.dup
        g.is_nil
        g.git nil_block

        g.push_cpath_top
        g.find_const :Proc

        g.swap
        g.send :__from_block__, 1

        nil_block.set!
      end

      def bytecode(g)
        @body.bytecode(g)
        convert(g)
      end

      def assignment_bytecode(g)
        g.push_block_arg
        convert(g)
        @body.bytecode(g)
      end
    end

    class ActualArguments < Node
      attr_accessor :array, :splat

      def initialize(line, arguments=nil)
        @line = line
        @splat = nil

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

      def new_local(name)
        variable = CompilerNG::LocalVariable.new allocate_slot
        variables[name] = variable
      end

      def new_nested_local(name)
        new_local(name).nested_reference
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
          variable = new_local var.name
          var.variable = variable.reference
        end
      end

      def bytecode(g)
        pos(g)

        state = g.state
        state.scope.nest_scope self

        blk = new_block_generator g, @arguments

        blk.push_state self
        blk.state.push_super state.super
        blk.state.push_eval state.eval

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

        blk.splat_index = @arguments.splat_index
        blk.local_count = local_count
        blk.local_names = local_names

        g.create_block blk
      end
    end

    class IterArguments < Node
      attr_accessor :prelude, :arity, :optional, :arguments, :splat_index
      attr_accessor :required_args

      def initialize(line, arguments)
        @line = line
        @optional = 0

        @splat_index = -1
        @required_args = 0
        @splat = nil
        @block = nil

        array = []
        case arguments
        when Fixnum
          @arity = 0
          @prelude = nil
        when MAsgn
          arguments.iter_arguments

          if arguments.splat
            @splat = arguments.splat = arguments.splat.value

            @optional = 1
            if arguments.left
              @prelude = :multi
              @arity = -(arguments.left.body.size + 1)
              @required_args = arguments.left.body.size
            else
              @prelude = :splat
              @arity = -1
            end
          elsif arguments.left
            @prelude = :multi
            @arity = arguments.left.body.size
            @required_args = arguments.left.body.size
          else
            @prelude = :multi
            @arity = -1
          end

          @block = arguments.block

          @arguments = arguments
        when nil
          @arity = -1
          @splat_index = -2 # -2 means accept the splat, but don't store it anywhere
          @prelude = nil
        when BlockPass
          @arity = -1
          @splat_index = -2
          @prelude = nil
          @block = arguments
        else # Assignment
          @arguments = arguments
          @arity = 1
          @required_args = 1
          @prelude = :single
        end
      end

      alias_method :total_args, :required_args

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

        if @splat
          @splat_index = @splat.variable.slot
        end
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

        if @block
          @block.assignment_bytecode(g)
        end
      end
    end

    class For < Iter
      def nest_scope(scope)
        scope.parent = self
      end

      def search_local(name)
        if reference = @parent.search_local(name)
          reference.depth += 1
          reference
        end
      end

      def new_nested_local(name)
        reference = @parent.new_nested_local name
        reference.depth += 1
        reference
      end

      def assign_local_reference(var)
        unless reference = search_local(var.name)
          reference = new_nested_local var.name
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
        @block = nil
        @arguments = ActualArguments.new line, arguments
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
        nope = g.new_label
        done = g.new_label

        g.push_variables
        g.send :super_method_defined?, 0

        g.gif nope

        g.push_literal "super"
        g.string_dup
        g.goto done

        nope.set!
        g.push :nil

        done.set!
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
        @block = nil
      end

      def bytecode(g)
        pos(g)

        @name = g.state.super.name if g.state.super?

        block_bytecode(g)

        g.zsuper @name
      end
    end
  end
end
