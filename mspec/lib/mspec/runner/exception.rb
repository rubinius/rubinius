class ExceptionState
  attr_reader :description, :describe, :it, :exception

  PATH = /#{File.expand_path(File.dirname(__FILE__) + '/../../..')}/

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
    [ExpectationNotMetError, ExpectationNotFoundError].any? { |e| @exception.is_a? e }
  end

  def message
    if @exception.message.empty?
      "<No message>"
    elsif @exception.class == ExpectationNotMetError ||
          @exception.class == ExpectationNotFoundError
      @exception.message
    else
      "#{@exception.class}: #{@exception.message}"
    end
  end

  def backtrace
    begin
      bt = @exception.awesome_backtrace.show.split "\n"
    rescue Exception
      bt = @exception.backtrace || []
    end
    bt.reject { |line| PATH =~ line }.join("\n")
  end
end
