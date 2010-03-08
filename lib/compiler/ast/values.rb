module Rubinius
  module AST
    class SplatValue < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        @value.bytecode(g)
        g.cast_array unless @value.kind_of? ArrayLiteral
      end

      def to_sexp
        [:splat, @value.to_sexp]
      end
    end

    class ConcatArgs < Node
      attr_accessor :array, :rest, :size

      def initialize(line, array, rest)
        @line = line
        @array = array
        @size = array.body.size
        @rest = rest
      end

      def bytecode(g)
        @array.bytecode(g)
        @rest.bytecode(g)
        g.cast_array
        g.send :+, 1
      end

      def to_sexp
        [:argscat, @array.to_sexp, @rest.to_sexp]
      end
    end

    class SValue < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
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

      def to_sexp
        [:svalue, @value.to_sexp]
      end
    end

    class ToArray < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.cast_multi_value
      end

      def to_sexp
        [:to_ary, @value.to_sexp]
      end
    end

    class ToString < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.send :to_s, 0, true
      end

      def value_defined(g, f)
        if @value
          @value.value_defined(g, f)
        end
      end

      def to_sexp
        sexp = [:evstr]
        sexp << @value.to_sexp if @value
        sexp
      end
    end
  end
end
