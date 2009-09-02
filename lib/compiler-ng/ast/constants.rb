module Rubinius
  module AST
    class ConstAccess < Node
      attr_accessor :parent, :name

      def initialize(line, parent, name)
        @line = line
        @parent = parent
        @name = name
      end

      def children
        [@parent]
      end

      def bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.find_const @name
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
    end

    class ConstSet < Node
      attr_accessor :parent, :name, :value

      def initialize(line, name, value)
        @line = line
        @value = value

        if name.kind_of? Symbol
          @name = ConstName.new line, name
        else
          @parent = name.parent
          @name = ConstName.new line, name.name
        end
      end

      def children
        [@parent, @value]
      end

      def bytecode(g)
        pos(g)

        if kernel?
          if @parent
            @parent.bytecode(g)
            @value.bytecode(g)
            g.set_const @name.name, true
          else
            @value.bytecode(g) if @value
            g.set_const @name
          end
        else
          @parent ? @parent.bytecode(g) : g.push_scope
          g.swap unless @value

          @name.bytecode(g)

          @value ? @value.bytecode(g) : g.swap
          g.send :__const_set__, 2
        end
      end
    end

    class ConstName < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        g.push_literal @name
      end
    end
  end
end
