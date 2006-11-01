class Exception
  def initialize(message)
    ctx = MethodContext.current.sender.sender
    # puts "EXCEPTION: #{ctx}"
    put 0, message
    put 1, Backtrace.backtrace(ctx)
  end
  
  def backtrace
    bk = at(1)
    if MethodContext === bk
      bk = Backtrace.backtrace(bk)
      self.put 1, bk
    end
    
    return bk
  end
  
  def message
    at(0)
  end
end
