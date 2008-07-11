require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'

describe ExpectationNotMetError do
  it "is a subclass of StandardError" do
    ExpectationNotMetError.ancestors.should include(StandardError)
  end
end

describe ExpectationNotFoundError do
  it "is a subclass of StandardError" do
    ExpectationNotFoundError.ancestors.should include(StandardError)
  end
end

describe ExpectationNotFoundError, "#message" do
  it "returns 'No behavior expectation was found in the example'" do
    m = ExpectationNotFoundError.new.message
    m.should == "No behavior expectation was found in the example"
  end
end

describe Expectation, "#fail_with" do
  it "raises an ExpectationNotMetError" do
    lambda {
      Expectation.fail_with "expected this", "to equal that"
    }.should raise_error(ExpectationNotMetError, "expected this to equal that")
  end
end
