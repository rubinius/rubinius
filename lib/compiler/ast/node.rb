module Rubinius
  module AST
    class Node
      attr_accessor :line

      def self.transform(category, name, comment)
        Transforms.register category, name, self
        @transform_name = name
        @transform_comment = comment
        @transform_kind = :call
      end

      def self.transform_name
        @transform_name
      end

      def self.transform_comment
        @transform_comment
      end

      def self.transform_kind
        @transform_kind
      end

      def self.transform_kind=(k)
        @transform_kind = k
      end

      def self.match_send?(node, receiver, method, name)
        node.kind_of? ConstantAccess and
          node.name == receiver and
          method == name
      end

      def self.match_arguments?(arguments, count)
        case arguments
        when ArrayLiteral
          arguments.body.size == count
        when nil
          count == 0
        else
          false
        end
      end

      def initialize(line)
        @line = line
      end

      def pos(g)
        g.set_line @line
      end

      def new_block_generator(g, arguments)
        blk = g.class.new
        blk.name = g.state.name || :__block__
        blk.file = g.file
        blk.for_block = true

        blk.required_args = arguments.required_args
        blk.total_args = arguments.total_args

        blk
      end

      def new_generator(g, name, arguments=nil)
        meth = g.class.new
        meth.name = name
        meth.file = g.file

        if arguments
          meth.required_args = arguments.required_args
          meth.total_args = arguments.total_args
          meth.splat_index = arguments.splat_index
        end

        meth
      end

      def bytecode(g)
      end

      def defined(g)
        g.push_rubinius
        g.push_scope
        g.send :active_path, 0
        g.push @line
        g.send :unrecognized_defined, 2

        g.push :nil
      end

      def value_defined(g, f)
        bytecode(g)
      end

      def visit(arg=true, &block)
        instance_variables.each do |name|
          child = instance_variable_get name
          next unless child.kind_of? Node
          next unless ch_arg = block.call(arg, child)
          child.visit(ch_arg, &block)
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

      def to_sexp
        [:node, self.class.name]
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
      attr_reader :scope, :super, :eval

      def initialize(scope)
        @scope = scope
        @ensure = 0
        @block = 0
        @masgn = 0
        @rescue = []
        @name = []
      end

      def push_name(name)
        @name.push name
      end

      def pop_name
        @name.pop
      end

      def name
        @name.last
      end

      def push_rescue(val)
        @rescue.push(val)
      end

      def pop_rescue
        @rescue.pop if rescue?
      end

      def rescue?
        @rescue.last
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

      def push_super(scope)
        @super = scope
      end

      alias_method :super?, :super

      def push_eval(scope)
        @eval = scope
      end

      alias_method :eval?, :eval
    end
  end
end
