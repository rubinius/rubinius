module Rubinius
  module AST
    class Node
      attr_accessor :line

      def self.transform(category, name, comment)
        Transforms.register category, name, self
        @transform_name = name
        @transform_comment = comment
      end

      def self.transform_name
        @transform_name
      end

      def self.transform_comment
        @transform_comment
      end

      def self.match_send?(node, receiver, method, name)
        node.kind_of? ConstFind and
          node.name == receiver and
          method == name
      end

      def self.match_arguments?(arguments, count)
        (arguments and arguments.body.size == count) or
          (arguments.nil? and count == 0)
      end

      def initialize(line)
        @line = line
      end

      def pos(g)
        g.set_line @line
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
