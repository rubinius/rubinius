##
# Used for the Rubinius::asm Compiler hook.

class Compiler::ExecuteContext
  def initialize(who)
    @self = who
    @file = nil
    @line = 0
    @locals = {}
  end

  attr_accessor :self, :file, :line

  def execute(node)
    node.execute self
  end

  def get_local(name)
    @locals[name]
  end

  def set_local(name, val)
    @locals[name] = val
  end

end

class Compiler::Node
  class Script
    def execute(e)
      e.file = @file
      e.line = @line

      @child.execute(e)
      return true
    end
  end

  class Newline
    def execute(e)
      @child.execute(e)
    end
  end

  class True
    def execute(e)
      true
    end
  end

  class False
    def execute(e)
      false
    end
  end

  class Nil
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
      @body.each do |x|
        val = x.execute(e)
      end

      return val
    end
  end

  class LocalVariable
    def execute(e)
      e.get_local @name
    end
  end

  class LocalAssignment
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
      args = @body.map { |x| x.execute(e) }
      Hash[*args]
    end
  end

  class IVar
    def execute(e)
      e.self.instance_variable_get @name
    end
  end

  class IVarAssign
    def execute(e)
      e.self.instance_variable_set @name, @value.execute(e)
    end
  end

  class GVar
    def execute(e)
      ::Rubinius::Globals[@name]
    end
  end

  class GVarAssign
    def execute(e)
      ::Rubinius::Globals[@name] = @value.execute(e)
    end
  end

  class ConstFind
    def execute(e)
      Object.const_get @name
    end
  end

  class ConstAccess
    def execute(e)
      par = @parent.execute(e)
      par.const_get @name
    end
  end

  class ConstAtTop
    def execute(e)
      Object.const_get @name
    end
  end

  class Call
    def execute(e)
      args = @arguments.map { |a| a.execute(e) }
      recv = @object.nil? ? e.self : @object.execute(e)

      recv.__send__ @method, *args
    end
  end

  class Yield
    def execute(e)
      if @arguments.kind_of? Array
        args = @arguments.map { |x| x.execute(e) }
      else
        args = @arguments.execute(e)
      end

      e.block.call(*args)
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
