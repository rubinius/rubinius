# -*- encoding: us-ascii -*-

##
# Used for the Rubinius::asm Compiler hook.

module Rubinius
  module AST
    class Evaluator
      attr_accessor :self

      def initialize(generator, names, arguments)
        @self = generator
        @locals = {}

        names.each_with_index do |name, index|
          set_local name, arguments[index]
        end
      end

      def execute(node)
        node.execute self
      end

      def get_local(name)
        @locals[name]
      end

      def set_local(name, value)
        @locals[name] = value
      end
    end

    class Container
      def execute(e)
        @body.execute(e)
        return true
      end
    end

    class TrueLiteral
      def execute(e)
        true
      end
    end

    class FalseLiteral
      def execute(e)
        false
      end
    end

    class NilLiteral
      def execute(e)
        nil
      end
    end

    class Self
      def execute(e)
        e.self
      end
    end

    class And
      def execute(e)
        @left.execute(e) and @right.execute(e)
      end
    end

    class Or
      def execute(e)
        @left.execute(e) or @right.execute(e)
      end
    end

    class Not
      def execute(e)
        not @child.execute(e)
      end
    end

    class Negate
      def execute(e)
        -@child.execute(e)
      end
    end

    class NumberLiteral
      def execute(e)
        @value
      end
    end

    class Literal
      def execute(e)
        @value
      end
    end

    class RegexLiteral
      def execute(e)
        ::Regexp.new(@source, @options)
      end
    end

    class StringLiteral
      def execute(e)
        @string.dup
      end
    end

    class DynamicString
      def execute(e)
        str = @string.dup
        @body.each do |x|
          str << x.execute(e)
        end

        str
      end
    end

    class DynamicRegex
      def execute(e)
        ::Regexp.new super(e)
      end
    end

    class DynamicOnceRegex
      def execute(e)
        @value ||= super(e)
      end
    end

    class If
      def execute(e)
        if @condition.execute(e)
          @then.execute(e) if @then
        else
          @else.execute(e) if @else
        end
      end
    end

    class While
      def execute(e)
        if @check_first
          while @condition.execute(e)
            @body.execute(e)
          end
        else
          begin
            @body.execute(e)
          end while @condition.execute(e)
        end
      end
    end

    class Until
      def execute(e)
        if @check_first
          until @condition.execute(e)
            @body.execute(e)
          end
        else
          begin
            @body.execute(e)
          end until @condition.execute(e)
        end
      end
    end

    class Block
      def execute(e)
        val = nil
        @array.each do |x|
          val = x.execute(e)
        end

        return val
      end
    end

    class LocalVariableAccess
      def execute(e)
        e.get_local @name
      end
    end

    class LocalVariableAssignment
      def execute(e)
        e.set_local @name, @value.execute(e)
      end
    end

    class ArrayLiteral
      def execute(e)
        @body.map { |x| x.execute(e) }
      end
    end

    class EmptyArray
      def execute(e)
        []
      end
    end

    class HashLiteral
      def execute(e)
        args = @array.map { |x| x.execute(e) }
        Hash[*args]
      end
    end

    class SymbolLiteral
      def execute(e)
        @value
      end
    end

    class InstanceVariableAccess
      def execute(e)
        e.self.instance_variable_get @name
      end
    end

    class InstanceVariableAssignment
      def execute(e)
        e.self.instance_variable_set @name, @value.execute(e)
      end
    end

    class GlobalVariableAccess
      def execute(e)
        ::Rubinius::Globals[@name]
      end
    end

    class GlobalVariableAssignment
      def execute(e)
        ::Rubinius::Globals[@name] = @value.execute(e)
      end
    end

    class ConstantAccess
      def execute(e)
        Object.const_get @name
      end
    end

    class ScopedConstant
      def execute(e)
        parent = @parent.execute(e)
        parent.const_get @name
      end
    end

    class ToplevelConstant
      def execute(e)
        Object.const_get @name
      end
    end

    class Send
      def execute_receiver(e)
        if @receiver.kind_of? Self
          e.self
        else
          @receiver.execute(e)
        end
      end

      def execute(e)
        receiver = execute_receiver(e)

        receiver.__send__ @name
      end
    end

    class SendWithArguments
      def execute(e)
        arguments = @arguments.execute(e)
        receiver = execute_receiver(e)

        receiver.__send__ @name, *arguments
      end
    end

    class ActualArguments
      def execute(e)
        array = @array.map { |x| x.execute(e) }
        array << @splat.execute if @splat.kind_of? SplatValue
        array
      end
    end

    class Yield
      def execute(e)
        # TODO
        e.block.call(*@arguments.execute(e))
      end
    end

    class ExecuteString
      def execute(e)
        `#{@string.execute(e)}`
      end
    end

    class ToString
      def execute(e)
        @child.execute(e).to_s
      end
    end

    class DynamicExecuteString
      def execute(e)
        `#{super(e)}`
      end
    end
  end
end
