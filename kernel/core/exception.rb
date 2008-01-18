# depends on: class.rb

class Exception
  
  ivar_as_index :__ivars => 0, :message => 1, :context => 2
  def message; @message ; end

  def initialize(message = nil)
    @message = message
    @context = nil
  end

  def backtrace
    return nil unless @context
    unless @context.kind_of? Array
      @context = Backtrace.backtrace(@context).to_mri
    end
    @context
  end
  
  def awesome_backtrace
    return nil unless @context
    unless @context.kind_of? Backtrace
      @context = Backtrace.backtrace(@context)
    end
    @context
  end

  def set_backtrace(bt)
    @context = bt
  end
  
  def to_s
    @message || self.class.to_s
  end
  
  def inspect
    "#<#{self.class.name}: #{self.to_s}>"
  end
  
  alias_method :message, :to_s
  alias_method :to_str, :to_s
  
  def self.exception(message=nil)
    self.new(message)
  end
  
  def exception(message=nil)
    if message
      self.class.new(message)
    else 
      self
    end
  end
  
  def context
    if @context.kind_of? Backtrace
      return @context.top_context
    end
    
    return @context
  end
  
  def location
    ctx = self.context
    [ctx.file.to_s, ctx.line]
  end
end

# primitive fails from opcode "send_primitive"
class PrimitiveFailure < Exception
end

class ScriptError < Exception
end

class StandardError < Exception
end

class SignalException < Exception
end

class NoMemoryError < Exception
end

class ZeroDivisionError < StandardError
end

class ArgumentError < StandardError
end

class IndexError < StandardError
end

class RangeError < StandardError
end

class FloatDomainError < RangeError
end

class LocalJumpError < StandardError
end

class NameError < StandardError
  attr_reader :name
  def initialize(*args)
    super(args.shift)
    @name = args.shift
  end
end

class NoMethodError < NameError
  attr_reader :name
  attr_reader :args
  def initialize(*arguments)
    super(arguments.shift)
    @name = arguments.shift
    @args = arguments.shift
  end
end

class RuntimeError < StandardError
end

class SecurityError < StandardError
end

class SystemStackError < StandardError
end

class ThreadError < StandardError
end

class TypeError < StandardError
end

class FloatDomainError < RangeError
end

class RegexpError < StandardError
end

class LoadError < ScriptError
end

class NotImplementedError < ScriptError
end

class Interrupt < SignalException
end

class IOError < StandardError
end

class EOFError < IOError
end

class LocalJumpError < StandardError
end

class NotImplementedError < ScriptError
end

class SyntaxError < ScriptError
  attr_accessor :column
  attr_accessor :line
  attr_accessor :file
  attr_accessor :code

  def import_position(c,l, code)
    @column = c
    @line = l
    @code = code
  end
end

class SystemCallError < StandardError
  def errno; @errno ; end

  def initialize(message, errno = nil)
    super(message)
    @errno = errno
  end
end

class IllegalLongReturn
  attr_reader :return_value
end

class ReturnException
  attr_reader :return_value

  def initialize(val)
    super(nil) # HACK
    @return_value = val
  end
end

class LongReturnException
  attr_reader :return_value

  def initialize(val)
    super(nil) # HACK
    @return_value = val
  end
end
