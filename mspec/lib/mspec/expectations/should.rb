class Object
  def should(matcher=nil)
    MSpec.expectation
    MSpec.actions :expectation, MSpec.current.state
    if matcher
      unless matcher.matches?(self)
        SpecExpectation.fail_with(*matcher.failure_message)
      end
    else
      SpecPositiveOperatorMatcher.new(self)
    end
  end

  def should_not(matcher=nil)
    MSpec.expectation
    MSpec.actions :expectation, MSpec.current.state
    if matcher
      if matcher.matches?(self)
        SpecExpectation.fail_with(*matcher.negative_failure_message)
      end
    else
      SpecNegativeOperatorMatcher.new(self)
    end
  end
end
