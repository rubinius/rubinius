class IncludeMatcher
  def initialize(*expected)
    @expected = *expected
  end
  
  def matches?(*actual)
    @actual = actual
    @actual.each do |a|
      unless @expected.include?(a)
        return false
      end
    end
    return true
  end
  
  def failure_message
    ["Expected #{@expected.inspect}", "to include #{@actual.inspect}"]
  end
  
  def negative_failure_message
    ["Expected #{@expected.inspect}", "not to include #{@actual.inspect}"]
  end
end

class Object
  def include(expected)
    IncludeMatcher.new(expected)
  end
end
