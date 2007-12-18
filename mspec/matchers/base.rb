class PositiveOperatorMatcher
  def initialize(actual)
    @actual = actual
  end
  
  def ==(expected)
    unless @actual == expected
      Expectation.fail_with("Expected '#{@actual}'", "to equal '#{expected}'")
    end
  end
  
  def =~(expected)
    unless @actual =~ expected
      Expectation.fail_with("Expected '#{@actual}'", "to match '#{expected.inspect}'")
    end
  end
end

class NegativeOperatorMatcher
  def initialize(actual)
    @actual = actual
  end
  
  def ==(expected)
    if @actual == expected
      Expectation.fail_with("Expected '#{@actual}'", "not to equal '#{expected}'")
    end
  end
  
  def =~(expected)
    if @actual =~ expected
      Expectation.fail_with("Expected '#{@actual}'", "not to match '#{expected.inspect}'")
    end
  end
end
