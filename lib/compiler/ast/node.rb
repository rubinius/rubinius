# -*- encoding: us-ascii -*-

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
        blk.post_args = arguments.post_args
        blk.total_args = arguments.total_args
        blk.splat_index = arguments.splat_index
        blk.block_index = arguments.block_index

        blk
      end

      def new_generator(g, name, arguments=nil)
        meth = g.class.new
        meth.name = name
        meth.file = g.file

        if arguments
          meth.required_args = arguments.required_args
          meth.post_args = arguments.post_args
          meth.total_args = arguments.total_args
          meth.splat_index = arguments.splat_index
          meth.block_index = arguments.block_index
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
        g.pop
        g.push :nil
      end

      def value_defined(g, f)
        bytecode(g)
      end

      # This method implements a sort of tree iterator, yielding each Node
      # instance to the provided block with the first argument to #walk. If
      # the block returns a non-true value, the walk is terminated.
      #
      # This method is really an iterator, not a Visitor pattern.
      def walk(arg=true, &block)
        children do |child|
          if ch_arg = block.call(arg, child)
            child.walk(ch_arg, &block)
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

      def to_sexp
        [:node, self.class.name]
      end

      # Yields each child of this Node to the block. Additionally, for any
      # attribute that is an Array, yields each element that is a Node.
      def children
        instance_variables.each do |var|
          child = instance_variable_get var
          if child.kind_of? Node
            yield child
          elsif child.kind_of? Array
            child.each { |x| yield x if x.kind_of? Node }
          end
        end
      end

      # The equivalent of Some::Module.demodulize.underscore in ActiveSupport.
      # The code is shamelessly borrowed as well.
      def node_name
        name = self.class.name.gsub(/^.*::/, '')
        name.gsub!(/([A-Z]+)([A-Z][a-z])/,'\1_\2')
        name.gsub!(/([a-z\d])([A-Z])/,'\1_\2')
        name.downcase!
        name
      end

      # Supports the Visitor pattern on a tree of Nodes. The +visitor+ should
      # be an object that responds to methods named after the Node subclasses.
      # The method called is determined by the #node_name method. Passes both
      # the node and its parent so that the visitor can maintain nesting
      # information if desired.
      #
      # The #visit implements a read-only traversal of the tree. To modify the
      # tree, see the #transform methed.
      def visit(visitor, parent=nil)
        visitor.__send__ self.node_name, self, parent
        children { |c| c.visit visitor, self }
      end

      # Called by #transform to update the child of a Node instance. The
      # default just calls the attr_accessor for the child. However, Node
      # subclasses that must synchronize other internal state can override
      # this method.
      def set_child(name, node)
        send :"#{name}=", node
      end

      # Yields each attribute and its name to the block.
      def attributes
        instance_variables.each do |var|
          child = instance_variable_get var
          name = var.to_s[1..-1]
          yield child, name
        end
      end

      # A fixed-point algorithm for transforming an AST with a visitor. The
      # traversal is top-down. The visitor object's method corresponding to
      # each node (see #node_name) is called for each node, passing the node
      # and its parent.
      #
      # To replace the node in the tree, the visitor method should return a
      # new node; otherwise, return the existing node. The visitor is free to
      # change values in the node, but substituting a node causes the entire
      # tree to be walked repeatedly until no modifications are made.
      def transform(visitor, parent=nil, state=nil)
        state ||= TransformState.new

        node = visitor.send :"node_#{node_name}", self, parent
        state.modify unless equal? node

        node.attributes do |attr, name|
          if attr.kind_of? Node
            child = attr.transform visitor, node, state
            unless attr.equal? child
              state.modify
              node.set_child name, child
            end
          elsif attr.kind_of? Array
            attr.each_with_index do |x, i|
              if x.kind_of? Node
                child = x.transform visitor, node, state
                unless x.equal? child
                  state.modify
                  attr[i] = child
                end
              end
            end
          end
        end

        # Repeat the walk until the tree is not modified.
        if parent.nil? and state.modified?
          state.unmodify
          node = transform visitor, nil, state
        end

        node
      end

      # Manage the state of the #transform method.
      class TransformState
        def initialized
          @modified = false
        end

        def modified?
          @modified
        end

        def modify
          @modified = true
        end

        def unmodify
          @modified = false
        end
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
      attr_accessor :check_for_locals

      class << self
        attr_accessor :flip_flops
      end

      self.flip_flops ||= 0

      def initialize(scope)
        @scope = scope
        @ensure = 0
        @block = 0
        @masgn = 0
        @loop = 0
        @op_asgn = 0
        @rescue = []
        @name = []
        @check_for_locals = true
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

      def flip_flops
        State.flip_flops
      end

      def push_flip_flop
        State.flip_flops += 1
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

      def push_op_asgn
        @op_asgn += 1
      end

      def pop_op_asgn
        @op_asgn -= 1 if op_asgn?
      end

      def op_asgn?
        @op_asgn > 0
      end

      def push_super(scope)
        @super = scope
      end

      alias_method :super?, :super

      def push_eval(scope)
        @eval = scope
      end

      alias_method :eval?, :eval

      def push_loop
        @loop += 1
      end

      def pop_loop
        @loop -= 1 if loop?
      end

      def loop?
        @loop > 0
      end
    end
  end
end
