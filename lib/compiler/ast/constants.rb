module Rubinius
  module AST
    class ConstAccess < Node
      attr_accessor :parent, :name

      def initialize(line, parent, name)
        @line = line
        @parent = parent
        @name = name
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
        ex = g.new_label
        ok = g.new_label
        g.setup_unwind ex, RescueType

        bytecode(g)

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.goto f

        ok.set!
      end

      def bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.find_const @name
      end

      def to_sexp
        [:colon2, @parent.to_sexp, @name]
      end
    end

    class ConstAtTop < Node
      attr_accessor :parent, :name

      def initialize(line, name)
        @line = line
        @name = name
        @parent = TopLevel.new line
      end

      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const @name
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
        ex = g.new_label
        ok = g.new_label
        g.setup_unwind ex, RescueType

        bytecode(g)

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.goto f

        ok.set!
      end

      def to_sexp
        [:colon3, @name]
      end
    end

    class TopLevel < Node
      def bytecode(g)
        g.push_cpath_top
      end
    end

    class ConstFind < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        pos(g)
        g.push_const @name
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
        ex = g.new_label
        ok = g.new_label
        g.setup_unwind ex, RescueType

        bytecode(g)

        g.pop_unwind
        g.goto ok

        ex.set!
        g.clear_exception
        g.goto f

        ok.set!
      end

      def to_sexp
        [:const, @name]
      end
    end

    class ConstSet < Node
      attr_accessor :parent, :name, :value

      def initialize(line, name, value)
        @line = line
        @value = value
        @parent = nil

        if name.kind_of? Symbol
          @name = ConstName.new line, name
        else
          @parent = name.parent
          @name = ConstName.new line, name.name
        end
      end

      def masgn_bytecode(g)
        g.swap
        @name.bytecode(g)
        g.swap
        g.send :const_set, 2
      end

      def bytecode(g)
        pos(g)

        @parent ? @parent.bytecode(g) : g.push_scope

        return masgn_bytecode(g) if g.state.masgn?

        @name.bytecode(g)
        @value.bytecode(g)
        g.send :const_set, 2
      end

      def to_sexp
        if @parent.kind_of?(TopLevel)
          name = [:colon3, @name.to_sexp]
        elsif @parent
          name = [:colon2, @parent.to_sexp,  @name.to_sexp]
        else
          name = @name.to_sexp
        end

        sexp = [:cdecl, name]
        sexp << @value.to_sexp if @value
        sexp
      end
    end

    class ConstName < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        pos(g)
        g.push_literal @name
      end

      def to_sexp
        @name
      end
    end
  end
end
