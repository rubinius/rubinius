class ExceptionState
  attr_reader :description, :describe, :it, :exception

  def initialize(state, location, exception)
    @exception = exception

    @description = location ? "An exception occurred during: #{location}" : ""
    if state
      @description << "\n" unless @description.empty?
      @description << state.description
      @describe = state.describe
      @it = state.it
    else
      @describe = @it = ""
    end
  end

  def failure?
    @exception.is_a? ExpectationNotMetError
  end

  def message
    if @exception.message.empty?
      "<No message>"
    elsif @exception.class == ExpectationNotMetError
      @exception.message
    else
      "#{@exception.class}: #{@exception.message}"
    end
  end

  def backtrace
    begin
      return @exception.awesome_backtrace.show
    rescue Exception
      return @exception.backtrace && @exception.backtrace.join("\n")
    end
  end
end
