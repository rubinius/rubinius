class Exception
  def self.exception(message=nil)
    new(message)
  end
  def exception(message=nil)
    if message
      self.class.new(message)
    else self
    end
  end
  def to_s
    message || self.class.name
  end
  alias :to_str :to_s
  def set_backtrace(bt)
    put 1, bt
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
end

class NoMethodError < NameError
end

class RuntimeError < StandardError
end

class SecurityError < StandardError
end

class SystemCallError < StandardError
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

### FIXME: This is defined in bootstrap/exception.rb -- bad inheritance
# class SyntaxError < ScriptError
# end

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

