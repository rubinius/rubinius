require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../matchers/base'

class Object
  alias_method :spec_should, :should
  alias_method :spec_should_not, :should_not
end
require File.dirname(__FILE__) + '/../../expectations/should'
class Object
  alias_method :mspec_should, :should
  alias_method :mspec_should_not, :should_not
end

def spec_runner
  obj = Object.new
  def obj.method_missing(*) self end
  obj
end

# Adapted from RSpec 1.0.8
describe Object, "#should" do  
  before :each do
    class Object; alias_method :should, :mspec_should; end
    @target = "target"
    @matcher = mock("matcher")
  end
  
  after :each do
    class Object; alias_method :should, :spec_should; end
  end
  
  it "accepts and interacts with a matcher" do
    @matcher.should_receive(:matches?).with(@target).and_return(true)
    @target.should @matcher
  end
  
  it "calls #failure_message when matcher #matches? returns false" do
    @matcher.should_receive(:matches?).with(@target).and_return(false)
    @matcher.should_receive(:failure_message).and_return(["expected", "actual"])
    @target.should @matcher rescue nil
  end

  it "raises an ExpectationNotMetError when matcher #matches? returns false" do
    @matcher.should_receive(:matches?).with(@target).and_return(false)
    @matcher.should_receive(:failure_message).and_return(["expected", "actual"])
    lambda {
      @target.should @matcher
    }.should raise_error(ExpectationNotMetError, "expected actual")
  end
  
  it "returns a PostiveOperatorMatcher instance when not passed a matcher" do
    matcher = should
    class Object; alias_method :should, :spec_should; end
    matcher.should be_instance_of(PositiveOperatorMatcher)
  end
end

describe Object, "#should_not" do
  before :each do
    class Object; alias_method :should, :mspec_should; end
    class Object; alias_method :should_not, :mspec_should_not; end
    @target = "target"
    @matcher = mock("matcher")
  end
  
  after :each do
    class Object; alias_method :should, :spec_should; end
    class Object; alias_method :should_not, :spec_should_not; end
  end
  
  it "accepts and interacts with a matcher" do
    @matcher.should_receive(:matches?).with(@target).and_return(false)
    @target.should_not @matcher
  end
  
  it "calls #negative_failure_message when matcher.matches? returns true" do
    @matcher.should_receive(:matches?).with(@target).and_return(true)
    @matcher.should_receive(:negative_failure_message).and_return(["expected", "actual"])
    @target.should_not @matcher rescue nil
  end
  
  it "raises an ExpectationNotMetError when matcher.matches? returns true" do
    @matcher.should_receive(:matches?).with(@target).and_return(true)
    @matcher.should_receive(:negative_failure_message).and_return(["expected", "actual"])
    lambda {
      @target.should_not @matcher
    }.should raise_error(ExpectationNotMetError, "expected actual")
  end
  
  it "returns a NegativeOperatorMatcher instance when not passed a matcher" do
    matcher = should_not
    class Object; alias_method :should, :spec_should; end
    matcher.should be_instance_of(NegativeOperatorMatcher)
  end
end
