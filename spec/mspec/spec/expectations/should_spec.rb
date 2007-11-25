require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'

class Object; alias_method :rspec_should, :should; end
require File.dirname(__FILE__) + '/../../expectations/should'
class Object; alias_method :mspec_should, :should; end

# Adapted from RSpec 1.0.8
describe Object, "#should" do
  before :all do
    class Object; alias_method :should, :mspec_should; end
  end
  
  after :all do
    class Object; alias_method :should, :rspec_should; end
  end
  
  before :each do
    @target = "target"
    @matcher = mock("matcher")
    @matcher.stub!(:matches?).and_return(true)
    @matcher.stub!(:failure_message)
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

  it "raises ExpectationNotMetError when matcher #matches? returns false" do
    @matcher.should_receive(:matches?).with(@target).and_return(false)
    @matcher.should_receive(:failure_message).and_return(["expected", "actual"])
    lambda {
      @target.should @matcher
    }.should raise_error(ExpectationNotMetError, "expected actual")
  end
end

describe Object, "#should_not" do
  before :all do
    class Object; alias_method :should, :mspec_should; end
  end
  
  after :all do
    class Object; alias_method :should, :rspec_should; end
  end
  
  before :each do
    @target = "target"
    @matcher = mock("matcher")
  end
  
  it "accepts and interacts with a matcher" do
    @matcher.should_receive(:matches?).with(@target).and_return(false)
    @matcher.stub!(:negative_failure_message).and_return(["expected", "actual"])
    @target.should_not @matcher
  end
  
  it "calls #negative_failure_message when matcher.matches? returns true" do
    @matcher.should_receive(:matches?).with(@target).and_return(true)
    @matcher.stub!(:negative_failure_message).and_return(["expected", "actual"])
    @target.should @matcher rescue nil
  end
  
  it "raises ExpectationNotMetError when matcher.matches? returns true" do
    @matcher.should_receive(:matches?).with(@target).and_return(true)
    @matcher.should_receive(:negative_failure_message).and_return(["expected", "actual"])
    lambda {
      @target.should_not @matcher
    }.should raise_error(ExpectationNotMetError, "expected actual")
  end
end
