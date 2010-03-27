module Rubinius
  module AST
    module Transforms
      def self.register(category, name, klass)
        transform_map[name] = klass
        category_map[category] << klass
      end

      def self.transform_map
        @transform_map ||= { }
      end

      def self.category_map
        @category_map ||= Hash.new { |h, k| h[k] = [] }
      end

      def self.[](name)
        transform_map[name]
      end

      def self.category(name)
        if name == :all
          category_map.values.flatten
        else
          category_map[name]
        end
      end
    end

    ##
    # Handles block_given?
    class BlockGiven < Send
      transform :default, :block_given, "VM instruction for block_given?, iterator?"

      def self.match?(line, receiver, name, arguments, privately)
        if receiver.kind_of? Self and (name == :block_given? or name == :iterator?)
          new line, receiver, name, privately
        end
      end

      def bytecode(g)
        pos(g)
        g.push_has_block
      end
    end

    class AccessUndefined < Send
      transform :kernel, :access_undefined, "VM instruction for undefined"

      def self.match?(line, receiver, name, arguments, privately)
        if privately and name == :undefined
          new line, receiver, name, privately
        end
      end

      def bytecode(g)
        pos(g)
        g.push_undef
      end
    end

    ##
    # Handles Ruby.primitive
    class SendPrimitive < SendWithArguments
      transform :default, :primitive, "Ruby.primitive"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Ruby, name, :primitive
      end

      def bytecode(g)
        g.send_primitive @arguments.array.first.value
      end
    end

    ##
    # Handles Ruby.check_frozen
    class CheckFrozen < SendWithArguments
      transform :default, :frozen, "Ruby.check_frozen"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Ruby, name, :check_frozen
      end

      def bytecode(g)
        pos(g)
        if @arguments.array.size == 0
          g.push_self
          g.check_frozen
        else
          @arguments.array.first.bytecode(g)
          g.check_frozen
        end
      end
    end

    ##
    # Handles Rubinius.invoke_primitive
    #
    class InvokePrimitive < SendWithArguments
      transform :default, :invoke_primitive, "Rubinius.invoke_primitive"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Rubinius, name, :invoke_primitive
      end

      def bytecode(g)
        if @arguments.splat?
          raise CompileError, "splat argument passed to invoke_primitive"
        elsif @block
          raise CompileError, "block passed to invoke_primitive"
        end

        pos(g)
        selector = @arguments.array.shift
        @arguments.bytecode(g)
        g.invoke_primitive selector.value, @arguments.size
      end
    end

    ##
    # Handles Rubinius.asm
    #
    class InlineAssembly < SendWithArguments
      transform :default, :assembly, "Rubinius.asm"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Rubinius, name, :asm
      end

      def bytecode(g)
        e = Evaluator.new g, @block.arguments.names, @arguments.array
        e.execute @block.body
      end
    end

    ##
    # Handles Rubinius.privately
    #
    class SendPrivately < Send
      transform :kernel, :privately, "Rubinius.privately"

      def self.match?(line, receiver, name, arguments, privately)
        if match_send? receiver, :Rubinius, name, :privately
          new line, receiver, name, privately
        end
      end

      def block=(iter)
        @block = iter.body
      end

      def map_sends
        visit do |result, node|
          case node
          when Send, SendWithArguments
            node.privately = true
          end

          result
        end
      end

      def bytecode(g)
        map_sends

        pos(g)
        @block.bytecode(g)
      end
    end

    ##
    # Emits fast VM instructions for certain methods.
    #
    class SendFastMath < SendWithArguments
      transform :default, :fast_math, "VM instructions for math, relational methods"

      Operators = {
        :+    => :meta_send_op_plus,
        :-    => :meta_send_op_minus,
        :==   => :meta_send_op_equal,
        :===  => :meta_send_op_tequal,
        :<    => :meta_send_op_lt,
        :>    => :meta_send_op_gt
      }

      def self.match?(line, receiver, name, arguments, privately)
        return false unless op = Operators[name]
        if match_arguments? arguments, 1
          node = new line, receiver, name, arguments
          node.operator = op
          node
        end
      end

      attr_accessor :operator

      def bytecode(g)
        pos(g)

        @receiver.bytecode(g)
        @arguments.bytecode(g)

        g.__send__ @operator, g.find_literal(@name)
      end
    end

    ##
    # Emits a fast path for #new
    #
    class SendFastNew < SendWithArguments
      transform :default, :fast_new, "Fast SomeClass.new path"

      # FIXME duplicated from kernel/common/compiled_method.rb
      KernelMethodSerial = 47

      def self.match?(line, receiver, name, arguments, privately)
        name == :new
      end

      def bytecode(g)
        return super(g) if @block or @arguments.splat?

        pos(g)

        slow = g.new_label
        done = g.new_label

        @receiver.bytecode(g)
        g.dup

        if @privately
          g.check_serial_private :new, KernelMethodSerial
        else
          g.check_serial :new, KernelMethodSerial
        end
        g.gif slow

        # fast path
        g.send :allocate, 0, true
        g.dup
        @arguments.bytecode(g)
        g.send :initialize, @arguments.size, true
        g.pop

        g.goto done

        # slow path
        slow.set!
        @arguments.bytecode(g)
        g.send :new, @arguments.size, @privately

        done.set!
      end
    end

    ##
    # Emits "safe" names for certain fundamental core library methods
    #
    class SendKernelMethod < SendWithArguments
      transform :kernel, :kernel_methods, "Safe names for fundamental methods"

      Methods = {
        :/      => :divide,
        :__slash__ => :/,
        :class  => :__class__
      }

      Arguments = {
        :/      => 1,
        :__slash__ => 1,
        :class  => 0
      }

      def self.match?(line, receiver, name, arguments, privately)
        return false unless rename = Methods[name]
        if match_arguments? arguments, Arguments[name]
          new line, receiver, rename, arguments, privately
        end
      end

      def bytecode(g)
        pos(g)
        @receiver.bytecode(g)
        @arguments.bytecode(g)

        g.send @name, @arguments.size, @privately
      end
    end

    ##
    # Maps various methods to VM instructions
    #
    class SendInstructionMethod < SendWithArguments
      transform :default, :fast_system, "VM instructions for certain methods"

      Methods = {
        :__kind_of__     => :kind_of,
        :__instance_of__ => :instance_of,
        :__nil__         => :is_nil,
      }

      Arguments = {
        :__kind_of__     => 1,
        :__instance_of__ => 1,
        :__nil__         => 0,
      }

      def self.match?(line, receiver, name, arguments, privately)
        return false unless rename = Methods[name]
        if match_arguments? arguments, Arguments[name]
          new line, receiver, rename, arguments, privately
        end
      end

      def bytecode(g)
        pos(g)
        @arguments.bytecode(g)
        @receiver.bytecode(g)

        g.__send__ @name
      end
    end

    ##
    # Speeds up certain forms of Type.coerce_to
    #
    class SendFastCoerceTo < SendWithArguments
      transform :default, :fast_coerce, "Fast Type.coerce_to path"

      def self.match?(line, receiver, name, arguments, privately)
        match_send?(receiver, :Type, name, :coerce_to) and
          arguments.body.size == 3
      end

      def bytecode(g)
        pos(g)
        var = @arguments.array[0]
        const = @arguments.array[1]

        if var.kind_of? LocalVariableAccess and
           const.kind_of? ConstantAccess and const.name == :Fixnum
          done = g.new_label

          var.bytecode(g)
          g.dup
          g.send :__fixnum__, 0
          g.git done

          g.pop # remove the dup
          super(g)

          done.set!
        else
          super(g)
        end
      end
    end

    class ArrayZen < Node
      transform :magic, :array_zen, "Array Zen support"

      def self.fixup_send(acc, node, recurse=true)
        if node.kind_of? Send and
           node.receiver.kind_of? Self and
           (node.name == acc or node.name == :_)
          node.check_for_local = true
          return
        end

        if node.kind_of? ArrayLiteral
          node.body.each { |x| fixup_send(acc, x) }
        end

        return unless recurse

        node.visit do |result, n|
          case n
          when Send
            fixup_send(acc, n, false)
          when ArrayLiteral
            n.body.each { |x| fixup_send(acc, x) }
          end

          true
        end
      end

      def self.match?(line, receiver, name, arguments, privately)
        return nil unless name == :"+@"
        return nil unless receiver.kind_of? ArrayLiteral

        ary = receiver.body

        transform = ary.shift
        source = ary.shift

        unless ary.empty?
          raise SyntaxError, "invalid array zen form"
        end

        unless source.kind_of? SendWithArguments
          raise SyntaxError, "invalid array zen form"
        end

        unless source.name == :in
          raise SyntaxError, "invalid array zen form"
        end

        var = source.receiver
        source_ary = source.arguments.array.first

        # Ok! it's the form we want. That was a lot of checking.

        acc = var.name
        acc_node = LocalVariableAssignment.new(line, acc, nil)

        fixup_send(acc, transform)

        new(line, acc_node, source_ary, transform)
      end

      def initialize(line, var, source, transform)
        @line = line
        @var = var
        @source = source
        @transform = transform

        @res_set = LocalVariableAssignment.new(line, :_, nil)
        @res = LocalVariableAccess.new(line, :_)
      end

      def bytecode(g)
        pos(g)

        done = g.new_label
        top = g.new_label

        g.make_array 0
        @res_set.bytecode(g)
        g.pop

        @source.bytecode(g)
        g.send :to_iter, 0

        top.set!

        g.dup
        g.send :next, 0
        g.gif done

        g.dup
        g.send :item, 0
        @var.bytecode(g)
        g.pop

        @res.bytecode(g)
        @transform.bytecode(g)
        g.send :<<, 1
        g.pop

        g.goto top

        done.set!

        g.pop # iter
        @res.bytecode(g)
      end
    end

    ##
    # Handles loop do ... end
    #
    class SendLoop < Send
      transform :magic, :loop, "loop do ... end"

      def self.match?(line, receiver, name, arguments, privately)
        if receiver.kind_of? Self and name == :loop
          new line, receiver, name, privately
        end
      end

      def block=(iter)
        if iter.kind_of? BlockPass
          @blockarg = iter
        else
          @block = iter.body
        end
      end

      def bytecode(g)
        pos(g)
        if @block
          g.push_modifiers

          g.break = g.new_label
          g.next = g.redo = top = g.new_label
          top.set!

          @block.bytecode(g)
          g.pop

          g.check_interrupts
          g.goto top

          g.break.set!
          g.pop_modifiers
        elsif @blockarg
          g.push :self
          @blockarg.bytecode(g)
          g.send_with_block :loop, 0, true
        else
          g.push :self
          g.send :loop, 0, true
        end
      end
    end
  end
end
