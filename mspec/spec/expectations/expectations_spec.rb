require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'

describe ExpectationNotMetError do
  it "is a subclass of StandardError" do
    ExpectationNotMetError.ancestors.should include(StandardError)
  end
end

describe Expectation, "#fail_with" do
  it "raises an ExpectationNotMetError" do
    lambda {
      Expectation.fail_with "expected this", "to equal that"
    }.should raise_error(ExpectationNotMetError, "expected this to equal that")
  end
end
