require File.dirname(__FILE__) + '/formatters/dotted'

class Object
  def it(msg)
    if @formatter.nil?
      @formatter = DottedFormatter.new
    end
    
    @formatter.before_it(msg)
    begin
      yield
    rescue Exception => e
      @formatter.exception(e)
    end
    @formatter.after_it(msg)
  end

  def describe(msg)
    @formatter.before_describe(msg)
    yield
    @formatter.after_describe(msg)
  end
end
