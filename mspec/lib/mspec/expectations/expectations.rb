class ExpectationNotMetError < StandardError; end
class ExpectationNotFoundError < StandardError
  def message
    "No behavior expectation was found in the example"
  end
end

class Expectation
  def self.fail_with(expected, actual)
    if expected.to_s.size + actual.to_s.size > 80
      message = expected.to_s.chomp + "\n" + actual.to_s
    else
      message = expected.to_s + " " + actual.to_s
    end
    Kernel.raise ExpectationNotMetError, message
  end
end
