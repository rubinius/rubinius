class Exception
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
  attr_reader :name, :args
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

