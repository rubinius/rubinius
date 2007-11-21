class Object
  def should(matcher=nil)
    if matcher
      raise Expectation.fail_with(*matcher.failure_message) unless matcher.matches?(self)
    else
      PositiveOperatorMatcher.new(self)
    end
  end
  
  def should_not(matcher=nil)
    if matcher
      raise Expectation.fail_with(*matcher.negative_failure_message) if matcher.matches?(self)
    else
      NegativeOperatorMatcher.new(self)
    end
  end
end
