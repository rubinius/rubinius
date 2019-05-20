module Rubinius
  module PEG
    class InvalidPatternError < Exception; end

    # Class Pattern is the base class for all nodes used to represent the AST
    # for a PEG. Following LPEG, the base is a pattern and not a grammar.
    class Pattern
      def graph
        if defined? Rubinius
          Rubinius::AST::AsciiGrapher.new(self, Pattern).print
        else
          inspect
        end

        nil
      end

      def visit(visitor)
      end

      # Pattern operators

      def /(other)
        Choice.new self, other
      end

      # TODO: remove after core transform for :/ is removed
      alias_method :divide, :/

      def +(other)
        Concatenation.new self, other
      end

      def -(other)
        Difference.new self, other
      end

      def *(other)
        Product.new self, other
      end

      def +@
        If.new self
      end

      def -@
        Unless.new self
      end

      def =~(string)
      end

      def match(string)
      end
    end

    class UnaryOp < Pattern
      attr_accessor :pattern

      def initialize(pattern)
        @pattern = PEG.pattern pattern
      end

      private :initialize
    end

    class BinaryOp < Pattern
      attr_accessor :first
      attr_accessor :second

      def initialize(first, second)
        @first = PEG.pattern first
        @second = PEG.pattern second
      end

      private :initialize
    end

    class Grammar < Pattern
      attr_accessor :start

      def initialize(variable=nil)
        if variable
          @start = variable.name
          set_variable variable.name, variable.pattern
        end
      end

      private :initialize

      def set_variable(name, pattern)
        variable = get_variable name
        variable.pattern = PEG.pattern pattern
        variable
      end

      def get_variable(name)
        if singleton_class.method_defined? name
          variable = send name
        else
          vname = name.to_sym
          variable = Variable.new vname
          singleton_class.thunk_method vname, variable
        end

        variable
      end

      def method_missing(sym, *args)
        name = sym.to_s

        if name[-1] == ?= and args.size == 1
          set_variable name[0..-2], args.first
        else
          get_variable name
        end
      end

      def visit(visitor)
        visitor.grammar self
      end

      def =~(string)
      end

      def match(string)
      end
    end

    class Always < Pattern
      def visit(visitor)
        visitor.always self
      end
    end

    class AnyRange < Pattern
      attr_accessor :range

      def initialize(range)
        @range = range
      end

      private :initialize

      def visit(visitor)
        visitor.any_range self
      end
    end

    class Any < Pattern
      attr_accessor :count

      def initialize(count)
        @count = count
      end

      private :initialize

      def visit(visitor)
        visitor.any self
      end
    end

    class CharacterRange < Pattern
      attr_accessor :range

      def initialize(range)
        @range = range
      end

      private :initialize

      def visit(visitor)
        visitor.character_range self
      end
    end

    class Character < Pattern
      attr_accessor :string

      def initialize(string)
        @string = string
      end

      private :initialize

      def visit(visitor)
        visitor.character self
      end
    end

    class Choice < BinaryOp
      def visit(visitor)
        visitor.choice self
      end
    end

    class Concatenation < BinaryOp
      def visit(visitor)
        visitor.concatenation self
      end
    end

    class Difference < BinaryOp
      def visit(visitor)
        visitor.difference self
      end
    end

    # Implements &patt in a PEG. The pattern succeeds if
    # patt matches. Does not consume any input.
    class If < UnaryOp
      def visit(visitor)
        visitor.if self
      end
    end

    class Never < Pattern
      def visit(visitor)
        visitor.never self
      end
    end

    class Product < Pattern
      attr_accessor :pattern
      attr_accessor :count

      def initialize(pattern, count)
        @pattern = PEG.pattern pattern
        @count = Type.coerce_to(count, Integer, :to_int).abs
      end

      private :initialize

      def visit(visitor)
        visitor.product self
      end
    end

    class Set < Pattern
      attr_accessor :set

      def initialize(set)
        @set = set.uniq
      end

      private :initialize

      def visit(visitor)
        visitor.set self
      end
    end

    # Implements !patt in a PEG. The pattern succeeds if
    # patt does not match. Does not consume any input.
    class Unless < UnaryOp
      def visit(visitor)
        visitor.unless self
      end
    end

    class Variable < Pattern
      attr_accessor :name
      attr_accessor :pattern

      def initialize(name, pattern=nil)
        @name = name.to_sym
        @pattern = PEG.pattern pattern
      end

      private :initialize

      def visit(visitor)
        visitor.variable self
      end
    end

    # Accepts a Ruby object representation of simple PEG "atoms" and returns a
    # PEG AST node.
    def pattern(spec)
      case spec
      when Array
        Set.new spec
      when Integer
        node = Any.new spec.abs
        node = If.new node if spec < 0
        node
      when String
        Character.new spec
      when Pattern
        spec
      when true
        Always.new
      when false
        Never.new
      when nil
        nil
      else
        raise InvalidPatternError, "unknown specification type for Pattern: #{spec.inspect}"
      end
    end
    module_function :pattern

    def grammar(spec=nil, pattern=nil)
      case spec
      when Symbol
        Grammar.new Variable.new(spec, pattern)
      when Variable
        Grammar.new spec
      when Pattern
        Grammar.new Variable.new(:S, spec)
      when nil
        Grammar.new
      else
        Grammar.new Variable.new(:S, pattern)
      end
    end
    module_function :grammar

    def variable(name, pattern=nil)
      PEG::Variable.new name, pattern
    end
    module_function :variable

    def self.compile(pattern)
    end

    def self.peg
      g = grammar   = grammar(:grammar)
      g.grammar     = (g.nonterminal + "=" + g.sp + g.pattern) * 1
      g.pattern     = g.alternative + (pattern("/") + g.sp + g.alternative) * 0
      g.alternative = ((+g.predicate + g.predicate + g.sp + g.suffix) /
                      (g.sp + g.suffix)) * 1
      g.predicate   = ["!&"]
      g.suffix      = g.primary + (pattern(["*+?"]) + g.sp) * 0
      g.primary     = (pattern("(") + g.sp + g.pattern + ")" + g.sp) / (pattern(1) + g.sp) /
                      g.literal / g.char_class / (g.nonterminal + -pattern("="))
      g.literal     = pattern("'") + (-pattern("'") + 1) * 0 + "'" + g.sp
      g.char_class  = (pattern("[") + (-pattern("]") +
                      ((pattern(1) + "-" + 1) / 1)) * 0) + "]" + g.sp
      g.nonterminal = (pattern("_") / ["a".."z"] / ["A".."Z"]) * 1 + g.sp
      g.sp          = pattern([" \t\n"]) * 0

      grammar
    end
  end
end
