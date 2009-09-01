module Rubinius
  module AST
    class SplatValue < Node
      attr_accessor :value

      def self.from(p, value)
        node = SplatValue.new p.compiler
        node.value = value
        node
      end

      def children
        [@value]
      end

      def bytecode(g)
        @value.bytecode(g)
        g.cast_array unless @value.kind_of? ArrayLiteral
      end
    end

    class ConcatArgs < Node
      attr_accessor :array, :rest, :size

      def self.from(p, array, rest)
        node = ConcatArgs.new p.compiler
        node.array = array
        node.size = array.body.size
        node.rest = rest
        node
      end

      def children
        [@array, @rest]
      end

      def bytecode(g)
        @array.bytecode(g)
        @rest.bytecode(g)
        g.cast_array
        g.send :+, 1
      end
    end

    class SValue < Node
      attr_accessor :value

      def self.from(p, expr)
        node = SValue.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        @value.bytecode(g)
        if @value.kind_of? SplatValue
          done = g.new_label

          g.dup
          g.send :size, 0
          g.push 1
          g.send :>, 1
          g.git done

          g.push 0
          g.send :at, 1

          done.set!
        end
      end
    end

    class ToArray < Node
      attr_accessor :value

      def self.from(p, expr)
        node = ToArray.new p.compiler
        node.value = expr
        node
      end

      def children
        [@value]
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.cast_array
      end
    end

    class ToString < Node
      attr_accessor :value

      def self.from(p, value)
        node = ToString.new p.compiler
        node.value = value
        node
      end

      def children
        [@value]
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.send :to_s, 0, true
      end
    end
  end
end
