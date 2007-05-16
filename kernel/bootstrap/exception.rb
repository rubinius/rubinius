# This class is not yet referenced in kernel/hints, so it must use put/at, rather than ivars
class Exception
  #ivar_as_index :message => 0, :context => 1
  def initialize(message = nil)
    self.put(0, message) # @message
    ctx = MethodContext.current.sender.sender
    self.put(1, ctx) # @context
  end

  def backtrace
    bt = self.at(1)
    if FastMethodContext === bt
      bt = Backtrace.backtrace(bt)
      self.put(1, bt)
    end

    return bt
  end

  def set_backtrace(bt)
    self.put(1, bt)
  end
  
  def to_s
    self.at(0) || self.class.to_s
  end
  
  def inspect
    "#<#{self.class.name}: #{self.to_s}>"
  end
  
  alias :message :to_s
  alias :to_str :to_s
end

class ScriptError < Exception
end

class StandardError < Exception
end

class NotImplementedError < ScriptError
end

class SyntaxError < ScriptError
  self.instance_fields = 4

  #ivar_as_index :column => 2, :line => 3

  def column
    self.at(2)
  end

  def line
    self.at(3)
  end

  def import_position(c,l)
    self.put(2, c)
    self.put(3, l)
  end
end

class SystemCallError < StandardError
  self.instance_fields = 3
  #ivar_as_index :errno => 2

  def initialize(message, errno = nil)
    super(message)
    self.put(2, errno)
  end
  
  def errno
    self.at(2)
  end
end


