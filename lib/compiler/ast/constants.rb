module Rubinius
  module AST
    class ScopedConstant < Node
      attr_accessor :parent, :name

      def initialize(line, parent, name)
        @line = line
        @parent = parent
        @name = name
      end

      def bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.find_const @name
      end

      def assign_bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.push_literal @name
      end

      def masgn_bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.swap
        g.push_literal @name
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        value_defined(g, f, false)

        g.pop
        g.push_literal "constant"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def value_defined(g, f, const_missing=true)
        # Save the current exception into a stack local
        g.push_exception_state
        outer_exc_state = g.new_stack_local
        g.set_stack_local outer_exc_state
        g.pop

        ex = g.new_label
        ok = g.new_label
        g.setup_unwind ex, RescueType

        @parent.bytecode(g)
        g.push_literal @name
        g.push(const_missing ? :true : :false)
        g.invoke_primitive :vm_const_defined_under, 3

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.push_stack_local outer_exc_state
        g.restore_exception_state
        g.goto f

        ok.set!
      end

      def to_sexp
        [:colon2, @parent.to_sexp, @name]
      end

      alias_method :assign_sexp, :to_sexp
    end

    class ToplevelConstant < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const @name
      end

      def assign_bytecode(g)
        pos(g)

        g.push_cpath_top
        g.push_literal @name
      end

      def masgn_bytecode(g)
        pos(g)

        g.push_cpath_top
        g.swap
        g.push_literal @name
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        value_defined(g, f)

        g.pop
        g.push_literal "constant"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def value_defined(g, f)
        # Save the current exception into a stack local
        g.push_exception_state
        outer_exc_state = g.new_stack_local
        g.set_stack_local outer_exc_state
        g.pop

        ex = g.new_label
        ok = g.new_label
        g.setup_unwind ex, RescueType

        g.push_cpath_top
        g.push_literal @name
        g.push :false
        g.invoke_primitive :vm_const_defined_under, 3

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.push_stack_local outer_exc_state
        g.restore_exception_state
        g.goto f

        ok.set!
      end

      def to_sexp
        [:colon3, @name]
      end

      alias_method :assign_sexp, :to_sexp
    end

    class ConstantAccess < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        pos(g)

        g.push_const @name
      end

      def assign_bytecode(g)
        pos(g)

        g.push_scope
        g.push_literal @name
      end

      def masgn_bytecode(g)
        pos(g)

        g.push_scope
        g.swap
        g.push_literal @name
      end

      def defined(g)
        f = g.new_label
        done = g.new_label

        value_defined(g, f)

        g.pop
        g.push_literal "constant"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def value_defined(g, f)
        # Save the current exception into a stack local
        g.push_exception_state
        outer_exc_state = g.new_stack_local
        g.set_stack_local outer_exc_state
        g.pop

        ex = g.new_label
        ok = g.new_label
        g.setup_unwind ex, RescueType

        g.push_literal @name
        g.invoke_primitive :vm_const_defined, 1

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.push_stack_local outer_exc_state
        g.restore_exception_state
        g.goto f

        ok.set!
      end

      def assign_sexp
        @name
      end

      def to_sexp
        [:const, @name]
      end
    end

    class ConstantAssignment < Node
      attr_accessor :constant, :value

      def initialize(line, expr, value)
        @line = line
        @value = value

        if expr.kind_of? Symbol
          @constant = ConstantAccess.new line, expr
        else
          @constant = expr
        end
      end

      def masgn_bytecode(g)
        @constant.masgn_bytecode(g)
        g.swap
        g.send :const_set, 2
      end

      def bytecode(g)
        pos(g)

        return masgn_bytecode(g) if g.state.masgn?

        @constant.assign_bytecode(g)
        @value.bytecode(g)
        g.send :const_set, 2
      end

      def to_sexp
        sexp = [:cdecl, @constant.assign_sexp]
        sexp << @value.to_sexp if @value
        sexp
      end
    end
  end
end
