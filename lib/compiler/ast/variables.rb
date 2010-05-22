module Rubinius
  module AST
    class BackRef < Node
      attr_accessor :kind

      def initialize(line, ref)
        @line = line
        @kind = ref
      end

      Kinds = {
        :& => 1,
        :"`" => 2,
        :"'" => 3,
        :+ => 4
      }

      def mode
        unless mode = Kinds[@kind]
          raise "Unknown backref: #{@kind}"
        end

        mode
      end

      def bytecode(g)
        pos(g)

        g.last_match mode, 0
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        g.last_match mode, 0
        g.is_nil
        g.git f

        g.push_literal "$#{@kind}"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        [:back_ref, @kind]
      end
    end

    class NthRef < Node
      attr_accessor :which

      def initialize(line, ref)
        @line = line
        @which = ref
      end

      Mode = 5

      def bytecode(g)
        pos(g)

        # These are for $1, $2, etc. We subtract 1 because
        # we start numbering the captures from 0.
        g.last_match Mode, @which - 1
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        g.last_match Mode, @which - 1
        g.is_nil
        g.git f

        g.push_literal "$#{@which}"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        [:nth_ref, @which]
      end
    end

    class VariableAccess < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def value_defined(g, f)
        variable_defined(g, f)
        bytecode(g)
      end
    end

    class VariableAssignment < Node
      attr_accessor :name, :value

      def initialize(line, name, value)
        @line = line
        @name = name
        @value = value
      end

      def defined(g)
        g.push_literal "assignment"
      end

      def to_sexp
        sexp = [sexp_name, @name]
        sexp << @value.to_sexp if @value
        sexp
      end
    end

    class ClassVariableAccess < VariableAccess
      def or_bytecode(g)
        pos(g)

        done =     g.new_label
        notfound = g.new_label

        variable_defined(g, notfound)

        # Ok, we know the value exists, get it.
        bytecode(g)
        g.dup
        g.git done
        g.pop

        # yield to generate the code for when it's not found
        notfound.set!
        yield

        done.set!
      end

      def bytecode(g)
        pos(g)

        push_scope(g)
        g.send :class_variable_get, 1
      end

      def push_scope(g)
        if g.state.scope.module?
          g.push :self
        else
          g.push_scope
        end
        g.push_literal @name
      end

      def variable_defined(g, f)
        push_scope(g)
        g.send :class_variable_defined?, 1
        g.gif f
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        variable_defined(g, f)
        g.push_literal "class variable"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        [:cvar, @name]
      end
    end

    class ClassVariableAssignment < VariableAssignment
      def bytecode(g)
        pos(g)

        if g.state.scope.module?
          g.push :self
        else
          g.push_scope
        end

        if @value
          g.push_literal @name
          @value.bytecode(g)
        else
          g.swap
          g.push_literal @name
          g.swap
        end

        g.send :class_variable_set, 2
      end

      def sexp_name
        :cvasgn
      end
    end

    class ClassVariableDeclaration < ClassVariableAssignment
      def sexp_name
        :cvdecl
      end
    end

    class GlobalVariableAccess < VariableAccess
      def bytecode(g)
        pos(g)

        if @name == :$!
          g.push_current_exception
        elsif @name == :$~
          g.last_match 0, 0
        else
          g.push_rubinius
          g.find_const :Globals
          g.push_literal @name
          g.send :[], 1
        end
      end

      def variable_defined(g, f)
        unless @name == :$! or @name == :$~
          g.push_rubinius
          g.find_const :Globals
          g.push_literal @name
          g.send :key?, 1
          g.gif f
        end
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        variable_defined(g, f)
        g.push_literal "global-variable"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        [:gvar, @name]
      end
    end

    class GlobalVariableAssignment < VariableAssignment
      def bytecode(g)
        pos(g)

        # @value can be nil if this is coming via an masgn, which means
        # the value is already on the stack.
        if @name == :$!
          @value.bytecode(g) if @value
          g.raise_exc
        elsif @name == :$~
          g.push_cpath_top
          g.find_const :Regexp
          @value ?  @value.bytecode(g) : g.swap
          g.send :last_match=, 1
        else
          g.push_rubinius
          g.find_const :Globals
          if @value
            g.push_literal @name
            @value.bytecode(g)
          else
            g.swap
            g.push_literal @name
            g.swap
          end
          g.send :[]=, 2
        end
      end

      def sexp_name
        :gasgn
      end
    end

    class SplatAssignment < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        pos(g)

        g.cast_array
        @value.bytecode(g)
      end
    end

    class SplatArray < SplatAssignment
      def initialize(line, value, size)
        @line = line
        @value = value
        @size = size
      end

      def bytecode(g)
        pos(g)

        g.make_array @size
        @value.bytecode(g)
      end

      def to_sexp
        [:splat, @value.to_sexp]
      end
    end

    class SplatWrapped < SplatAssignment
      def bytecode(g)
        pos(g)

        assign = g.new_label

        g.dup
        g.push_cpath_top
        g.find_const :Array
        g.swap
        g.kind_of
        g.git assign
        g.make_array 1

        assign.set!
        @value.bytecode(g)
      end

      def to_sexp
        [:splat, @value.to_sexp]
      end
    end

    class EmptySplat < Node
      def initialize(line, size)
        @line = line
        @size = size
      end

      def bytecode(g)
        pos(g)

        g.make_array @size
      end

      def to_sexp
        [:splat]
      end
    end

    class InstanceVariableAccess < VariableAccess
      def bytecode(g)
        pos(g)

        g.push_ivar @name
      end

      def variable_defined(g, f)
        g.push :self
        g.push_literal @name
        g.send :__instance_variable_defined_eh__, 1
        g.gif f
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        variable_defined(g, f)
        g.push_literal "instance-variable"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        [:ivar, @name]
      end
    end

    class InstanceVariableAssignment < VariableAssignment
      def bytecode(g)
        pos(g)

        @value.bytecode(g) if @value
        g.set_ivar @name
      end

      def sexp_name
        :iasgn
      end
    end

    class LocalVariableAccess < VariableAccess
      include LocalVariable

      def initialize(line, name)
        @line = line
        @name = name
        @variable = nil
      end

      def bytecode(g)
        pos(g)

        unless @variable
          g.state.scope.assign_local_reference self
        end
        @variable.get_bytecode(g)
      end

      def defined(g)
        g.push_literal "local-variable"
      end

      def value_defined(g, f)
        bytecode(g)
      end

      def to_sexp
        [:lvar, @name]
      end
    end

    class LocalVariableAssignment < VariableAssignment
      include LocalVariable

      def initialize(line, name, value)
        @line = line
        @name = name
        @value = value
        @variable = nil
      end

      def bytecode(g)
        pos(g)

        g.state.scope.assign_local_reference self

        if @value
          @value.bytecode(g)
        end

        @variable.set_bytecode(g)
      end

      def sexp_name
        :lasgn
      end
    end

    class MultipleAssignment < Node
      attr_accessor :left, :right, :splat, :block

      def initialize(line, left, right, splat)
        @line = line
        @left = left
        @right = right
        @splat = nil
        @block = nil # support for |&b|

        @fixed = right.kind_of?(ArrayLiteral) ? true : false

        if splat.kind_of? Node
          if @left
            if right
              @splat = SplatAssignment.new line, splat
            else
              @splat = SplatWrapped.new line, splat
            end
          elsif @fixed
            @splat = SplatArray.new line, splat, right.body.size
          elsif right.kind_of? SplatValue
            @splat = splat
          else
            @splat = SplatWrapped.new line, splat
          end
        elsif splat and @fixed
          @splat = EmptySplat.new line, right.body.size
        end
      end

      def pad_short(g)
        short = @left.body.size - @right.body.size
        if short > 0
          short.times { g.push :nil }
          g.make_array 0 if @splat
        end
      end

      def pop_excess(g)
        excess = @right.body.size - @left.body.size
        excess.times { g.pop } if excess > 0
      end

      def make_array(g)
        size = @right.body.size - @left.body.size
        g.make_array size if size >= 0
      end

      def rotate(g)
        if @splat
          size = @left.body.size + 1
        else
          size = @right.body.size
        end
        g.rotate size
      end

      def iter_arguments
        @iter_arguments = true
      end

      def bytecode(g)
        g.cast_array unless @right or (@splat and not @left)

        if @fixed
          pad_short(g) if @left and !@splat
          @right.body.each { |x| x.bytecode(g) }
          pad_short(g) if @left and @splat

          if @left
            make_array(g) if @splat
            rotate(g)

            g.state.push_masgn
            @left.body.each do |x|
              x.bytecode(g)
              g.pop
            end
            g.state.pop_masgn

            pop_excess(g) unless @splat
          end
        else
          if @right
            @right.bytecode(g)

            g.cast_array unless @right.kind_of? ToArray
          end

          if @left
            g.state.push_masgn
            @left.body.each do |x|
              g.shift_array
              g.cast_array if x.kind_of? MultipleAssignment and x.left
              x.bytecode(g)
              g.pop
            end
            g.state.pop_masgn
          end
        end

        @splat.bytecode(g) if @splat

        if @right
          g.pop if !@fixed or @splat
          g.push :true
        end
      end

      def defined(g)
        g.push_literal "assignment"
      end

      def to_sexp
        left = @left ? @left.to_sexp : [:array]
        left << [:splat, @splat.to_sexp] if @splat
        left << @block.to_sexp if @block

        sexp = [:masgn, left]
        sexp << @right.to_sexp if @right
        sexp
      end
    end
  end
end
