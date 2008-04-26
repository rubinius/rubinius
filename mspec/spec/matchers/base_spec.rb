require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/base'
require 'time'

describe PositiveOperatorMatcher, "== operator" do
  it "raises an ExpectationNotMetError when expected == actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(1) == 2
    }.should raise_error(ExpectationNotMetError, "Expected 1 to equal 2")
  end

  it "does not raise an exception when expected == actual returns true" do
    lambda {
      PositiveOperatorMatcher.new(1) == 1
    }.should_not raise_error
  end
end

describe PositiveOperatorMatcher, "=~ operator" do
  it "raises an ExpectationNotMetError when expected =~ actual returns false" do
    lambda {
      PositiveOperatorMatcher.new('real') =~ /fake/
    }.should raise_error(ExpectationNotMetError, %(Expected "real" to match /fake/))
  end

  it "does not raise an exception when expected =~ actual returns true" do
    lambda {
      PositiveOperatorMatcher.new('real') =~ /real/
    }.should_not raise_error
  end
end

describe PositiveOperatorMatcher, "> operator" do
  it "raises an ExpectationNotMetError when expected > actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(4) > 5
    }.should raise_error(ExpectationNotMetError, "Expected 4 to be greater than 5")
  end

  it "does not raise an exception when expected > actual returns true" do
    lambda {
      PositiveOperatorMatcher.new(5) > 4
    }.should_not raise_error
  end
end

describe PositiveOperatorMatcher, ">= operator" do
  it "raises an ExpectationNotMetError when expected >= actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(4) >= 5
    }.should raise_error(ExpectationNotMetError, "Expected 4 to be greater than or equal to 5")
  end

  it "does not raise an exception when expected > actual returns true" do
    lambda {
      PositiveOperatorMatcher.new(5) >= 4
      PositiveOperatorMatcher.new(5) >= 5
    }.should_not raise_error
  end
end

describe PositiveOperatorMatcher, "< operater" do
  it "raises an ExpectationNotMetError when expected < actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(5) < 4
    }.should raise_error(ExpectationNotMetError, "Expected 5 to be less than 4")
  end

  it "does not raise an exception when expected < actual returns true" do
    lambda {
      PositiveOperatorMatcher.new(4) < 5
    }.should_not raise_error
  end
end

describe PositiveOperatorMatcher, "<= operater" do
  it "raises an ExpectationNotMetError when expected < actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(5) <= 4
    }.should raise_error(ExpectationNotMetError, "Expected 5 to be less than or equal to 4")
  end

  it "does not raise an exception when expected < actual returns true" do
    lambda {
      PositiveOperatorMatcher.new(4) <= 5
      PositiveOperatorMatcher.new(4) <= 4
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher, "== operator" do
  it "raises an ExpectationNotMetError when expected == actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(1) == 1
    }.should raise_error(ExpectationNotMetError, "Expected 1 not to equal 1")
  end

  it "does not raise an exception when expected == actual returns false" do
    lambda {
      NegativeOperatorMatcher.new(1) == 2
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher, "=~ operator" do
  it "raises an ExpectationNotMetError when expected =~ actual returns true" do
    lambda {
      NegativeOperatorMatcher.new('real') =~ /real/
    }.should raise_error(ExpectationNotMetError, %(Expected "real" not to match /real/))
  end

  it "does not raise an exception when expected =~ actual returns false" do
    lambda {
      NegativeOperatorMatcher.new('real') =~ /fake/
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher, "< operator" do
  it "raises an ExpectationNotMetError when expected < actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(4) < 5
    }.should raise_error(ExpectationNotMetError, "Expected 4 not to be less than 5")
  end

  it "does not raise an exception when expected < actual returns false" do
    lambda {
      NegativeOperatorMatcher.new(5) < 4
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher, "<= operator" do
  it "raises an ExpectationNotMetError when expected <= actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(4) <= 5
    }.should raise_error(ExpectationNotMetError, "Expected 4 not to be less than or equal to 5")
    lambda {
      NegativeOperatorMatcher.new(5) <= 5
    }.should raise_error(ExpectationNotMetError, "Expected 5 not to be less than or equal to 5")
  end

  it "does not raise an exception when expected <= actual returns false" do
    lambda {
      NegativeOperatorMatcher.new(5) <= 4
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher, "> operator" do
  it "raises an ExpectationNotMetError when expected > actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(5) > 4
    }.should raise_error(ExpectationNotMetError, "Expected 5 not to be greater than 4")
  end

  it "does not raise an exception when expected > actual returns false" do
    lambda {
      NegativeOperatorMatcher.new(4) > 5
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher, ">= operator" do
  it "raises an ExpectationNotMetError when expected >= actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(5) >= 4
    }.should raise_error(ExpectationNotMetError, "Expected 5 not to be greater than or equal to 4")
    lambda {
      NegativeOperatorMatcher.new(5) >= 5
    }.should raise_error(ExpectationNotMetError, "Expected 5 not to be greater than or equal to 5")
  end

  it "does not raise an exception when expected >= actual returns false" do
    lambda {
      NegativeOperatorMatcher.new(4) >= 5
    }.should_not raise_error
  end
end
