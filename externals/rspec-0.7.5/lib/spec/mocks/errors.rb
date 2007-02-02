module Spec
  module Mocks
    class MockExpectationError < Expectations::ExpectationNotMetError
    end
    
    class AmbiguousReturnError < Expectations::ExpectationNotMetError
    end
  end
end

