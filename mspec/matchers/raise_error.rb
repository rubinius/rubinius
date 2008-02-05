class RaiseErrorMatcher
  def initialize(exception, message)
    @exception = exception
    @message = message
  end

  def matches?(proc)
    proc.call
    return false
  rescue Exception => @actual
    return false unless @exception === @actual
    if @message then
      case @message
      when String then
        return false if @message != @actual.message
      when Regexp then
        return false if @message !~ @actual.message
      end
    end
    return true
  end

  def failure_message
    message = ["Expected #{@exception}#{%[ (#{@message})] if @message}"]

    if @actual then
      message << "but got #{@actual.class}#{%[ (#{@actual.message})] if @actual.message}"
    else
      message << "but no exception was raised"
    end

    message
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
