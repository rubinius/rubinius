class PositiveOperatorMatcher
  def initialize(actual)
    @actual = actual
  end
  
  def ==(expected)
    unless @actual == expected
      Expectation.fail_with("Expected #{@actual.inspect}", "to equal #{expected.inspect}")
    end
  end
  
  def <(expected)
    unless @actual < expected
      Expectation.fail_with("Expected #{@actual.inspect}",
                            "to be less than #{expected.inspect}")
    end
  end

  def <=(expected)
    unless @actual <= expected
      Expectation.fail_with("Expected #{@actual.inspect}",
                            "to be less than or equal to #{expected.inspect}")
    end
  end

  def >(expected)
    unless @actual > expected
      Expectation.fail_with("Expected #{@actual.inspect}",
                            "to be greater than #{expected.inspect}")
    end
  end
  
  def >=(expected)
    unless @actual >= expected
      Expectation.fail_with("Expected #{@actual.inspect}",
                            "to be greater than or equal to #{expected.inspect}")
    end
  end
  
  def =~(expected)
    unless @actual =~ expected
      Expectation.fail_with("Expected #{@actual.inspect}", "to match #{expected.inspect}")
    end
  end
end

class NegativeOperatorMatcher
  def initialize(actual)
    @actual = actual
  end
  
  def ==(expected)
    if @actual == expected
      Expectation.fail_with("Expected #{@actual.inspect}", "not to equal #{expected.inspect}")
    end
  end
  
  def <(expected)
    if @actual < expected
      Expectation.fail_with("Expected #{@actual.inspect}", 
                            "not to be less than #{expected.inspect}")
    end
  end
  
  def <=(expected)
    if @actual <= expected
      Expectation.fail_with("Expected #{@actual.inspect}", 
                            "not to be less than or equal to #{expected.inspect}")
    end
  end
  
  def >(expected)
    if @actual > expected
      Expectation.fail_with("Expected #{@actual.inspect}", 
                            "not to be greater than #{expected.inspect}")
    end
  end
  
  def >=(expected)
    if @actual >= expected
      Expectation.fail_with("Expected #{@actual.inspect}", 
                            "not to be greater than or equal to #{expected.inspect}")
    end
  end
  
  def =~(expected)
    if @actual =~ expected
      Expectation.fail_with("Expected #{@actual.inspect}", "not to match #{expected.inspect}")
    end
  end
end
