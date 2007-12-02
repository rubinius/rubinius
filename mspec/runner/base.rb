require 'mspec/runner/formatters/dotted'

class Object
  def formatter
    if @formatter.nil?
      @formatter = DottedFormatter.new
    end
    @formatter
  end
    
  def it(msg)
    formatter.before_it(msg)
    begin
      yield
    rescue Exception => e
      formatter.exception(e)
    end
    formatter.after_it(msg)
  end

  def describe(msg)
    formatter.before_describe(msg)
    yield
    formatter.after_describe(msg)
  end
end
