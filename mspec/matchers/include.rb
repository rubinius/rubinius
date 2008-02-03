class IncludeMatcher
  def initialize(*expected)
    @expected = expected
  end
  
  def matches?(actual)
    @actual = actual
    @expected.each do |e|
      @element = e
      unless @actual.include?(e)
        return false
      end
    end
    return true
  end
  
  def failure_message
    ["Expected #{@actual.inspect}", "to include #{@element}"]
  end
  
  def negative_failure_message
    ["Expected #{@actual.inspect}", "not to include #{@element}"]
  end
end

class << self
  undef :include
  def include(*expected)
    IncludeMatcher.new(*expected)
  end
end
