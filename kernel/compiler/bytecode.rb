# Each Node subclass is reopened here, and bytecode-generating
# methods are added.

class Compiler

  ##
  # The precursor to a CompiledMethod

  class MethodDescription
    def initialize(gen_class, locals)
      @generator = gen_class.new
      @locals = locals
      @required = 0
      @optional = 0
      @splat = nil
      @for_block = false
      @name = :__unknown__
    end

    attr_reader :generator, :locals
    attr_accessor :required, :optional, :name, :args, :splat, :for_block

    def run(container, body)
      @generator.run(body)
      @required, @optional, @splat = container.argument_info
      @name = container.name if container.name
    end

    def ==(desc)
      desc.kind_of? MethodDescription and @generator == desc.generator
    end

    def to_cmethod
      @generator.to_cmethod(self)
    end

    def to_ary
      [:method_description, @generator]
    end

    def pretty_inspect
      to_ary.pretty_inspect
    end

    def inspect
      to_ary.inspect
    end
  end

  ##
  # Parent class of all AST classes used by the compiler.  Contains utility
  # classes used by nodes.

  class Node

    def pos(g)
      g.set_line @line, @file
    end

    def show_errors(gen, &block)
      @compiler.show_errors(gen, &block)
    end

    # Called if used as the lhs of an ||=. Expected to yield if the
    # value was not found, so the bytecode for it to be emitted.
    def bytecode_for_or(g)
      found = g.new_label
      bytecode(g)
      g.dup
      g.git found
      g.pop
      yield
      found.set!
    end

    class AccessSlot
      def bytecode(g)
        g.push_my_field @index
      end
    end

    class Alias
      def bytecode(g)
        g.push_scope
        g.push_literal @current
        g.push_literal @new
        g.send :alias_method, 2, true
      end
    end

    class And
      def bytecode(g, use_gif=true)
        @left.bytecode(g)
        g.dup
        lbl = g.new_label

        if use_gif
          g.gif lbl
        else
          g.git lbl
        end

        g.pop
        @right.bytecode(g)
        lbl.set!
      end
    end

    class Arguments
      def bytecode(g)
        min = @required.size
        if @splat
          max = 1024
        else
          max = min + @optional.size
        end

        @optional.each do |var|
          assign = @mapped_defaults[var.name]
          done = g.new_label
          g.passed_arg var.stack_position
          g.git done
          assign.bytecode(g)
          g.pop

          done.set!
        end

        if @block_arg
          g.push_block
          g.dup
          g.is_nil

          after = g.new_label
          g.git after

          g.push_const :Proc
          g.swap
          g.send :__from_block__, 1

          after.set!

          @block_arg.bytecode(g)
          g.pop
        end
      end
    end

    class ArrayLiteral
      def bytecode(g)
        splat = nil
        if @body.last.kind_of?(Splat)
          splat = @body.pop
        end

        @body.each do |x|
          x.bytecode(g)
        end

        g.make_array @body.size

        splat.bytecode_in_array_literal(g) if splat
      end
    end

    class AttrAssign < Call
      # Calling a private setter method permits self as a receiver
      def allow_private?
        super || @object.kind_of?(Self)
      end

      def bytecode(g, in_masgn = false)
        if in_masgn
          @in_masgn = true
          @swap = (in_masgn == :swap)
          super(g)
        else
          @swap = false
          super(g)
          g.pop
        end
      end

      def receiver_bytecode(g)
        super(g)
        g.swap if @swap
      end

      def emit_args(g)
        @concat = false
        if @in_masgn
          if @rhs_expression
            extra = 2
            @rhs_expression.bytecode(g)
            g.swap if @swap
          else
            extra = 1
          end

          super(g)

          @argcount += extra
          return
        end

        if @rhs_expression then
          super(g)
          @rhs_expression.bytecode(g)
          @argcount += 1
        end

        if @arguments.grep(Splat).empty?
          g.dup
          # + 1 for the receiver
          g.move_down @argcount + 1
        else
          g.dup
          # + 1 for the receiver
          g.move_down @argcount + 2

          # PushArgs only for this branch
          @arguments.attr_bytecode(g)
          @dynamic = true
          @concat = true
          @argcount = 1

          g.swap
        end

      end

    end

    class BackRef
      def bytecode(g)
        g.push_variables
        g.push_literal @kind
        g.send :back_ref, 1
      end
    end

    # random ^^^
    ############################################################
    # sorted vvv

    class Begin
      def bytecode(g)
        @body.bytecode(g)
      end
    end

    class Block
      def bytecode(g)
        if @body.empty?
          g.push :nil
          return
        end

        count = @body.size - 1
        i = 0
        while i < count
          ip = g.ip
          @body[i].bytecode(g)

          # guards for things that plugins might optimize away.
          g.pop if g.advanced_since?(ip)
          i += 1
        end
        @body[count].bytecode(g)
      end
    end

    class BlockAsArgument
      def bytecode(g)
        g.set_local @variable.slot
      end
    end

    class BlockPass
      def bytecode(g)
        @block.bytecode(g)
        nil_block = g.new_label
        g.dup
        g.is_nil
        g.git nil_block
        g.find_cpath_top_const :Proc
        g.swap

        g.send :__from_block__, 1

        nil_block.set!
      end
    end

    class Break
      def do_value(g)
        if @value
          @value.bytecode(g)
        else
          g.push :nil
        end
      end

      def jump_error(g, msg)
        g.push :self
        g.push_const :LocalJumpError
        g.push_literal msg
        g.send :raise, 2, true
      end

      def bytecode(g)
        do_value(g)

        g.pop_unwind if @pop_unwind

        if g.break
          g.goto g.break
        elsif @in_block
          g.raise_break
        else
          g.pop
          g.push_const :Compiler
          g.find_const :Utils
          g.send :__unexpected_break__, 0
        end
      end
    end

    class CVar
      def bytecode(g)
        if @in_module
          g.push :self
        else
          g.push_scope
        end
        g.push_literal @name
        g.send :class_variable_get, 1
      end

      def bytecode_for_or(g)
        if @in_module
          g.push :self
        else
          g.push_scope
        end

        done =     g.new_label
        notfound = g.new_label
        g.push_literal @name
        g.send :class_variable_defined?, 1
        g.gif notfound

        # Ok, we the value exists, get it.
        bytecode(g)
        g.dup
        g.git done
        g.pop

        # yield to generate the code for when it's not found
        notfound.set!
        yield

        done.set!
      end
    end

    class CVarAssign
      def bytecode(g)
        if @in_module
          g.push :self
        else
          g.push_scope
        end

        #the value is on the stack if @value is nil
        if @value
          g.push_literal @name
          @value.bytecode(g)
        else
          g.swap # switch places with receiver
          g.push_literal @name
          g.swap # switch places with the value
        end

        g.send :class_variable_set, 2
      end
    end

    class Call
      def allow_private?
        !@object or get(:privately) or false
      end

      def receiver_bytecode(g)
        if @object then
          @object.bytecode(g)
        else
          g.push :self
        end
      end

      def emit_args(g)
        @dynamic = false
        @concat = false

        @arguments = Array(@arguments)

        @arguments.each do |argument|
          if argument.is? Splat then
            @dynamic = true
            argument.call_bytecode(g)
          else
            argument.bytecode(g)
          end
        end

        @argcount = @arguments.size - (@dynamic ? 1 : 0)
      end

      def dynamic_args?
        !@arguments.grep(Splat).empty?
      end

      def bytecode(g, try_plugin=true)
        g.set_line @line, @file

        # HACK deal with locals being inject for eval
        if @local_rewrite
          @local_rewrite.bytecode(g)
          return
        end

        return if try_plugin and use_plugin(g, :call)

        # @block might be BlockPass, and we don't generate the
        # LongReturnException detection code for that.
        if @block and @block.is? Iter
          if @in_block
            receiver_bytecode(g)
            emit_args(g)
            @block.bytecode(g)

            if @dynamic
              g.send_with_splat @method, @argcount, allow_private?, false
            else
              g.send_with_block @method, @argcount, allow_private?
            end
          else
            receiver_bytecode(g)
            emit_args(g)
            @block.bytecode(g)

            if @dynamic
              g.send_with_splat @method, @argcount, allow_private?, false
            else
              g.send_with_block @method, @argcount, allow_private?
            end
          end
        else
          receiver_bytecode(g)
          emit_args(g)

          if @block
            @block.bytecode(g)
          elsif @dynamic
            g.push :nil
          end

          if @dynamic
            g.send_with_splat @method, @argcount, allow_private?, @concat
          elsif @block
            # Only BlockPass currently
            g.send_with_block @method, @argcount, allow_private?
          else
            g.send @method, @argcount, allow_private?
          end
        end
      end
    end

    class Case
      def bytecode(g)
        fin = g.new_label

        @receiver.bytecode(g) if @receiver
        @whens.each do |w|
          nxt = g.new_label
          w.bytecode(g, @receiver ? true : false, nxt, fin)
          nxt.set!
        end

        # The condition is still on the stack
        g.pop if @receiver
        if @else
          @else.bytecode(g)
        else
          g.push :nil
        end

        fin.set!
      end
    end

    class Class
      def superclass_bytecode(g)
        if @superclass
          @superclass.bytecode(g)
        else
          g.push :nil
        end
      end

      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.push_literal @name

        if @parent
          superclass_bytecode(g)
          @parent.bytecode(g)
          g.send :open_class_under, 3
        else
          superclass_bytecode(g)
          g.push_scope
          g.send :open_class, 3
        end

        desc = attach_and_call g, :__class_init__, true
        desc.name = @name if desc
      end
    end

    class ClosedScope
      def new_description
        MethodDescription.new(@compiler.generator_class, self.locals)
      end

      def to_description(name = nil)
        desc = new_description()
        desc.name = name if name
        gen = desc.generator

        show_errors(gen) do
          desc.run(self, self)
          gen.close
        end
        return desc
      end

      def argument_info
        [0, 0, nil]
      end

      def attach_and_call(g, name, scoped=false)
        if @body.empty?
          g.pop
          g.push :nil
          return
        end

        desc = new_description()
        desc.name = name
        meth = desc.generator

        prelude(g, meth)

        if scoped
          meth.push_self
          meth.add_scope
        end

        set(:scope, self) do
          show_errors(meth) do
            desc.run self, @body
          end
        end

        meth.ret
        meth.close

        g.dup
        g.push_const :Rubinius
        g.swap
        g.push_literal name
        g.swap
        g.push_literal desc
        g.swap
        g.push_scope
        g.swap
        g.send :attach_method, 4
        g.pop
        g.send name, 0

        return desc
      end

      def prelude(orig, g)
        if orig
          g.set_line orig.line, orig.file
        end

        # Allocate some stack to store locals.

        if @alloca > 0
          g.allocate_stack @alloca
        end
      end
    end

    class ConcatArgs
      def call_bytecode(g)
        @rest.each do |x|
          x.bytecode(g)
        end

        @array.bytecode(g)
        # TODO this needs to call #to_a instead
        g.cast_array

        return @rest.size
      end

      def bytecode(g)
        @rest.each do |x|
          x.bytecode(g)
        end
        g.make_array @rest.size
        @array.bytecode(g)
        g.cast_array
        g.send :+, 1
      end
    end

    class ConstAccess
      def bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.find_const @name
      end
    end

    class ConstAtTop
      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const @name
      end

      class Toplevel
        def bytecode(g)
          g.push_cpath_top
        end
      end
    end

    class ConstFind
      def bytecode(g)
        pos(g)
        g.push_const @name
      end
    end

    class ConstSet
      def bytecode(g)
        pos(g)

        if @compiler.kernel?
          if @parent
            @parent.bytecode(g)
            @value.bytecode(g)
            g.set_const @name, true
          elsif @from_top
            g.push_cpath_top
            @value.bytecode(g)
            g.set_const @name, true
          else
            @value.bytecode(g) if @value
            g.set_const @name
          end
        else
          if @value
            # Put the value on the stack
            if @parent
              @parent.bytecode(g)
            elsif @from_top
              g.push_cpath_top
            else
              g.push_scope
            end
            g.push_literal @name
            @value.bytecode(g)
            g.send :__const_set__, 2
          else
            # if @value is nil, it means this was used inside masgn, and the value
            # is already on the stack.
            if @parent
              @parent.bytecode(g)
            elsif @from_top
              g.push_cpath_top
            else
              g.push_scope
            end
            g.swap
            g.push_literal @name
            g.swap

            g.send :__const_set__, 2
          end
        end # @compiler.kernel?
      end
    end

    class Define
      def argument_info
        opt = @arguments.optional || [] # FIX: temporary

        [@arguments.arity, opt.size, @arguments.splat]
      end

      def compile_body(g)
        desc = new_description()
        meth = desc.generator

        prelude(g, meth)

        set(:scope, nil) do
          show_errors(meth) do
            @arguments.bytecode(meth)
            desc.run self, @body # TODO: why is it not @body.bytecode(meth) ?
          end
        end

        required =  @arguments.required.map {|x| x.name} if @arguments.required
        optional =  @arguments.optional.map {|x| x.name} if @arguments.optional

        # TODO: remove this, desc.args should have @arguments
        desc.args = [required, optional, @arguments.splat && @arguments.splat.name]

        meth.ret
        meth.close

        use_plugin g, :method, desc

        return desc
      end

      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.push_literal @name
        g.push_literal compile_body(g)
        g.push_scope

        if @compiler.kernel?
          g.push :nil
        else
          g.push_variables
          g.send :method_visibility, 0
        end

        g.send :add_defn_method, 4
      end
    end

    class DefineSingleton
      def bytecode(g)
        pos(g)

        if @compiler.kernel?
          g.push_const :Rubinius
          g.push_literal @name
          g.push_literal compile_body(g)
          g.push_scope
          @object.bytecode(g)
          g.send :attach_method, 4
        else
          @object.bytecode(g)
          g.send :metaclass, 0
          g.push_literal @name
          g.push_literal compile_body(g)
          g.push_scope
          g.send :attach_method, 3
        end
      end
    end

    class Defined
      NODE_TYPES = {
        :array  => "expression",
        :call   => "method",
        :cdecl  => "assignment",
        :colon2 => "constant",
        :colon3 => "constant",
        :const  => "constant",
        :cvar   => "class variable",
        :cvasgn => "assignment",
        :cvdecl => "assignment",
        :false  => "false",
        :fixnum => "expression",
        :gasgn  => "assignment",
        :gvar   => "global-variable",
        :hash   => "expression",
        :iasgn  => "assignment",
        :ivar   => "instance-variable",
        :lasgn  => "assignment",
        :lit    => "expression",
        :lvar   => "local-variable",
        :nil    => "nil",
        :self   => "self",
        :str    => "expression",
        :true   => "true",
        :yield  => "yield",
      }

      def bytecode(g)
        pos(g)

        # Imported directly from compiler1 and reworked to use g.

        expr = @expression.dup

        # if something is defined, !something is too.
        # if !something is undefined, then so is something.
        expr.shift if expr[0] == :not

          # grouped expression == evil
          # TODO - Verify that this is still a sane way to detect such things
          if expr.flatten.include?(:newline)
            reject(g)
            return
          end

        node = expr.shift

        case node
        when :call
          receiver = expr.shift
          msg = expr.shift # method name
          args = expr.shift

          # Make sure there are no args.
          if args.size > 1 then
            reject(g)
            return
          end

          # defined?(DoesNotExist.puts) should not raise NameError
          if receiver && receiver.is?(ConstFind) then
            no_const = g.new_label
            done = g.new_label
            g.push_scope
            g.push_literal receiver.name
            g.send :const_defined?, 1
            g.gif no_const

            if receiver then
              receiver.bytecode(g)
            else
              g.push :self
            end

            g.push_literal msg
            g.push :true
            g.send :__respond_to_eh__, 2
            g.goto done

            no_const.set!
            g.push :nil
            done.set!
          else
            if receiver then
              receiver.bytecode(g)
            else
              g.push :self
            end
            g.push_literal msg
            g.push :true
            g.send :__respond_to_eh__, 2
          end
        when :cvar
          cvar = expr.shift
          g.push :self
          g.push :true # class vars as symbols, not strings
          g.send :class_variables, 1
          g.push_literal cvar
          g.send :include?, 1
        when :gvar
          g.push_const :Rubinius
          g.find_const :Globals
          g.push_literal expr.shift
          g.send :key?, 1
        when :ivar
          ivar = expr.shift
          g.push :self
          g.push_literal ivar
          g.send :__instance_variable_defined_eh__, 1
        when :yield
          g.push_block
        when :const
          g.push_scope
          g.push_literal expr.shift
          g.send :const_defined?, 1
        when :colon2
          str = ""
          until expr.empty?
            # Convert the constant parse tree into a string like ::Object::SomeClass
            str = const_to_string(expr, str)
          end
          g.push_scope
          g.push_literal str
          g.send :const_path_defined?, 1
        when :colon3
          str = ""
          until expr.empty?
            # Convert the constant parse tree into a string like ::Object::SomeClass
            str = const_to_string(expr, str)
          end
          g.push_const :Object
          g.push_literal str
          g.send :const_path_defined?, 1
        else
          if NODE_TYPES.key?(node)
            g.push :true
          else
            reject(g)
            return
          end
        end
        push_return_value(g, node)
      end

      # Return the correct string based on the node type
      def push_return_value(g, node)
        defined = g.new_label
        done = g.new_label
        g.git defined
        g.push :nil # defined? returns nil instead of false
        g.goto done
        defined.set!
        lit = NODE_TYPES[node].dup
        g.push_literal lit
        done.set!
      end

      # e.g. [[:const, :Object], :Blah]
      # e.g. [[:colon3, :Foo], :Bar]
      # e.g. [[:colon2, [:colon3, :Faz], :Boo], :Batch]
      # e.g. [[:colon2, [:const, :Zizz], :Koom], :Yonk]
      # TODO - There is probably a better way, but it is late. Really late.
      def const_to_string(tree, str)
        return str if tree.empty?
        piece = tree.shift
        unless str[-2,2] == "::" || str == ""
          str << "::"
        end
        if piece.is_a?(Array)
          str = const_to_string(piece, str) until piece.empty?
          str
        elsif [:const, :colon2].include?(piece)
          str
        elsif piece == :colon3
          "::" + str
        else
          str << "#{piece}"
        end
      end

      def reject(g)
        if $DEBUG
          STDERR << "Passed a complex expression to 'defined?'"
        end

        g.push :false
      end
    end

    class DynamicExecuteString
      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true #`
      end
    end

    class DynamicOnceRegex
      def bytecode(g)
        pos(g)

        idx = g.add_literal(nil)
        g.push_literal_at idx
        g.dup
        g.is_nil

        lbl = g.new_label
        g.gif lbl
        g.pop

        super(g)

        g.set_literal idx
        lbl.set!
      end
    end

    class DynamicRegex
      def bytecode(g)
        pos(g)

        g.push_const :Regexp
        super(g)
        g.push @options
        g.send :new, 2
      end
    end

    class DynamicString
      def bytecode(g)
        pos(g)

        @body.reverse_each do |x|
          x.bytecode(g)
        end
        g.push_literal @string
        g.string_dup

        @body.size.times do
          g.string_append
        end
      end
    end

    class DynamicSymbol
      def bytecode(g)
        pos(g)

        super
        g.send :to_sym, 0, true
      end
    end

    class EmptyArray
      def bytecode(g)
        pos(g)

        g.make_array 0
      end
    end

    class Ensure
      def bytecode(g)
        pos(g)

        if @body.nil?
          # So dumb. Oh well.

          if @ensure
            # Well, run it.
            @ensure.bytecode(g)
            g.pop
          end

          g.push :nil

          return
        end

        # No ensure? dumb.
        if @ensure.nil?
          @body.bytecode(g)
          return
        end

        ok = g.new_label
        g.exceptions :ensure do |ex|
          @body.bytecode(g)
          ex.escape ok

          ex.handle!
          g.push_exception

          @ensure.bytecode(g)
          g.pop

          g.pop_exception

          # Re-raise the exception
          g.reraise
        end

        ok.set!

        # Now, re-emit the code for the ensure which will run if there was no
        # exception generated.
        @ensure.bytecode(g)
        g.pop
      end
    end

    # REFACTOR See if there is a sane way to call 'super' here
    class EvalExpression
      def bytecode(g)
        set(:scope, self) do
          prelude(nil, g)
          @body.bytecode(g)
          g.ret
        end
        enlarge_context
      end
    end

    class ExecuteString
      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true # ` (silly vim/emacs)
      end
    end

    class Expression
      def bytecode(g)
        set(:scope, self) do
          prelude(nil, g)
          @body.bytecode(g)
          g.ret
        end
      end
    end

    class False
      def bytecode(g)
        g.push :false
      end
    end

    class File
      def bytecode(g)
        pos(g)

        g.push_scope
        g.send :active_path, 0
      end
    end

    class FloatLiteral
      def bytecode(g)
        g.push_literal @value
      end
    end

    class GVar
      def bytecode(g)
        pos(g)

        if @name == :$!
          g.push_exception
        elsif @name == :$~
          g.push_variables
          g.send :last_match, 0
        else
          g.push_const :Rubinius
          g.find_const :Globals
          g.push_literal @name
          g.send :[], 1
        end
      end
    end

    class GVarAssign
      def bytecode(g)
        pos(g)

        # @value can to be present if this is coming via an masgn, which means
        # the value is already on the stack.
        if @name == :$!
          @value.bytecode(g) if @value
          g.raise_exc
        elsif @name == :$~
          if @value
            g.find_cpath_top_const :Regexp
            @value.bytecode(g)
            g.send :last_match=, 1
          else
            g.find_cpath_top_const :Regexp
            g.swap
            g.send :last_match=, 1
          end
        else
          if @value
            g.push_const :Rubinius
            g.find_const :Globals
            g.push_literal @name
            @value.bytecode(g)
            g.send :[]=, 2
          else
            g.push_const :Rubinius
            g.find_const :Globals
            g.swap
            g.push_literal @name
            g.swap
            g.send :[]=, 2
          end
        end
      end
    end

    class HashLiteral
      def bytecode(g)
        pos(g)

        count = @body.size
        i = 0

        g.find_cpath_top_const :Hash

        while i < count
          v = @body[i]
          k = @body[i + 1]

          v.bytecode(g)
          k.bytecode(g)
          i += 2
        end

        g.send :[], count
      end
    end

    class IVar
      def bytecode(g)
        pos(g)

        g.push_ivar @name
      end
    end

    class IVarAssign
      def bytecode(g)
        pos(g)

        @value.bytecode(g) if @value
        g.set_ivar @name
      end
    end

    class If
      def bytecode(g)
        pos(g)

        ed = g.new_label
        el = g.new_label

        @condition.bytecode(g)

        if @then and @else
          g.gif el
          @then.bytecode(g)
          g.goto ed
          el.set!
          @else.bytecode(g)
        elsif @then
          g.gif el
          @then.bytecode(g)
          g.goto ed
          el.set!
          g.push :nil
        elsif @else
          g.git el
          @else.bytecode(g)
          g.goto ed
          el.set!
          g.push :nil
        else
          # An if with no code. Sweet.
          g.pop
          g.push :nil
          return
        end

        ed.set!
      end
    end

    class Iter
      def argument_info
        [@arguments.arity, @arguments.optional.size, nil]
      end

      def bytecode(g)
        pos(g)

        desc = MethodDescription.new @compiler.generator_class, @locals
        desc.name = :__block__
        desc.for_block = true
        desc.required, desc.optional, _ = argument_info
        sub = desc.generator

        # Push line info down.
        sub.set_line g.line, g.file

        show_errors(sub) do
          if @arguments
            @arguments.bytecode(sub)
          else
            # Remove the block args.
            sub.pop
          end
          sub.push_modifiers
          sub.break = nil
          sub.next = nil
          sub.redo = sub.new_label
          sub.redo.set!
          @body.bytecode(sub)
          sub.pop_modifiers
          sub.ret
          sub.close
        end

        g.create_block desc
      end
    end

    class IterArgs
      def bytecode(g)
        case @child
        when MAsgn
          if @child.splat_only?
            g.cast_for_splat_block_arg
          else
            g.cast_for_multi_block_arg
          end
          @child.in_block = true
          @child.bytecode(g)
        when LocalAssignment, IVarAssign, GVarAssign
          g.cast_for_single_block_arg
          @child.bytecode(g)
          g.pop
        when AttrAssign
          g.cast_for_single_block_arg
          @child.bytecode(g, :swap)
          g.pop
        when Fixnum, nil
          # nothing
        else
          raise Error, "Unknown form of block args: #{@child.class}"
        end
      end
    end

    class Literal
      def bytecode(g)
        pos(g)

        g.push_unique_literal @value
      end
    end

    class LocalAccess
      def bytecode(g)
        pos(g)

        # No variable? Ok, send a method instead.
        if !@variable
          g.push_self
          g.send @name, 0
        elsif @variable.kind_of? EvalExpression::DynamicLocal
          g.push_variables
          g.push_literal @variable.name
          g.send :get_eval_local, 1, false
        elsif @variable.on_stack?
          if @variable.argument?
            g.from_fp @variable.stack_position
          else
            g.get_local_fp @variable.stack_position
          end
        elsif !@depth or @depth == 0
          g.push_local @variable.slot
        else
          g.push_local_depth @depth, @variable.slot
        end
      end
    end

    class LocalAssignment
      def bytecode(g)
        pos(g)

        if @value
          @value.bytecode(g)
        end

        if @splat then
          @splat.bytecode(g)
          g.cast_array
          g.send :+, 1
        end

        # No @value means assume that someone else put the value on the
        # stack (ie, an masgn)

        if @variable.kind_of? EvalExpression::DynamicLocal
          g.push_variables
          g.swap
          g.push_literal @variable.name
          g.swap
          g.send :set_eval_local, 2, false
        elsif !@depth or @depth == 0
          g.set_local @variable.slot
        else
          g.set_local_depth @depth, @variable.slot
        end
      end
    end

    class Loop
      def bytecode(g)
        g.push_modifiers

        g.break = g.new_label
        g.next = g.redo = top = g.new_label
        top.set!

        if @body then
          @body.bytecode(g)
          g.pop
        end

        g.check_interrupts
        g.goto top

        g.break.set!
        g.pop_modifiers
      end
    end

    class MAsgn
      # MAsgn rules:
      # FIX: this is all completely wrong
      # - MAsgn sexps always contain either 2 or 3 values
      # - If the masgn contains only 2 values:
      #   - it means there are no lasgns, i.e. the lhs consists solely of a splat
      #     arg.
      #   - the first arg is the lhs splat arg, and cannot be nil; however, it
      #     may be just the value true, in the case of a block anonymous splat arg,
      #     i.e. |*|
      #   - the second arg is either one of [:array, :splat, :argscat, :to_ary], or
      #     nil if the masgn represents block arguments.
      # - If the masgn contains 3 values:
      #   - the first arg is an array of lasgns with a minimum of one lasgn.
      #   - the second arg is the lhs splat arg, or nil if there are no splats
      #   - the third arg is either one of [:array, :splat, :argscat, :to_ary], or
      #     nil if the masgn represents block arguments.
      # - An MAsgn may contain nested masgns, e.g. a,(b,c) = 1,[2,3]. The nested
      #   masgn(s) will have nil for their @lhs, since the assigns come from
      #   the outermost masgn.
      def bytecode(g)
        # FIX: this is completely insane... most brittle code EVER
        # FIX: violation of encapsulation is rife in this file
        if @rhs
          if @rhs.is? ArrayLiteral
            if @splat_lhs && @splat_rhs then
              statement_bytecode(g)
            elsif @splat_lhs then
              array_bytecode(g)
            elsif @splat_rhs then
              statement_bytecode(g)
            else
              flip_assign_bytecode(g)
            end
          else
            statement_bytecode(g)
          end
        elsif @in_block
          block_arg_bytecode(g)
        else
          statement_bytecode(g)
        end
      end

      # Pad stack if there are insufficient source values for the assigns
      def pad_stack(g)
        diff = -@rhs.body.size
        if @lhs
          diff += @lhs.body.size
          if diff > 0
            diff.times { g.push :nil }
          end
        end
        return diff
      end

      # The easiest case for a normal masgn.
      # a,b = c,d
      # a = c,d
      # a,b,c = d,e
      def flip_assign_bytecode(g)
        # Pad the stack with extra nils if there are more assigns
        # than sources
        diff = pad_stack(g)

        @rhs.body.each do |x|
          x.bytecode(g)
        end

        if @splat_rhs then
          g.make_array @rhs.body.size
          @splat_rhs.bytecode(g)
          g.cast_array
          g.send :+, 1
        else
          g.rotate @rhs.body.size if @rhs.respond_to? :body # HACK
        end

        # Now all the source data is on the stack.

        @lhs.body.each do |x|
          if x.is? AttrAssign
            x.bytecode(g, :swap)
          else
            x.bytecode(g)
          end
          g.pop
        end if @lhs

        # Clean up the stack if there was extra sources
        if diff < 0
          # unary - has binds strangly
          (-diff).times { g.pop }
        end

        g.push :true
      end

      def array_bytecode(g)
        @rhs.body.each do |x|
          x.bytecode(g)
        end

        if @splat_rhs then
          g.make_array @rhs.body.size
          @splat_rhs.bytecode(g)
          g.cast_array
          g.send :+, 1
        else
          diff = pad_stack(g)
          if diff >= 0
            sz = 0
          else
            sz = -diff
          end

          g.make_array sz
        end

        @splat_lhs.bytecode(g) unless @splat_lhs.kind_of? TrueClass  # e.g. * = 1,2
        g.pop
        if @lhs
          @lhs.body.reverse_each do |x|
            if x.is? AttrAssign
              x.bytecode(g, true)
            else
              x.bytecode(g)
            end
            g.pop
          end
        end

        g.push :true
      end

      def statement_bytecode(g)
        if @rhs.nil?
          # skip
        elsif @rhs.is? Splat or @rhs.is? ToArray
          @rhs.child.bytecode(g)
        else
          @rhs.body.each do |x|
            x.bytecode(g)
          end
        end

        if @splat_rhs then
          g.make_array @rhs.body.size
          @splat_rhs.bytecode(g)
          g.cast_array
          g.send :+, 1
        else
          g.rotate @rhs.body.size if @rhs.respond_to? :body # HACK
        end

        if @lhs && !@lhs.body.empty?
          g.cast_array
          @lhs.body.each do |x|
            g.shift_array
            if x.is? AttrAssign
              x.bytecode(g, :swap)
            else
              x.bytecode(g)
            end
            g.pop
          end
        end

        if @splat_lhs
          @splat_lhs.bytecode_for_lhs(g, @lhs, @rhs)
        end

        g.pop
        g.push :true
      end

      def block_arg_bytecode(g)
        if @lhs
          g.cast_array
          @lhs.body.each do |x|
            g.shift_array
            x.bytecode(g)
            g.pop
          end
        end

        # TODO: = 0 (empty goalposts) should be handled here

        if @splat_lhs and @splat_lhs.child
          g.cast_array
          @splat_lhs.bytecode(g)
        end

        g.pop
      end

      def splat_only?
        @lhs.body.empty? and @splat_lhs and @splat_lhs.child
      end
    end

    class Match
      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.find_const :Globals
        g.push_literal :$_
        g.send :[], 1

        @pattern.bytecode(g)

        g.send :=~, 1
      end
    end

    class Match2
      def bytecode(g)
        pos(g)

        @pattern.bytecode(g)
        @target.bytecode(g)
        g.send :=~, 1
      end
    end

    class Match3
      def bytecode(g)
        pos(g)

        @target.bytecode(g)
        @pattern.bytecode(g)
        g.send :=~, 1
      end
    end

    class Module
      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.push_literal @name

        if @parent
          @parent.bytecode(g)
          g.send :open_module_under, 2
        else
          g.push_scope
          g.send :open_module, 2
        end

        attach_and_call g, :__module_init__, true
      end
    end

    class Negate
      def bytecode(g)
        pos(g)

        if @child.is? NumberLiteral
          g.push(-@child.value)
        else
          @child.bytecode(g)
          g.send :"-@", 0
        end
      end
    end

    class Newline
      def bytecode(g)
        g.set_line @line, @file
        @child.bytecode(g) if @child
      end

      def call_bytecode(g)
        g.set_line @line, @file
        @child.call_bytecode(g)
      end
    end

    class Next
      def bytecode(g)
        pos(g)

        g.pop_unwind if @pop_unwind

        if g.next
          g.goto g.next
        elsif @in_block
          if @value
            @value.bytecode(g)
          else
            g.push :nil
          end
          g.ret
        else
          @value.bytecode(g) if @value # next(raise("foo")) ha ha ha
          jump_error g, "next used in invalid context"
        end
      end
    end

    class Nil
      def bytecode(g)
        g.push :nil
      end
    end

    class Not
      def bytecode(g)
        @child.bytecode(g)
        tr = g.new_label
        ed = g.new_label
        g.git tr
        g.push :true
        g.goto ed
        tr.set!
        g.push :false
        ed.set!
      end
    end

    class NthRef
      def bytecode(g)
        pos(g)

        g.push_variables
        g.push @which
        g.send :nth_ref, 1
      end
    end

    class NumberLiteral
      def bytecode(g)
        g.push @value
      end
    end

    class OpAssign1
      def bytecode(g)
        pos(g)

        # X: Snippet used for explanation: h[:a] += 3
        # X: given h = { :a => 2 }
        # X: Pull h onto the stack
        @object.bytecode(g)
        g.dup
        # X: Pull :a in
        @index.each do |idx|
          idx.bytecode(g)
        end
        #
        # X: Call [](:a) on h
        #
        # @index.size will be 1

        g.send :[], @index.size

        # X: 2 is now on the top of the stack (TOS)

        # A special case, where we use the value as boolean
        if @kind == :or or @kind == :and
          fnd = g.new_label
          fin = g.new_label

          # We dup the value from [] to leave it as the value of the
          # expression

          g.dup
          if @kind == :or
            g.git fnd
          else
            g.gif fnd
          end

          # Ok, take the extra copy off and pull the value onto the stack
          g.pop

          # Pull the index onto the stack again, swaping to push
          # @value to the top of the stack (last argument)
          @index.each do |idx|
            idx.bytecode(g)
          end

          @value.bytecode(g)

          # retain the rhs as the expression value
          g.dup
          g.move_down @index.size + 2

          g.send :[]=, @index.size + 1
          g.pop
          g.goto fin

          fnd.set!

          # Clean up the stack by swaping, allowing us to pop of the extra
          # copy of @object we left on the stack
          g.swap
          g.pop

          fin.set!
        else
          # @kind is something like + or -
          # We pull in @value to the stack
          @value.bytecode(g)
          # X: 3 TOS

          # ... then call it as an argument to @kind, called on the return
          # from [].
          # X: 2 + 3

          g.send @kind, 1
          # X: 5 TOS

          # The new value is on the stack now. It needs to be the last argument
          # when we call []=, so we call the index again, swapping each time to
          # move the new value up to the top of the stack (where the last
          # argument lives)
          @index.each do |idx|
            idx.bytecode(g)
            g.swap
          end

          # retain the rhs as the expression value
          g.dup
          g.move_down @index.size + 2

          # X: Call []=(:a, 5) on h
          g.send :[]=, @index.size + 1
          g.pop
        end
      end
    end

    class OpAssign2
      def bytecode(g)
        pos(g)

        # X: h[:a] += 3, given h.a == 2
        @object.bytecode(g)
        # X: TOS = h
        g.dup
        g.send @method, 0
        # X: TOS = 2

        if @kind == :or or @kind == :and
          fnd = g.new_label
          fin = g.new_label

          g.dup
          if @kind == :or
            g.git fnd
          else
            g.gif fnd
          end

          # Remove the copy of 2 and push @value on the stack
          g.pop
          @value.bytecode(g)

          # Retain the this value to use as the expression value
          g.dup
          g.move_down 2

          # Call the assignement method, passing @value as the argument
          g.send @assign, 1
          g.pop

          g.goto fin

          fnd.set!

          # Clean up the stack
          g.swap
          g.pop

          fin.set!
        else
          @value.bytecode(g)
          # X: TOS = 3
          # X: 2 + 3
          g.send @kind, 1

          # Retain the this value to use as the expression value
          g.dup
          g.move_down 2
          # X: TOS = 5
          g.send @assign, 1
          # X: TOS = 5 (or whatever a=() returns)

          # Discard the methods return value
          g.pop
        end
      end
    end

    class OpAssignAnd
      def bytecode(g)
        pos(g)

        @left.bytecode(g)
        lbl = g.new_label
        g.dup
        g.gif lbl
        g.pop
        @right.bytecode(g)
        lbl.set!
      end
    end

    class OpAssignOr
      def bytecode(g)
        pos(g)

        @left.bytecode_for_or(g) do
          @right.bytecode(g)
        end
      end
    end

    class Or
      def bytecode(g)
        super(g, false)
      end
    end

    class PostExe < Call
      def allow_private?
        true
      end

      def receiver_bytecode(g)
        g.push :self
      end
    end

    class PushArgs
      def bytecode(g)
        raise Error, "Shouldn't be used"
      end

      # Used for h[*x] = 3
      def attr_bytecode(g)
        @array.bytecode(g)
        g.cast_array
        @item.bytecode(g)
        g.swap
      end
    end

    class Range
      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.send :new, 2
      end
    end

    class RangeExclude
      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.push :true

        g.send :new, 3
      end
    end

    class Redo
      def bytecode(g)
        g.pop_unwind if @pop_unwind

        if g.redo
          g.goto g.redo
        else
          jump_error g, "redo used in invalid context"
        end
      end
    end

    class RegexLiteral
      def bytecode(g)
        pos(g)

        # A regex literal should only be converted to a Regexp the first time it
        # is encountered. We push a literal nil here, and then overwrite the
        # literal value with the created Regexp if it is nil, i.e. the first time
        # only. Subsequent encounters will use the previously created Regexp
        idx = g.add_literal(nil)
        g.push_literal_at idx
        g.dup
        g.is_nil

        lbl = g.new_label
        g.gif lbl
        g.pop
        g.push_const :Regexp
        g.push_literal @source
        g.push @options
        g.send :new, 2
        g.set_literal idx
        lbl.set!
      end
    end

    class Rescue
      def bytecode(g)
        pos(g)

        g.push_modifiers
        if @body.nil?
          if @else.nil?
            # Stupid. No body and no else.
            g.push :nil
          else
            # Only an else, run it.
            @else.bytecode(g)
          end
        else
          g.retry = g.new_label
          reraise = g.new_label
          els     = g.new_label
          last    = g.new_label

          # Save the current exception into a local
          g.push_exception

          g.retry.set!
          g.exceptions do |ex|
            @body.bytecode(g)
            ex.escape els

            ex.handle!
            max = @rescues.size - 1
            @rescues.each_with_index do |resbody, i|
              resbody.bytecode(g, reraise, last, i == max)
            end
            reraise.set!
            g.reraise
          end

          els.set!
          if @else
            g.pop
            @else.bytecode(g)
          end
          last.set!

          # Restore the previous exception if execution reaches this point
          #g.push_local @saved_exception.slot
          g.swap
          g.pop_exception
        end
        g.pop_modifiers
      end
    end

    class RescueCondition
      def bytecode(g, reraise, if_done, last)
        body = g.new_label
        assignment = nil

        if @conditions
          if !@conditions.last.is?(ConstFind) and
             !@conditions.last.is?(ConstAccess) then
            assignment = @conditions.pop
          end

          @conditions.each do |x|
            x.bytecode(g)
            g.push_exception
            g.send :===, 1
            g.git body
          end
        end

        @splat.bytecode_in_rescue_clause(g, body) if @splat

        if last then
          g.goto reraise
        else
          if_false = g.new_label
          g.goto if_false
        end

        body.set!
        if assignment
          assignment.bytecode(g)
          g.pop
        end

        current_break = g.break
        g.break = g.new_label

        @body.bytecode(g)
        g.clear_exception
        g.goto if_done

        if g.break.used?
          g.break.set!
          g.clear_exception

          # duplicated code from rescue
          g.swap
          g.pop_exception
          g.goto current_break
        end

        g.break = current_break

        if_false.set! unless last
      end
    end

    class Retry
      def bytecode(g)
        pos(g)

        if g.retry
          g.goto g.retry
        else
          jump_error g, "retry used in invalid context"
        end
      end
    end

    class Return
      def bytecode(g, force=false)
        pos(g)

        if @in_rescue
          g.clear_exception
        end

        # Literal ArrayList and a splat
        if @splat
          splat_node = @value.body.pop
          @value.bytecode(g)
          splat_node.call_bytecode(g)
          g.send :+, 1
        elsif @value
          @value.bytecode(g)
        else
          g.push :nil
        end

        if @in_block
          g.raise_return
        elsif !force and @in_ensure
          g.ensure_return
        else
          g.ret
        end
      end

    end

    class SClass
      def bytecode(g)
        pos(g)

        @object.bytecode(g)
        g.dup
        g.send :__verify_metaclass__, 0
        g.pop
        g.push_const :Rubinius
        g.swap
        g.send :open_metaclass, 1

        attach_and_call g, :__metaclass_init__, true
      end
    end

    class SValue
      def bytecode(g)
        pos(g)

        if @literal and @splat
          # Both a literal list of args and a splat
          # Create two arrays and combine them.
          @literal.bytecode(g)
          @splat.call_bytecode(g)
          g.send :+, 1
        elsif @literal
          # Only a literal list of args, no splat
          @literal.bytecode(g)
        elsif @splat
          # Just a splat, no literal args
          @splat.bytecode(g)
          unwrap_array(g)
        end
      end

      # If the array has 1 or 0 elements, grab the 0th element.
      # otherwise, leave the array on the stack.
      def unwrap_array(g)
        g.cast_array
        g.dup
        g.send :size, 0
        g.push 1
        g.send :>, 1

        lbl = g.new_label
        g.git lbl

        g.push 0
        g.send :at, 1

        lbl.set!
      end

    end

    class Scope
      def bytecode(g)
        if @block.nil?
          g.push :nil
          return
        end

        @block.bytecode(g)
      end
    end

    class Script
      def bytecode(g)
        pos(g)

        set(:scope, self) do
          prelude(nil, g)
          @body.bytecode(g)
          g.pop
          g.push :true
          g.ret
        end
      end
    end

    class Self
      def bytecode(g)
        g.push :self
      end
    end

    class SetSlot
      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.store_my_field @index
      end
    end

    class Snippit
      def bytecode(g)
        set(:scope, self) do
          prelude(nil, g)
          @body.bytecode(g)
        end
      end
    end

    class Splat
      def bytecode(g)
        @child.bytecode(g) if @child
      end

      # Bytecode generation when a splat is used as a method arg
      def call_bytecode(g)
        if @child
          @child.bytecode(g)
          g.cast_array
        end

        return 0
      end

      # Bytecode emitted when a splat is present in an ArrayLiteral
      # expects the top of the stack to be an array
      def bytecode_in_array_literal(g)
        if @child
          @child.bytecode(g)
          g.cast_array
          g.send :+, 1
        end
      end

      # *x = r() should wrap r()'s return value in an Array
      # but a,b,*c = r() has no need to do so
      def bytecode_for_lhs(g, lhs, rhs)
        return unless @child

        should_wrap = lhs.body.size.zero? ? true : false
        should_wrap = false if rhs.nil? || rhs.is?(Splat)
        if should_wrap
          wrap_array(g)
        elsif rhs.nil?
          cast_array(g)
        else
          g.cast_array
        end

        @child.bytecode(g)
      end

      # rescue *blah => e has an lvar assignment as the second component
      def bytecode_in_rescue_clause(g, body_label)
        @child.bytecode(g)
        g.cast_array
        if @rescue_clause
          @rescue_clause.bytecode(g)
        else
          g.push_exception
        end
        g.send :__rescue_match__, 1
        g.git body_label
      end

      # If the top of the stack is already an Array, wrap it in another.
      # If it is anything else, use cast_array
      def wrap_array(g)
        g.dup
        g.find_cpath_top_const :Array
        g.swap
        g.kind_of

        wrap = g.new_label
        done = g.new_label
        g.git wrap
        g.cast_array
        g.goto done

        wrap.set!
        g.make_array 1
        done.set!
      end

      # If the top of the stack is already an Array, return it.
      # If it is anything else, use make_array
      def cast_array(g)
        g.dup
        g.find_cpath_top_const :Array
        g.swap
        g.kind_of

        done = g.new_label
        g.git done
        g.make_array 1
        done.set!
      end
    end

    class StringLiteral
      def bytecode(g)
        pos(g)

        g.push_literal @string
        g.string_dup
      end
    end

    class Super < Call
      def bytecode(g)
        pos(g)

        emit_args(g)

        if @block
          @block.bytecode(g)
        else
          g.push_block
        end

        if @dynamic
          g.send_super @method.name, @argcount, true
        else
          g.send_super @method.name, @argcount
        end
      end
    end

    class ToArray
      def bytecode(g)
        pos(g)

        @child.bytecode(g)
        g.cast_array
      end
    end

    class ToString
      def bytecode(g)
        pos(g)

        @child.bytecode(g)
        g.send :to_s, 0, true
      end
    end

    class True
      def bytecode(g)
        g.push :true
      end
    end

    class Undef
      def bytecode(g)
        pos(g)

        g.push_scope
        g.push_literal @name
        g.send :__undef_method__, 1
      end
    end

    class Until
      def bytecode(g)
        super(g, false)
      end
    end

    class VAlias
      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.find_const :Globals
        g.push_literal @new
        g.push_literal @current
        g.send :add_alias, 2
      end
    end

    class When
      # The bytecode to test a when condition is different, depending on whether
      # or not the case statement has a receiver, i.e.
      #   case foo; when ...
      # vs
      #   case; when ...
      def condition_bytecode(g, has_receiver, cond)
        if has_receiver
          # Case
          g.dup
          cond.bytecode(g)
          g.swap
          g.send :===, 1
        else
          # ManyIf - no receiver
          cond.bytecode(g)
        end
      end

      def bytecode(g, has_receiver, nxt, fin)
        if @conditions.size == 1 and !@splat
          condition_bytecode(g, has_receiver, @conditions.first)
          g.gif nxt
        else
          body = g.new_label

          @conditions.each do |c|
            condition_bytecode(g, has_receiver, c)
            g.git body
          end

          if @splat
            if @splat.kind_of? Array
              @splat.each do |c|
                condition_bytecode(g, has_receiver, c)
                g.git body
              end
            elsif has_receiver
              g.dup
              @splat.bytecode(g)
              g.cast_array
              g.swap
              g.send :__matches_when__, 1
              g.git body
            else
              raise Error, "encountered splat on many-if!"
            end
          end

          g.goto nxt

          body.set!
        end

        # Remove the thing we've been testing.
        g.pop if has_receiver
        @body.bytecode(g)
        g.goto fin
      end
    end

    class While
      def bytecode(g, use_gif=true)
        pos(g)

        g.push_modifiers

        top = g.new_label
        bot = g.new_label
        g.break = g.new_label

        if @check_first
          g.redo = g.new_label
          g.next = top

          top.set!

          @condition.bytecode(g)
          if use_gif
            g.gif bot
          else
            g.git bot
          end

          g.redo.set!

          @body.bytecode(g)
          g.pop
        else
          g.next = g.new_label
          g.redo = top

          top.set!

          @body.bytecode(g)
          g.pop

          g.next.set!
          @condition.bytecode(g)
          if use_gif
            g.gif bot
          else
            g.git bot
          end
        end

        g.check_interrupts
        g.goto top

        bot.set!
        g.push :nil
        g.break.set!

        g.pop_modifiers
      end
    end

    class Yield < Call
      def bytecode(g)
        pos(g)

        emit_args(g)
        if @dynamic
          g.yield_splat @argcount
        else
          g.yield_stack @argcount
        end
      end
    end

    class ZSuper
      def bytecode(g)
        pos(g)
        args = []

        @method.arguments.required.each do |var|
          la = LocalAccess.new @compiler
          la.from_variable var
          la.set_position self
          args << la
        end

        @method.arguments.optional.each do |var|
          la = LocalAccess.new @compiler
          la.from_variable var
          la.set_position self
          args << la
        end

        if @method.arguments.splat
          cc = Splat.new @compiler
          cc.set_position self

          la = LocalAccess.new @compiler
          la.set_position self
          la.from_variable @method.arguments.splat

          cc.args la
          args << cc
        end

        @arguments = args

        super(g)
      end
    end
  end
end

