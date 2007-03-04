class Exception
  def initialize(message = nil)
    ctx = MethodContext.current.sender.sender
    # puts "EXCEPTION: #{ctx}"
    put 0, message
    put 1, ctx
  end

  def backtrace
    bk = at(1)
    if FastMethodContext === bk
      bk = Backtrace.backtrace(bk)
      self.put 1, bk
    end

    return bk
  end
  
  def to_s
    at(0) or self.class.to_s
  end
  
  def inspect
    "#<#{self.class.name}: #{self.to_s}>"
  end
  
  alias :message :to_s
  alias :to_str :to_s
end

class ScriptError < Exception
end

class NotImplementedError < ScriptError
end

class SyntaxError
  self.instance_fields = 4

  ivar_as_index :column => 2, :line => 3

  def column
    @column
  end

  def line
    @line
  end

  def import_position(c,l)
    @column = c
    @line = l
  end
end
