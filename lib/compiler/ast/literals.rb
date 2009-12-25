module Rubinius
  module AST
    class ArrayLiteral < Node
      attr_accessor :body

      def initialize(line, array)
        @line = line
        @body = array
      end

      def bytecode(g)
        pos(g)

        @body.each do |x|
          x.bytecode(g)
        end

        g.make_array @body.size
      end
    end

    class EmptyArray < Node
      def bytecode(g)
        pos(g)

        g.make_array 0
      end
    end

    class False < Node
      def bytecode(g)
        pos(g)

        g.push :false
      end

      def defined(g)
        g.push_literal "false"
      end
    end

    class True < Node
      def bytecode(g)
        pos(g)

        g.push :true
      end

      def defined(g)
        g.push_literal "true"
      end
    end

    class Float < Node
      attr_accessor :value

      def initialize(line, str)
        @line = line
        @value = str.to_f
      end

      def bytecode(g)
        pos(g)

        g.push_unique_literal @value
      end
    end

    class HashLiteral < Node
      attr_accessor :array

      def initialize(line, array)
        @line = line
        @array = array
      end

      def bytecode(g)
        pos(g)

        count = @array.size
        i = 0

        g.push_cpath_top
        g.find_const :Hash
        g.push count / 2
        g.send :new_from_literal, 1

        while i < count
          k = @array[i]
          v = @array[i + 1]

          g.dup
          k.bytecode(g)
          v.bytecode(g)
          g.send :[]=, 2
          g.pop

          i += 2
        end
      end
    end

    class SymbolLiteral < Node
      attr_accessor :value

      def initialize(line, sym)
        @line = line
        @value = sym
      end

      def bytecode(g)
        pos(g)

        g.push_literal @value
      end

      def defined(g)
        g.push_literal "expression"
      end
    end

    class Nil < Node
      def bytecode(g)
        pos(g)

        g.push :nil
      end

      def defined(g)
        g.push_literal "nil"
      end
    end

    class NumberLiteral < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        pos(g)

        g.push_unique_literal @value
      end

      def defined(g)
        g.push_literal "expression"
      end
    end

    class FixnumLiteral < NumberLiteral
      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        pos(g)

        g.push @value
      end
    end

    class Range < Node
      attr_accessor :start, :finish

      def initialize(line, start, finish)
        @line = line
        @start = start
        @finish = finish
      end

      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.send :new, 2
      end
    end

    class RangeExclude < Range
      def initialize(line, start, finish)
        @line = line
        @start = start
        @finish = finish
      end

      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.push :true

        g.send :new, 3
      end
    end

    class RegexLiteral < Node
      attr_accessor :source, :options

      def initialize(line, str, flags)
        @line = line
        @source = str
        @options = flags
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

      def initialize(line, str)
        @line = line
        @string = str
      end

      def bytecode(g)
        pos(g)

        # TODO: change to push_unique_literal
        g.push_literal @string
        g.string_dup
      end

      def defined(g)
        g.push_literal "expression"
      end
    end

    class DynamicString < StringLiteral
      attr_accessor :array, :options

      def initialize(line, str, array)
        @line = line
        @string = str
        @array = array
      end

      # This extensive logic is 100% for optimizing rather ridiculous edge
      # cases for string interpolation and I (brixen) do not think it is
      # worthwhile.
      #
      # Some examples:
      #
      #  "#{}"
      #  "#{} foo"
      #  "foo #{}"
      #  "#{}#{}"
      #  "#{bar}"
      #
      # Also, as Zocx pointed out in IRC, the following code is not compatible
      # in Rubinius because we convert an empty evstr into "" directly in
      # Melbourne parse tree to AST processor rather than calling #to_s on
      # 'nil'.
      #
      # def nil.to_s; "hello"; end
      # a = "#{}" # => "hello" in MRI
      #
      # We also do not handle any case where #to_s does not actually return a
      # String instance.
      #
      def bytecode(g)
        pos(g)

        if @string.empty?
          if @array.size == 1
            case x = @array.first
            when StringLiteral
              x.bytecode(g)
            else
              x.bytecode(g)
              g.string_dup
            end
            return
          end

          prefix = false
        else
          prefix = true
          g.push_literal @string
        end

        total = 0
        @array.each do |x|
          case x
          when StringLiteral
            unless x.string.empty?
              g.push_literal x.string
              total += 1
            end
          else
            x.bytecode(g)
            total += 1
          end
        end

        if prefix
          if total == 0
            g.string_dup
            return
          end
          total += 1
        else
          if total == 0
            g.push_literal ""
            g.string_dup
            return
          elsif total == 1
            g.string_dup
            return
          end
        end

        g.string_build total
      end
    end

    class DynamicSymbol < DynamicString
      def bytecode(g)
        super(g)
        g.send :to_sym, 0, true
      end
    end

    class DynamicExecuteString < DynamicString
      def bytecode(g)
        g.push :self
        super(g)
        g.send :`, 1, true #`
      end
    end

    class DynamicRegex < DynamicString
      def initialize(line, str, array, flags)
        super line, str, array
        @options = flags || 0
      end

      def bytecode(g)
        g.push_const :Regexp
        super(g)
        g.push @options
        g.send :new, 2
      end
    end

    class DynamicOnceRegex < DynamicRegex
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
      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true # ` (silly vim/emacs)
      end
    end
  end
end
