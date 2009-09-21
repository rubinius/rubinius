module Rubinius
  module AST
    class Node
      attr_accessor :line

      def self.transform(category, name)
        Transforms.register category, name, self
      end

      def initialize(line)
        @line = line
      end

      def pos(g)
        g.set_line @line
      end

      # TODO: transform nodes that use this
      def kernel?
        false
      end

      def children
        []
      end

      def bytecode(g)
      end

      def in_rescue
      end

      def in_block
      end

      def in_module
      end

      def in_masgn
      end

      def visit(arg=true, &block)
        children.each do |child|
          if child
            next unless ch_arg = block.call(arg, child)
            child.visit(ch_arg, &block)
          end
        end
      end

      def ascii_graph
        AsciiGrapher.new(self).print
      end

      # Called if used as the lhs of an ||=. Expected to yield if the
      # value was not found, so the bytecode for it to be emitted.
      def or_bytecode(g)
        found = g.new_label
        bytecode(g)
        g.dup
        g.git found
        g.pop
        yield
        found.set!
      end
    end
  end
end
