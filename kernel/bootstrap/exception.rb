class Exception
  ivar_as_index :__ivars => 0, :message => 1, :context => 2
  def message; @message ; end
  def context; @context ; end

  def initialize(message = nil)
    @message = message
    ctx = MethodContext.current.sender.sender
    @context = ctx
  end

  def backtrace
    bt = @context
    if FastMethodContext === bt
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

