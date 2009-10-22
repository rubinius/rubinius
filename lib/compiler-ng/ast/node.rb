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

      def defined(g)
        g.push_const :Rubinius
        g.push_scope
        g.send :active_path, 0
        g.push @line
        g.send :unrecognized_defined, 2

        g.push :nil
      end

      def receiver_defined(g, f)
        g.goto f
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

    # In a perfect world, each AST node would fully encapsulate its state. But
    # in the real world, some state exists across the AST rather than just in
    # a node. For example, some nodes need to emit different bytecode when in
    # a rescue.
    #
    # This class maintains state needed as the AST is walked to generate
    # bytecode. An instance of State is pushed onto a stack in the Generator
    # instance as each ClosedScope or Iter is entered, and popped when left.
    class State
      attr_reader :scope

      def initialize(scope)
        @scope = scope
        @rescue = 0
        @ensure = 0
        @block = 0
        @masgn = 0
      end

      def push_rescue
        @rescue += 1
      end

      def pop_rescue
        @rescue -= 1 if rescue?
      end

      def rescue?
        @rescue > 0
      end

      def push_ensure
        @ensure += 1
      end

      def pop_ensure
        @ensure -= 1 if ensure?
      end

      def ensure?
        @ensure > 0
      end

      def push_block
        @block += 1
      end

      def pop_block
        @block -= 1 if block?
      end

      def block?
        @block > 0
      end

      def push_masgn
        @masgn += 1
      end

      def pop_masgn
        @masgn -= 1 if masgn?
      end

      def masgn?
        @masgn > 0
      end
    end
  end
end
