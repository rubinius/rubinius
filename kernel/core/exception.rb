# depends on: class.rb

class Exception
  
  ivar_as_index :__ivars => 0, :message => 1, :context => 2
  def message; @message ; end

  def initialize(message = nil)
    @message = message
    @context = nil
    @backtrace = nil
  end

  def backtrace
    return @backtrace if @backtrace
    return nil unless @context
    awesome_backtrace.to_mri
  end
  
  def awesome_backtrace
    return nil unless @context
    @backtrace = Backtrace.backtrace(@context)
  end

  def set_backtrace(bt)
    @backtrace = bt
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
    @context
  end

  def context=(other)
    @context = other
  end
  
  def location
    ctx = self.context
    [ctx.file.to_s, ctx.line]
  end
end

##
# Primitive fails from opcode "send_primitive"

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

  def message
    msg = super
    msg = "#{file}:#{@line}: #{msg}" if file && @line
    msg
  end
end

class SystemCallError < StandardError
  def errno; @errno ; end

  def initialize(message, errno = nil)
    if message.is_a?(Integer) && errno.nil?
      errno   = message
      message = "Unknown error"
    end
    super(message)
    @errno = errno
  end
end

##
# Raised when you try to return from a block when not allowed.  Never seen by
# ruby code.

class IllegalLongReturn < LocalJumpError
  attr_reader :return_value
end

##
# Abstract class for implementing flow control in Rubinius.  Never seen by
# ruby code.

class FlowControlException < Exception
end

##
# Flow control exception used to implement return inside an ensure.  Never
# seen by ruby code.

class ReturnException < FlowControlException
  attr_reader :return_value

  def initialize(val)
    super(nil) # HACK
    @return_value = val
  end
end

##
# Raised when returning from a block to handle proper flow control.  Never
# seen by ruby code.

class LongReturnException < FlowControlException
  attr_reader :value
  attr_reader :is_return

  def return_value=(val)
    @value = val
    @is_return = true
  end

  def break_value=(val)
    @value = val
    @is_return = false
  end

end
