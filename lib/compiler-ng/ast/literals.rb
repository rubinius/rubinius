module Rubinius
  module AST
    class ArrayLiteral < Node
      attr_accessor :body

      def self.from(p, array)
        node = ArrayLiteral.new p.compiler
        node.body = array
        node
      end

      def children
        @body
      end

      def bytecode(g)
        @body.each do |x|
          x.bytecode(g)
        end

        g.make_array @body.size
      end
    end

    class EmptyArray < Node
      def self.from(p)
        EmptyArray.new p.compiler
      end

      def bytecode(g)
        pos(g)

        g.make_array 0
      end
    end

    class False < Node
      def self.from(p)
        False.new p.compiler
      end

      def bytecode(g)
        g.push :false
      end
    end

    class True < Node
      def self.from(p)
        True.new p.compiler
      end

      def bytecode(g)
        g.push :true
      end
    end

    class Float < Node
      def self.from(p, str)
        float = Literal.new p.compiler
        float.normalize str.to_f
      end
    end

    class HashLiteral < Node
      attr_accessor :body

      def self.from(p, array)
        node = HashLiteral.new p.compiler
        node.body = array
        node
      end

      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Hash

        @body.each { |x| x.bytecode(g) }

        g.send :[], @body.size
      end
    end

    class Literal < Node
      attr_accessor :value

      def self.from(p, sym)
        literal = Literal.new p.compiler
        literal.normalize sym
      end

      # TODO: remove
      def normalize(value)
        @value = value

        case value
        when Fixnum
          nd = NumberLiteral.new(@compiler)
          nd.value = value
          return nd
        when Regexp
          nd = RegexLiteral.new(@compiler)
          nd.args(value.source, value.options)
          return nd
        when ::Range
          if value.exclude_end?
            nd = RangeExclude.new(@compiler)
          else
            nd = Range.new(@compiler)
          end

          start = NumberLiteral.new(@compiler)
          start.args value.begin

          fin = NumberLiteral.new(@compiler)
          fin.args value.end

          nd.args start, fin
          return nd
        end

        return self
      end

      def bytecode(g)
        pos(g)

        g.push_unique_literal @value
      end
    end

    class Nil < Node
      def self.from(p)
        Nil.new p.compiler
      end

      def bytecode(g)
        g.push :nil
      end
    end

    class NumberLiteral < Node
      attr_accessor :value

      def self.from(p, base, str)
        number = Literal.new p.compiler
        number.normalize(str.to_i(base))
      end

      def bytecode(g)
        g.push @value
      end
    end

    class FixnumLiteral < NumberLiteral
      def self.from(p, value)
        node = FixnumLiteral.new p.compiler
        node.value = value
        node
      end
    end

    class Range < Node
      attr_accessor :start, :finish

      def self.from(p, start, finish)
        node = Range.new p.compiler
        node.start = start
        node.finish = finish
        node
      end

      def children
        [@start, @finish]
      end

      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.send :new, 2
      end
    end

    class RangeExclude < Range
      def self.from(p, start, finish)
        node = RangeExclude.new p.compiler
        node.start = start
        node.finish = finish
        node
      end

      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.push :true

        g.send :new, 3
      end
    end

    class RegexLiteral < Node
      attr_accessor :source, :options

      def self.from(p, str, flags)
        node = RegexLiteral.new p.compiler
        node.source = str
        node.options = flags
        node
      end

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

    class StringLiteral < Node
      attr_accessor :string

      def self.from(p, str)
        node = StringLiteral.new p.compiler
        node.string = str
        node
      end

      def bytecode(g)
        pos(g)

        # TODO: change to push_unique_literal
        g.push_literal @string
        g.string_dup
      end
    end

    class DynamicString < StringLiteral
      attr_accessor :array, :options

      def self.from(p, str, array)
        node = DynamicString.new p.compiler
        node.string = str
        node.array = array
        node
      end

      def children
        @array
      end

      def bytecode(g)
        pos(g)

        @array.reverse_each do |x|
          x.bytecode(g)
        end
        g.push_literal @string
        g.string_dup

        @array.size.times do
          g.string_append
        end
      end
    end

    class DynamicSymbol < DynamicString
      def self.from(p, str, array)
        node = DynamicSymbol.new p.compiler
        node.string = str
        node.array = array
        node
      end

      def bytecode(g)
        pos(g)

        super(g)
        g.send :to_sym, 0, true
      end
    end

    class DynamicExecuteString < DynamicString
      def self.from(p, str, array)
        node = DynamicExecuteString.new p.compiler
        node.string = str
        node.array = array
        node
      end

      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true #`
      end
    end

    class DynamicRegex < DynamicString
      def self.from(p, str, array, flags)
        node = DynamicRegex.new p.compiler
        node.string = str
        node.array = array
        node.options = flags || 0
        node
      end

      def bytecode(g)
        pos(g)

        g.push_const :Regexp
        super(g)
        g.push @options
        g.send :new, 2
      end
    end

    class DynamicOnceRegex < DynamicRegex
      def self.from(p, str, array, flags)
        node = DynamicOnceRegex.new p.compiler
        node.string = str
        node.array = array
        node.options = flags || 0
        node
      end

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

    class ExecuteString < StringLiteral
      attr_accessor :string

      def self.from(p, str)
        node = ExecuteString.new p.compiler
        node.string = str
        node
      end

      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true # ` (silly vim/emacs)
      end
    end
  end
end
