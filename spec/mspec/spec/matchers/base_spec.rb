require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../matchers/base'

describe PositiveOperatorMatcher do
  it "raises ExpectationNotMetError when expected == actual returns false" do
    lambda {
      PositiveOperatorMatcher.new(1) == 2
    }.should raise_error(ExpectationNotMetError, "Expected 1 to equal 2")
  end
  
  it "does not raise an exception when expected == actual returns true" do
    lambda {
      PositiveOperatorMatcher.new(1) == 1
    }.should_not raise_error
  end
  
  it "raises ExpectationNotMetError when expected =~ actual returns false" do
    lambda {
      PositiveOperatorMatcher.new('real') =~ /fake/
    }.should raise_error(ExpectationNotMetError, "Expected real to match /fake/")
  end
  
  it "does not raise an exception when expected =~ actual returns true" do
    lambda {
      PositiveOperatorMatcher.new('real') =~ /real/
    }.should_not raise_error
  end
end

describe NegativeOperatorMatcher do
  it "raises ExpectationNotMetError when expected == actual returns true" do
    lambda {
      NegativeOperatorMatcher.new(1) == 1
    }.should raise_error(ExpectationNotMetError, "Expected     1 not to equal 1")
  end
  
  it "does not raise an exception when expected == actual returns false" do
    lambda {
      NegativeOperatorMatcher.new(1) == 2
    }.should_not raise_error
  end
  
  it "raises ExpectationNotMetError when expected =~ actual returns true" do
    lambda {
      NegativeOperatorMatcher.new('real') =~ /real/
    }.should raise_error(ExpectationNotMetError, "Expected     real not to match /real/")
  end
  
  it "does not raise an exception when expected =~ actual returns false" do
    lambda {
      NegativeOperatorMatcher.new('real') =~ /fake/
    }.should_not raise_error
  end
end
