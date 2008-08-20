require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/base'
require 'time'

describe PositiveOperatorMatcher, "== operator" do
  it "raises an ExpectationNotMetError when expected == actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(1) == 2
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x to equal y'" do
    Expectation.should_receive(:fail_with).with("Expected 1\n", "to equal 2\n")
    PositiveOperatorMatcher.new(1) == 2
  end

  it "does not raise an exception when expected == actual returns true" do
    PositiveOperatorMatcher.new(1) == 1
  end
end

describe PositiveOperatorMatcher, "=~ operator" do
  it "raises an ExpectationNotMetError when expected =~ actual returns false" do
    lambda {
      PositiveOperatorMatcher.new('real') =~ /fake/
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected \"x\" to match y'" do
    Expectation.should_receive(:fail_with).with("Expected \"real\"\n", "to match /fake/\n")
    PositiveOperatorMatcher.new('real') =~ /fake/
  end

  it "does not raise an exception when expected =~ actual returns true" do
    PositiveOperatorMatcher.new('real') =~ /real/
  end
end

describe PositiveOperatorMatcher, "> operator" do
  it "raises an ExpectationNotMetError when expected > actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(4) > 5
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x to be greater than y'" do
    Expectation.should_receive(:fail_with).with("Expected 4\n", "to be greater than 5\n")
    PositiveOperatorMatcher.new(4) > 5
  end

  it "does not raise an exception when expected > actual returns true" do
    PositiveOperatorMatcher.new(5) > 4
  end
end

describe PositiveOperatorMatcher, ">= operator" do
  it "raises an ExpectationNotMetError when expected >= actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(4) >= 5
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x to be greater than or equal to y'" do
    Expectation.should_receive(:fail_with).with("Expected 4\n", "to be greater than or equal to 5\n")
    PositiveOperatorMatcher.new(4) >= 5
  end

  it "does not raise an exception when expected > actual returns true" do
    PositiveOperatorMatcher.new(5) >= 4
    PositiveOperatorMatcher.new(5) >= 5
  end
end

describe PositiveOperatorMatcher, "< operater" do
  it "raises an ExpectationNotMetError when expected < actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(5) < 4
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x to be less than y'" do
    Expectation.should_receive(:fail_with).with("Expected 5\n", "to be less than 4\n")
    PositiveOperatorMatcher.new(5) < 4
  end

  it "does not raise an exception when expected < actual returns true" do
    PositiveOperatorMatcher.new(4) < 5
  end
end

describe PositiveOperatorMatcher, "<= operater" do
  it "raises an ExpectationNotMetError when expected < actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(5) <= 4
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x to be less than or equal to y'" do
    Expectation.should_receive(:fail_with).with("Expected 5\n", "to be less than or equal to 4\n")
    PositiveOperatorMatcher.new(5) <= 4
  end

  it "does not raise an exception when expected < actual returns true" do
    PositiveOperatorMatcher.new(4) <= 5
    PositiveOperatorMatcher.new(4) <= 4
  end
end

describe NegativeOperatorMatcher, "== operator" do
  it "raises an ExpectationNotMetError when expected == actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(1) == 1
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x not to equal y'" do
    Expectation.should_receive(:fail_with).with("Expected 1\n", "not to equal 1\n")
    NegativeOperatorMatcher.new(1) == 1
  end

  it "does not raise an exception when expected == actual returns false" do
    NegativeOperatorMatcher.new(1) == 2
  end
end

describe NegativeOperatorMatcher, "=~ operator" do
  it "raises an ExpectationNotMetError when expected =~ actual returns true" do
    lambda {
      NegativeOperatorMatcher.new('real') =~ /real/
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected \"x\" not to match /y/'" do
    Expectation.should_receive(:fail_with).with("Expected \"real\"\n", "not to match /real/\n")
    NegativeOperatorMatcher.new('real') =~ /real/
  end

  it "does not raise an exception when expected =~ actual returns false" do
    NegativeOperatorMatcher.new('real') =~ /fake/
  end
end

describe NegativeOperatorMatcher, "< operator" do
  it "raises an ExpectationNotMetError when expected < actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(4) < 5
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x not to be less than y'" do
    Expectation.should_receive(:fail_with).with("Expected 4\n", "not to be less than 5\n")
    NegativeOperatorMatcher.new(4) < 5
  end

  it "does not raise an exception when expected < actual returns false" do
    NegativeOperatorMatcher.new(5) < 4
  end
end

describe NegativeOperatorMatcher, "<= operator" do
  it "raises an ExpectationNotMetError when expected <= actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(4) <= 5
    }.should raise_error(ExpectationNotMetError)
    lambda {
      NegativeOperatorMatcher.new(5) <= 5
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x not to be less than or equal to y'" do
    Expectation.should_receive(:fail_with).with("Expected 4\n", "not to be less than or equal to 5\n")
    NegativeOperatorMatcher.new(4) <= 5
  end

  it "does not raise an exception when expected <= actual returns false" do
    NegativeOperatorMatcher.new(5) <= 4
  end
end

describe NegativeOperatorMatcher, "> operator" do
  it "raises an ExpectationNotMetError when expected > actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(5) > 4
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x not to be greater than y'" do
    Expectation.should_receive(:fail_with).with("Expected 5\n", "not to be greater than 4\n")
    NegativeOperatorMatcher.new(5) > 4
  end

  it "does not raise an exception when expected > actual returns false" do
    NegativeOperatorMatcher.new(4) > 5
  end
end

describe NegativeOperatorMatcher, ">= operator" do
  it "raises an ExpectationNotMetError when expected >= actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(5) >= 4
    }.should raise_error(ExpectationNotMetError)
    lambda {
      NegativeOperatorMatcher.new(5) >= 5
    }.should raise_error(ExpectationNotMetError)
  end

  it "provides a failure message that 'Expected x not to be greater than or equal to y'" do
    Expectation.should_receive(:fail_with).with("Expected 5\n", "not to be greater than or equal to 4\n")
    NegativeOperatorMatcher.new(5) >= 4
  end

  it "does not raise an exception when expected >= actual returns false" do
    NegativeOperatorMatcher.new(4) >= 5
  end
end
