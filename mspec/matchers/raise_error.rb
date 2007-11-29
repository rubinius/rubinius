class RaiseErrorMatcher
  def initialize(exception, message)
    @exception = exception
    @message = message
  end
  
  def matches?(proc)
    begin
      proc.call
    rescue Exception => @actual
      unless @exception === @actual
        return false
      end
      
      if @message and @message != @actual.message
        return false
      end
    end
    return true
  end
  
  def failure_message
    ["Expected #{@exception}#{%[ (#{@message})] if @message}",
     "but got #{@actual.class}#{%[ (#{@actual.message})] if @actual.message}"]
  end
  
  def negative_failure_message
    ["Expected to not get #{@exception}#{%[ (#{@message})] if @message}", ""]
  end
end

class Object
  def raise_error(exception=Exception, message=nil)
    RaiseErrorMatcher.new(exception, message)
  end
end
