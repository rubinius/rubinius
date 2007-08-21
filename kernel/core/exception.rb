class Exception
  
  ivar_as_index :__ivars => 0, :message => 1, :context => 2
  def message; @message ; end
  def context; @context ; end

  def initialize(message = nil)
    @message = message
    @context = nil
  end

  def backtrace
    return nil unless @context
    
    bt = @context
    unless bt.kind_of? Backtrace
      bt = Backtrace.backtrace(bt)
      @context = bt
    end

    return bt
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
  
  alias :message :to_s
  alias :to_str :to_s
  
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

class SystemStackError < Exception
end

class ThreadError < StandardError
end

class TypeError < StandardError
end

class FloatDomainError < RangeError
end

class RegexpError < StandardError
end

class ZeroDivisionError < StandardError
end

class NoMemoryError < Exception
end

class ScriptError < Exception
end

class LoadError < ScriptError
end

class NotImplementedError < ScriptError
end

class SignalException < Exception
end

class Interrupt < SignalException
end

class EOFError < IOError
end

class LocalJumpError < StandardError
end

# primitive fails from opcode "send_primitive"
class PrimitiveFailure < Exception
end

class ScriptError < Exception
end

class StandardError < Exception
end

class NotImplementedError < ScriptError
end

class SyntaxError < ScriptError
  def column; @column ; end
  def line  ; @line   ; end

  def import_position(c,l)
    @column = c
    @line = l
  end
end

class SystemCallError < StandardError
  def errno; @errno ; end

  def initialize(message, errno = nil)
    super(message)
    @errno = errno
  end
end

