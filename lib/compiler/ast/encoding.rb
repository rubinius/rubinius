# -*- encoding: us-ascii -*-

module Rubinius
  module AST
    class Encoding < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        pos(g)

        g.push_literal Rubinius::Compiler::Runtime
        g.push_literal @name
        g.send :get_encoding, 1
      end

      def to_sexp
        [:encoding, @name]
      end
    end
  end
end
