module Rubinius
  module AST
    class ConstAccess < Node
      attr_accessor :parent, :name

      def self.from(p, parent, name)
        node = ConstAccess.new p.compiler
        node.parent = parent
        node.name = name
        node
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

      def self.from(p, name)
        node = ConstAtTop.new p.compiler
        node.name = name
        node.parent = TopLevel.from p
        node
      end

      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const @name
      end
    end

    class TopLevel < Node
      def self.from(p)
        TopLevel.new p.compiler
      end

      def bytecode(g)
        g.push_cpath_top
      end
    end

    class ConstFind < Node
      attr_accessor :name

      def self.from(p, name)
        node = ConstFind.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
        pos(g)
        g.push_const @name
      end
    end

    class ConstSet < Node
      attr_accessor :parent, :name, :value

      def self.from(p, name, value)
        node = ConstSet.new p.compiler
        case name
        when Symbol
          node.name = ConstName.from p, name
        else
          node.parent = name.parent
          node.name = ConstName.from p, name.name
        end
        node.value = value
        node
      end

      def children
        [@parent, @value]
      end

      def bytecode(g)
        pos(g)

        if @compiler.kernel?
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

      def self.from(p, name)
        node = ConstName.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
        g.push_literal @name
      end
    end
  end
end
