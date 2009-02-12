require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/base'
require 'mspec/runner/mspec'

class Object
  alias_method :rspec_should,     :should
  alias_method :rspec_should_not, :should_not
end
require 'mspec/expectations/should'
class Object
  alias_method :mspec_should,     :should
  alias_method :mspec_should_not, :should_not
  alias_method :should,           :rspec_should
  alias_method :should_not,       :rspec_should_not
end

# Adapted from RSpec 1.0.8
describe Object, "#should" do
  before :each do
    class Object; alias_method :should, :mspec_should; end

    @state = mock("example state", :null_object => true)
    context = mock("context state", :null_object => true)
    context.stub!(:state).and_return(@state)
    MSpec.stub!(:current).and_return(context)
    MSpec.stub!(:actions)

    @target = "target"
    @matcher = mock("matcher", :null_object => true)
  end

  after :each do
    class Object; alias_method :should, :rspec_should; end
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
    class Object; alias_method :should, :rspec_should; end
    matcher.should be_instance_of(PositiveOperatorMatcher)
  end

  it "invokes the MSpec :expectation actions" do
    MSpec.should_receive(:actions).with(:expectation, @state)
    @target.should @matcher
  end

  it "registers that an expectation has been encountered" do
    MSpec.should_receive(:expectation)
    @target.should @matcher
  end
end

describe Object, "#should_not" do
  before :each do
    class Object; alias_method :should,     :mspec_should; end
    class Object; alias_method :should_not, :mspec_should_not; end

    @state = mock("example state", :null_object => true)
    context = mock("context state", :null_object => true)
    context.stub!(:state).and_return(@state)
    MSpec.stub!(:current).and_return(context)
    MSpec.stub!(:actions)

    @target = "target"
    @matcher = mock("matcher", :null_object => true)
  end

  after :each do
    class Object; alias_method :should,     :rspec_should; end
    class Object; alias_method :should_not, :rspec_should_not; end
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
    matcher = should_not nil
    class Object; alias_method :should, :rspec_should; end
    matcher.should be_instance_of(NegativeOperatorMatcher)
  end

  it "invokes the MSpec :expectation actions" do
    MSpec.should_receive(:actions).with(:expectation, @state)
    @matcher.should_receive(:negative_failure_message).and_return(["expected", "actual"])
    @target.should_not @matcher rescue nil
  end

  it "registers that an expectation has been encountered" do
    MSpec.should_receive(:expectation)
    @matcher.should_receive(:negative_failure_message).and_return(["expected", "actual"])
    @target.should_not @matcher rescue nil
  end
end
