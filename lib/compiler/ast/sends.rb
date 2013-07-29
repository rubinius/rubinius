# -*- encoding: us-ascii -*-

module Rubinius
  module AST
    class Send < Node
      attr_accessor :receiver, :name, :privately, :block, :variable, :vcall_style

      def initialize(line, receiver, name, privately=false, vcall_style=false)
        @line = line
        @receiver = receiver
        @name = name
        @privately = privately
        @block = nil
        @vcall_style = vcall_style
      end

      def check_local_reference(g)
        if @receiver.kind_of? Self and g.state.check_for_locals
          g.state.scope.search_local(@name)
        end
      end

      def bytecode(g)
        pos(g)

        if @vcall_style and reference = check_local_reference(g)
          return reference.get_bytecode(g)
        end

        @receiver.bytecode(g)

        if @block
          @block.bytecode(g)
          g.send_with_block @name, 0, @privately
        elsif @vcall_style
          g.send_vcall @name
        else
          g.send @name, 0, @privately
        end
      end

      def value_defined(g, f)
        # Save the current exception into a stack local
        g.push_exception_state
        outer_exc_state = g.new_stack_local
        g.set_stack_local outer_exc_state
        g.pop

        ok = g.new_label
        ex = g.new_label
        g.setup_unwind ex, RescueType

        bytecode(g)

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.push_stack_local outer_exc_state
        g.restore_exception_state
        g.goto f

        ok.set!
      end

      def defined(g)
        if @vcall_style and check_local_reference(g)
          g.push_literal "local-variable"
          return
        end

        f = g.new_label
        done = g.new_label

        @receiver.value_defined(g, f)

        g.push_literal @name

        if @vcall_style or @privately
          g.push :true
          g.send :__respond_to_p__, 2
        else
          g.push_self
          g.invoke_primitive :vm_check_callable, 3
        end
        g.gif f
        g.push_literal "method"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def sexp_name
        :call
      end

      def receiver_sexp
        @privately ? nil : @receiver.to_sexp
      end

      def arguments_sexp
        return nil if @vcall_style

        sexp = [:arglist]
        sexp << @block.to_sexp if @block.kind_of? BlockPass
        sexp
      end

      def to_sexp
        sexp = [sexp_name, receiver_sexp, @name, arguments_sexp]
        case @block
        when For
          @block.to_sexp.insert 1, @receiver.to_sexp
        when Iter
          @block.to_sexp.insert 1, sexp
        else
          sexp
        end
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
        @receiver.bytecode(g)
        @arguments.bytecode(g)

        pos(g)

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

      def arguments_sexp(name=:arglist)
        sexp = [name] + @arguments.to_sexp
        sexp << @block.to_sexp if @block
        sexp
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

      def sexp_name
        :attrasgn
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
          @arguments = PushActualArguments.new arguments
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

      def sexp_name
        :attrasgn
      end
    end

    class PreExe < Node
      attr_accessor :block

      def initialize(line)
        @line = line
      end

      def pre_bytecode(g)
        pos(g)

        g.push_state ClosedScope.new(@line)
        g.state.push_name :BEGIN

        g.push_literal Rubinius::Compiler::Runtime
        @block.bytecode(g)
        g.send_with_block :pre_exe, 0, false

        g.state.pop_name
        g.pop_state
      end

      def to_sexp
      end

      def pre_sexp
        @block.to_sexp.insert 1, :pre_exe
      end
    end

    class PreExe19 < PreExe
      def pre_bytecode(g)
        pos(g)

        @block.body.bytecode(g)
      end
    end

    class PushActualArguments
      def initialize(pa)
        @arguments = pa.arguments
        @value = pa.value
      end

      def size
        splat? ? 1 : @arguments.size + 1
      end

      def splat?
        @arguments.splat?
      end

      def bytecode(g)
        @arguments.bytecode(g)
        @value.bytecode(g)
      end

      def to_sexp
        [@arguments.to_sexp, @value.to_sexp]
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

      def to_sexp
        [:block_pass, @body.to_sexp]
      end
    end

    class BlockPass19 < BlockPass
      attr_accessor :arguments

      def initialize(line, arguments, body)
        super(line, body)
        @arguments = arguments
      end
    end

    class CollectSplat < Node
      def initialize(line, *parts)
        @line = line
        @splat = parts.shift
        @last = parts.pop
        @array = parts
      end

      def bytecode(g)
        @splat.bytecode(g)
        g.cast_array

        @array.each do |x|
          x.bytecode(g)
          g.cast_array
          g.send :+, 1
        end

        return unless @last

        not_hash = g.new_label
        done = g.new_label

        @last.bytecode(g)
        g.dup

        g.push_const :Hash

        g.push_type
        g.move_down 2
        g.send :object_kind_of?, 2
        g.gif not_hash

        g.make_array 1
        g.goto done

        not_hash.set!
        g.cast_array

        done.set!
        g.send :+, 1
      end

      def to_sexp
        [:collect_splat] + @parts.map { |x| x.to_sexp }
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
          case arguments.array
          when ArrayLiteral
            @array = arguments.array.body
            @splat = SplatValue.new line, arguments.rest
          when PushArgs
            @array = []
            node = SplatValue.new line, arguments.rest
            @splat = CollectSplat.new line, arguments.array, node
          else
            @array = []
            @splat = CollectSplat.new line, arguments.array, arguments.rest
          end
        when PushArgs
          if arguments.arguments.kind_of? ConcatArgs
            if ary = arguments.arguments.peel_lhs
              @array = ary
            else
              @array = []
            end
          else
            @array = []
          end

          node = ArrayLiteral.new line, [arguments.value]
          @splat = CollectSplat.new line, arguments.arguments, node
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

      def stack_size
        size = @array.size
        size += 1 if splat?
        size
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

      def to_sexp
        sexp = @array.map { |x| x.to_sexp }
        sexp << @splat.to_sexp if @splat
        sexp
      end
    end

    class Iter < Node
      include Compiler::LocalVariables

      attr_accessor :parent, :arguments, :body

      def initialize(line, arguments, body)
        @line = line
        @arguments = IterArguments.new line, arguments
        @body = body || NilLiteral.new(line)
      end

      # 1.8 doesn't support declared Iter locals
      def block_local?(name)
        false
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
        elsif block_local?(name)
          new_local name
        elsif reference = @parent.search_local(name)
          reference.depth += 1
          reference
        end
      end

      def new_local(name)
        variables[name] ||= Compiler::LocalVariable.new allocate_slot
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
        elsif block_local?(var.name)
          variable = new_local var.name
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
        blk.definition_line @line
        blk.state.push_super state.super
        blk.state.push_eval state.eval

        blk.state.push_name blk.name

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

      def sexp_name
        :iter
      end

      def to_sexp
        [sexp_name, @arguments.to_sexp, @body.to_sexp]
      end
    end

    class Iter19 < Iter
      def initialize(line, arguments, body)
        @line = line
        @arguments = arguments || IterArguments.new(line, nil)
        @body = body || NilLiteral.new(line)

        if @body.kind_of?(Block) and @body.locals
          @locals = @body.locals.body.map { |x| x.value }
        else
          @locals = nil
        end
      end

      def block_local?(name)
        @locals.include?(name) if @locals
      end
    end

    class IterArguments < Node
      attr_accessor :prelude, :arity, :optional, :arguments, :splat_index, :block_index
      attr_accessor :required_args

      def initialize(line, arguments)
        @line = line
        @optional = 0
        @arguments = nil

        @splat_index = -1
        @block_index = nil
        @required_args = 0
        @splat = nil
        @block = nil
        @prelude = nil

        case arguments
        when Fixnum
          @splat_index = nil
          @arity = 0
          @prelude = nil
        when MultipleAssignment
          arguments.iter_arguments

          if arguments.splat
            case arguments.splat
            when EmptySplat
              @splat_index = -2
              arguments.splat = nil
              @prelude = :empty
            else
              @splat = arguments.splat = arguments.splat.value
            end

            @optional = 1
            if arguments.left
              @prelude = :multi
              size = arguments.left.body.size
              @arity = -(size + 1)
              @required_args = size
            else
              @prelude = :splat unless @prelude
              @arity = -1
            end
          elsif arguments.left
            size = arguments.left.body.size
            @prelude = :multi
            @arity = size
            @required_args = size

            # distinguish { |a, | ... } from { |a| ... }
            @splat_index = nil unless size == 1
          else
            @splat_index = 0
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
          @splat_index = nil
          @arguments = arguments
          @arity = 1
          @required_args = 1
          @prelude = :single
        end
      end

      alias_method :total_args, :required_args

      def post_args
        0
      end

      def names
        case @arguments
        when MultipleAssignment
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

      def arguments_bytecode(g, is_array=false)
        g.state.push_masgn

        if @arguments.kind_of? MultipleAssignment
          @arguments.bytecode(g, is_array)
        else
          @arguments.bytecode(g) if @arguments
        end

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
          arguments_bytecode(g, true)
          g.pop
        when :splat
          g.cast_for_splat_block_arg
          arguments_bytecode(g)
          g.pop
        when :empty
          g.cast_for_splat_block_arg
          g.pop
        end

        if @block
          @block.assignment_bytecode(g)
        end
      end

      def to_sexp
        if @arguments
          @arguments.to_sexp
        elsif @arity == 0
          0
        else
          nil
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

      def sexp_name
        :for
      end
    end

    class For19Arguments < Node
      attr_reader :block_index

      def initialize(line, arguments)
        @line = line
        @arguments = arguments

        if @arguments.kind_of? MultipleAssignment
          @args = 0
          @splat = 0
        else
          @args = 1
          @splat = nil
        end
      end

      def bytecode(g)
        if @splat
          g.push_literal Rubinius::Compiler::Runtime
          g.push_local 0
          g.send :unwrap_block_arg, 1
        else
          g.push_local 0
        end

        g.state.push_masgn
        @arguments.bytecode(g)
        g.state.pop_masgn
        g.pop
      end

      def required_args
        @args
      end

      def total_args
        @args
      end

      def post_args
        0
      end

      def splat_index
        @splat
      end
    end

    class For19 < For
      def initialize(line, arguments, body)
        @line = line
        @arguments = For19Arguments.new line, arguments
        @body = body || NilLiteral.new(line)

        new_local :"$for_args"
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

      def to_sexp
        [:negate, @value.to_sexp]
      end
    end

    class Super < SendWithArguments
      attr_accessor :name, :block

      def initialize(line, arguments)
        @line = line
        @block = nil
        @name = nil
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

        g.invoke_primitive :vm_check_super_callable, 0

        g.gif nope

        g.push_literal "super"
        g.string_dup
        g.goto done

        nope.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        arguments_sexp :super
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

      def to_sexp
        arguments_sexp :yield
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

      def to_sexp
        [:zsuper]
      end
    end
  end
end
