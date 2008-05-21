# This is a bit awkward. Currently the way to verify that the
# opposites are true (for example a failure when the specified
# arguments are NOT provided) is to simply alter the particular
# spec to a failure condition.
require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/runner/mspec'
require 'mspec/mocks/mock'
require 'mspec/mocks/proxy'

describe Mock do
  after :each do
    Mock.cleanup
  end

  it "provides expects that returns a Hash" do
    Mock.expects.should be_an_instance_of(Hash)
  end
end

describe Mock, ".replaced_name" do
  it "returns the name for a method that is being replaced by a mock method" do
    Mock.replaced_name(:method_call).should == :__ms_method_call__
  end
end

describe Mock, ".install_method" do
  before :each do
    @mock = mock('install_method')
    MSpec.stub!(:actions)
    MSpec.stub!(:current).and_return(mock("spec state", :null_object => true))
  end

  after :each do
    Mock.cleanup
  end

  it "returns a MockProxy instance" do
    Mock.install_method(@mock, :method_call).should be_an_instance_of(MockProxy)
  end

  it "sets the proxy to expect exactly 1 call" do
    proxy = Mock.install_method(@mock, :method_call)
    proxy.count.should == [:exactly, 1]
  end

  it "does not override a previously mocked method with the same name" do
    Mock.install_method(@mock, :method_call).with(:a, :b).and_return(1)
    Mock.install_method(@mock, :method_call).with(:c).and_return(2)
    @mock.method_call(:a, :b)
    @mock.method_call(:c)
    lambda { @mock.method_call(:d) }.should raise_error(ExpectationNotMetError)
  end

  it "properly sends #respond_to? calls to the aliased respond_to? method when not matching mock expectations" do
    Mock.install_method(@mock, :respond_to?).with(:to_str).and_return('mock to_str')
    Mock.install_method(@mock, :respond_to?).with(:to_int).and_return('mock to_int')
    @mock.respond_to?(:to_str).should == 'mock to_str'
    @mock.respond_to?(:to_int).should == 'mock to_int'
    @mock.respond_to?(:to_s).should == true
    @mock.respond_to?(:not_really_a_real_method_seriously).should == false
  end

  it "adds to the expectation tally" do
    state = mock("run state", :null_object => true)
    state.stub!(:state).and_return(mock("spec state"))
    MSpec.should_receive(:current).and_return(state)
    MSpec.should_receive(:actions).with(:expectation, state.state)
    Mock.install_method(@mock, :method_call).and_return(1)
  end
end

describe Mock, ".verify_call" do
  before :each do
    MSpec.stub!(:actions)
    MSpec.stub!(:current).and_return(mock("spec state", :null_object => true))

    @mock = mock('verify_call')
    @proxy = Mock.install_method @mock, :method_call
  end

  after :each do
    ScratchPad.clear
    Mock.cleanup
  end

  it "does not raise an exception when the mock method receives the expected arguments" do
    @proxy.with(1, 'two', :three)
    Mock.verify_call @mock, :method_call, 1, 'two', :three
  end

  it "raises an ExpectationNotMetError when the mock method does not receive the expected arguments" do
    @proxy.with(4, 2)
    lambda {
      Mock.verify_call @mock, :method_call, 42
    }.should raise_error(ExpectationNotMetError)
  end

  it "raises an ExpectationNotMetError when the mock method is called with arguments but expects none" do
    lambda {
      @proxy.with(:no_args)
      Mock.verify_call @mock, :method_call, "hello"
    }.should raise_error(ExpectationNotMetError)
  end

  it "raises an ExpectationNotMetError when the mock method is called with no arguments but expects some" do
    @proxy.with("hello", "beautiful", "world")
    lambda {
      Mock.verify_call @mock, :method_call
    }.should raise_error(ExpectationNotMetError)
  end

  it "does not raise an exception when the mock method is called with arguments and is expecting :any_args" do
    @proxy.with(:any_args)
    Mock.verify_call @mock, :method_call, 1, 2, 3
  end
  
  it "yields a passed block when it is expected to" do
    @proxy.and_yield()
    Mock.verify_call @mock, :method_call do 
      ScratchPad.record true
    end
    ScratchPad.recorded.should == true
  end
  
  it "does not yield a passed block when it is not expected to" do
    Mock.verify_call @mock, :method_call do 
      ScratchPad.record true
    end
    ScratchPad.recorded.should == nil
  end
  
  it "can yield subsequently" do
    @proxy.and_yield(1).and_yield(2).and_yield(3)
    
    ScratchPad.record []
    Mock.verify_call @mock, :method_call do |arg|
      ScratchPad << arg
    end
    ScratchPad.recorded.should == [1, 2, 3]
  end

  it "can yield and return an expected value" do
    @proxy.and_yield(1).and_return(3)
    
    Mock.verify_call(@mock, :method_call) { |arg| ScratchPad.record arg }.should == 3
    ScratchPad.recorded.should == 1
  end

  it "raises an expection when it is expected to yield but no block is given" do
    @proxy.and_yield(1, 2, 3)
    lambda {
      Mock.verify_call(@mock, :method_call)
    }.should raise_error(ExpectationNotMetError)
  end

  it "raises an expection when it is expected to yield more arguments than the block can take" do
    @proxy.and_yield(1, 2, 3)
    lambda {
      Mock.verify_call(@mock, :method_call) {|a, b|}
    }.should raise_error(ExpectationNotMetError)
  end

  it "does not raise an expection when it is expected to yield to a block that can take any number of arguments" do
    @proxy.and_yield(1, 2, 3)
    lambda {
      Mock.verify_call(@mock, :method_call) {|*a|}
    }.should_not raise_error(ExpectationNotMetError)
  end
end

describe Mock, ".verify_count" do
  before :each do
    MSpec.stub!(:actions)
    MSpec.stub!(:current).and_return(mock("spec state", :null_object => true))

    @mock = mock('verify_count')
    @proxy = Mock.install_method @mock, :method_call
  end

  after :each do
    Mock.cleanup
  end

  it "does not raise an exception when the mock receives at least the expected number of calls" do
    @proxy.at_least(2)
    @mock.method_call
    @mock.method_call
    Mock.verify_count
  end

  it "raises an ExpectationNotMetError when the mock receives less than at least the expected number of calls" do
    @proxy.at_least(2)
    @mock.method_call
    lambda { Mock.verify_count }.should raise_error(ExpectationNotMetError)
  end

  it "does not raise an exception when the mock receives at most the expected number of calls" do
    @proxy.at_most(2)
    @mock.method_call
    @mock.method_call
    Mock.verify_count
  end

  it "raises an ExpectationNotMetError when the mock receives more than at most the expected number of calls" do
    @proxy.at_most(2)
    @mock.method_call
    @mock.method_call
    @mock.method_call
    lambda { Mock.verify_count }.should raise_error(ExpectationNotMetError)
  end

  it "does not raise an exception when the mock receives exactly the expected number of calls" do
    @proxy.exactly(2)
    @mock.method_call
    @mock.method_call
    Mock.verify_count
  end

  it "raises an ExpectationNotMetError when the mock receives less than exactly the expected number of calls" do
    @proxy.exactly(2)
    @mock.method_call
    lambda { Mock.verify_count }.should raise_error(ExpectationNotMetError)
  end

  it "raises an ExpectationNotMetError when the mock receives more than exactly the expected number of calls" do
    @proxy.exactly(2)
    @mock.method_call
    @mock.method_call
    @mock.method_call
    lambda { Mock.verify_count }.should raise_error(ExpectationNotMetError)
  end
end

describe Mock, ".cleanup" do
  before :each do
    MSpec.stub!(:actions)
    MSpec.stub!(:current).and_return(mock("spec state", :null_object => true))

    @mock = mock('cleanup')
    @proxy = Mock.install_method @mock, :method_call
  end

  after :each do
    Mock.cleanup
  end

  it "removes the mock method call if it did not override an existing method" do
    @mock.should respond_to(:method_call)

    Mock.cleanup
    @mock.should_not respond_to(:method_call)
  end

  it "removes the replaced method if the mock method overrides an existing method" do
    def @mock.already_here() :hey end
    @mock.should respond_to(:already_here)
    Mock.install_method @mock, :already_here
    @mock.should respond_to(Mock.replaced_name(:already_here))

    Mock.cleanup
    @mock.should_not respond_to(Mock.replaced_name(:already_here))
    @mock.should respond_to(:already_here)
    @mock.already_here.should == :hey
  end

  it "removes all mock expectations" do
    Mock.expects.should == { [@mock, :method_call] => [@proxy] }

    Mock.cleanup
    Mock.expects.should == {}
  end
end
