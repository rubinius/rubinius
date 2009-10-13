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
        category_map[name]
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
        g.push_block
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

        @block.bytecode(g)
      end
    end

    ##
    # Emits fast VM instructions for certain methods.
    #
    class SendFastMath < SendWithArguments
      transform :default, :fastmath, "VM instructions for math, relational methods"

      Operators = {
        :+    => :meta_send_op_plus,
        :-    => :meta_send_op_minus,
        :==   => :meta_send_op_equal,
        :===  => :meta_send_op_tequal,
        :<    => :meta_send_op_lt,
        :>    => :meta_send_op_gt
      }

      def self.match?(line, receiver, name, arguments, privately)
        if op = Operators[name] and arguments.body.size == 1
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

        g.add @operator, g.find_literal(@name)
      end
    end

    ##
    # Emits a fast path for #new
    #
    class SendFastNew < SendWithArguments
      transform :default, :fastnew, "Fast SomeClass.new path"

      # FIXME duplicated from kernel/common/compiled_method.rb
      KernelMethodSerial = 47

      def self.match?(line, receiver, name, arguments, privately)
        name == :new
      end

      def bytecode(g)
        return super(g) if @block or @arguments.splat?

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
        :class  => :__class__
      }

      Arguments = {
        :/      => 1,
        :class  => 0
      }

      def self.match?(line, receiver, name, arguments, privately)
        return false unless rename = Methods[name]
        if match_arguments? arguments, Arguments[name]
          new line, receiver, rename, arguments, privately
        end
      end

      def bytecode(g)
        @receiver.bytecode(g)
        @arguments.bytecode(g)

        g.send @name, @arguments.size, @privately
      end
    end

    ##
    # Maps various methods to VM instructions
    #
    class SendInstructionMethod < SendWithArguments
      transform :kernel, :fastsystem, "VM instructions for certain methods"

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
        @arguments.bytecode(g)
        @receiver.bytecode(g)

        g.add @name
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
        var = @arguments.array[0]
        const = @arguments.array[1]

        if var.kind_of? LocalVariableAccess and
           const.kind_of? ConstFind and const.name == :Fixnum
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

    ##
    # Handles loop do ... end
    #
    class SendLoop < Send
      transform :default, :loop, "loop do ... end"

      def self.match?(line, receiver, name, arguments, privately)
        if receiver.kind_of? Self and name == :loop
          new line, receiver, name, privately
        end
      end

      def block=(iter)
        @block = iter.body
      end

      def bytecode(g)
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
      end
    end
  end
end
