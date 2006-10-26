class Exception
  def initialize(message)
    ctx = MethodContext.current.sender.sender
    # puts "EXCEPTION: #{ctx}"
    put 0, message
    put 1, Backtrace.backtrace(ctx)
  end
  
  def backtrace
    at(1)
  end
  
  def message
    at(0)
  end
end
