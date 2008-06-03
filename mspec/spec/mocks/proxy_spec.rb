require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/mocks/proxy'

describe MockProxy, "reporting method" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns the expected number of calls the mock should receive with #count" do
    @proxy.count.should == [:exactly, 0]
  end

  it "returns the expected arguments with #arguments" do
    @proxy.arguments.should == :any_args
  end

  it "returns the expected return value with #returning" do
    @proxy.returning.should == nil
  end
end

describe MockProxy, "#with" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.with(:a).should be_equal(@proxy)
  end

  it "raises an ArgumentError if no arguments are given" do
    lambda { @proxy.with }.should raise_error(ArgumentError)
  end

  it "accepts any number of arguments" do
    @proxy.with(1, 2, 3).should be_an_instance_of(MockProxy)
    @proxy.arguments.should == [1,2,3]
  end
end

describe MockProxy, "#once" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.once.should be_equal(@proxy)
  end

  it "sets the expected calls to 1" do
    @proxy.once
    @proxy.count.should == [:exactly, 1]
  end

  it "accepts no arguments" do
    lambda { @proxy.once(:a) }.should raise_error
  end
end

describe MockProxy, "#twice" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.twice.should be_equal(@proxy)
  end

  it "sets the expected calls to 2" do
    @proxy.twice
    @proxy.count.should == [:exactly, 2]
  end

  it "accepts no arguments" do
    lambda { @proxy.twice(:b) }.should raise_error
  end
end

describe MockProxy, "#exactly" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.exactly(2).should be_equal(@proxy)
  end

  it "sets the expected calls to exactly n" do
    @proxy.exactly(5)
    @proxy.count.should == [:exactly, 5]
  end

  it "does not accept an argument that Integer() cannot convert" do
    lambda { @proxy.exactly('x') }.should raise_error
  end
end

describe MockProxy, "#at_least" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.at_least(3).should be_equal(@proxy)
  end

  it "sets the expected calls to at least n" do
    @proxy.at_least(3)
    @proxy.count.should == [:at_least, 3]
  end

  it "accepts :once :twice" do
    @proxy.at_least(:once)
    @proxy.count.should == [:at_least, 1]
    @proxy.at_least(:twice)
    @proxy.count.should == [:at_least, 2]
  end

  it "does not accept an argument that Integer() cannot convert" do
    lambda { @proxy.at_least('x') }.should raise_error
  end
end

describe MockProxy, "#at_most" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.at_most(2).should be_equal(@proxy)
  end

  it "sets the expected calls to at most n" do
    @proxy.at_most(2)
    @proxy.count.should == [:at_most, 2]
  end

  it "accepts :once, :twice" do
    @proxy.at_most(:once)
    @proxy.count.should == [:at_most, 1]
    @proxy.at_most(:twice)
    @proxy.count.should == [:at_most, 2]
  end

  it "does not accept an argument that Integer() cannot convert" do
    lambda { @proxy.at_most('x') }.should raise_error
  end
end

describe MockProxy, "#any_number_of_times" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.any_number_of_times.should be_equal(@proxy)
  end

  it "sets the expected calls to any number of times" do
    @proxy.any_number_of_times
    @proxy.count.should == [:any_number_of_times, 0]
  end

  it "does not accept an argument" do
    lambda { @proxy.any_number_of_times(2) }.should raise_error
  end
end

describe MockProxy, "#and_return" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns self" do
    @proxy.and_return(false).should be_equal(@proxy)
  end

  it "sets the expected return value" do
    @proxy.and_return(false)
    @proxy.returning.should == false
  end

  it "accepts any number of return values" do
    @proxy.and_return(1, 2, 3)
    @proxy.returning.should == 1
    @proxy.returning.should == 2
    @proxy.returning.should == 3
  end
  
  it "implicitly sets the expected number of calls" do
    @proxy.and_return(1, 2, 3)
    @proxy.count.should == [:exactly, 3]
  end
  
  it "it only sets the expected number of calls if it is higher than what is already set" do
    @proxy.at_least(5).times.and_return(1, 2, 3)
    @proxy.count.should == [:at_least, 5]

    @proxy.at_least(2).times.and_return(1, 2, 3)
    @proxy.count.should == [:at_least, 3]
  end
end

describe MockProxy, "#calls" do
  before :each do
    @proxy = MockProxy.new
  end

  it "returns the number of times the proxy is called" do
    @proxy.calls.should == 0
  end
end

describe MockProxy, "#called" do
  before :each do
    @proxy = MockProxy.new
  end

  it "increments the number of times the proxy is called" do
    @proxy.called
    @proxy.called
    @proxy.calls.should == 2
  end
end

describe MockProxy, "#returning" do
  before :each do
    @proxy = MockProxy.new
  end

  it "should return nil by default" do
    @proxy.returning.should be_nil
  end

  it "should return the value set by #and_return" do
    @proxy.and_return(2)
    @proxy.returning.should == 2
  end

  it "should return a sequence of values set by #and_return" do
    @proxy.and_return(1,2,3,4)
    @proxy.returning.should == 1
    @proxy.returning.should == 2
    @proxy.returning.should == 3
    @proxy.returning.should == 4
    @proxy.returning.should == 4
    @proxy.returning.should == 4
  end
end

describe MockProxy, "#times" do
  before :each do
    @proxy = MockProxy.new
  end

  it "is a no-op" do
    @proxy.times.should == @proxy
  end
end
