# -*- encoding: us-ascii -*-

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
    # Handles Rubinius.primitive
    class SendPrimitive < SendWithArguments
      transform :default, :primitive, "Rubinius.primitive"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Rubinius, name, :primitive
      end

      def bytecode(g)
        g.send_primitive @arguments.array.first.value
      end
    end

    ##
    # Handles Rubinius.check_frozen
    class CheckFrozen < SendWithArguments
      transform :default, :frozen, "Rubinius.check_frozen"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Rubinius, name, :check_frozen
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
    # Handles Rubinius.call_custom
    #
    class CallCustom < SendWithArguments
      transform :default, :call_custom, "Rubinius.call_custom"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Rubinius, name, :call_custom
      end

      def bytecode(g)
        if @arguments.splat?
          raise CompileError, "splat argument passed to call_custom"
        elsif @block
          raise CompileError, "block passed to call_custom"
        end

        pos(g)
        rec = @arguments.array.shift
        rec.bytecode(g)

        selector = @arguments.array.shift
        @arguments.bytecode(g)
        g.call_custom selector.value, @arguments.size
      end
    end

    ##
    # Handles Rubinius.single_block_arg
    #
    # Given the following code:
    #
    #   m { |x| ... }
    #
    # In Ruby 1.8, this has the following semantics:
    #
    #   * x == nil if no values are yielded
    #   * x == val if one value is yielded
    #   * x == [p, q, r, ...] if more than one value is yielded
    #   * x == [a, b, c, ...] if one Array is yielded
    #
    # In Ruby 1.9, this has the following semantics:
    #
    #   * x == nil if no values are yielded
    #   * x == val if one value is yielded
    #   * x == p if yield(p, q, r, ...)
    #   * x == [a, b, c, ...] if one Array is yielded
    #
    # However, in Ruby 1.9, the Enumerator code manually implements the 1.8
    # block argument semantics. This transform exposes the VM instruction we
    # use in 1.8 mode (cast_for_single_block_arg) so we can use it in 1.9 mode
    # for Enumerator.
    #
    class SingleBlockArg < SendWithArguments
      transform :default, :single_block_arg, "Rubinius.single_block_arg"

      def self.match?(line, receiver, name, arguments, privately)
        match_send? receiver, :Rubinius, name, :single_block_arg
      end

      def bytecode(g)
        if @arguments.splat?
          raise CompileError, "splat argument passed to single_block_arg"
        elsif @block
          raise CompileError, "block passed to single_block_arg"
        end

        pos(g)
        g.cast_for_single_block_arg
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
        walk do |result, node|
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
    # Emits a fast path for #new
    #
    class SendFastNew < SendWithArguments
      transform :default, :fast_new, "Fast SomeClass.new path"

      # FIXME duplicated from kernel/common/compiled_code.rb
      KernelMethodSerial = 47

      def self.match?(line, receiver, name, arguments, privately)
        # ignore vcall style
        return false if !arguments and privately
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
    # Speeds up certain forms of Type.coerce_to
    #
    class SendFastCoerceTo < SendWithArguments
      transform :default, :fast_coerce, "Fast Rubinius::Type.coerce_to path"

      def self.match?(line, receiver, name, arguments, privately)
        methods = [:coerce_to, :check_convert_type, :try_convert]
        receiver.kind_of?(TypeConstant) && methods.include?(name) &&
          arguments.body.size == 3
      end

      def bytecode(g)
        pos(g)
        var = @arguments.array[0]
        const = @arguments.array[1]

        if (var.kind_of?(LocalVariableAccess) ||
            var.kind_of?(InstanceVariableAccess)) and
           (const.kind_of?(ConstantAccess) ||
            const.kind_of?(ScopedConstant) ||
            const.kind_of?(ToplevelConstant))
          done = g.new_label

          var.bytecode(g)
          g.dup
          const.bytecode(g)
          g.swap
          g.kind_of
          g.git done
          g.pop
          super(g)

          done.set!
        else
          super(g)
        end
      end
    end
  end
end
