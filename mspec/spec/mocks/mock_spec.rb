# This is a bit awkward. Currently the way to verify that the
# opposites are true (for example a failure when the specified
# arguments are NOT provided) is to simply alter the particular
# spec to a failure condition.
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../mocks/mock'
require File.dirname(__FILE__) + '/../../mocks/proxy'

describe Mock do
  after :each do
    Mock.cleanup
  end
  
  it "provides expects that returns a Hash" do
    Mock.expects.should be_an_instance_of(Hash)
  end
  
  it "provides set_expect that returns a MockProxy" do
    Mock.install_method(self, :foo).should be_an_instance_of(MockProxy)
  end
end

describe Mock, ".replaced_name" do
  it "returns the name for a method that is being replaced by a mock method" do
    Mock.replaced_name(:method_call).should == :__ms_method_call__
  end
end

describe Mock, ".install_method" do
  before :each do
    @mock = Object.new
  end
  
  after :each do
    Mock.cleanup
  end
  
  it "returns a MockProxy instance" do
    Mock.install_method(@mock, :method_call).should be_an_instance_of(MockProxy)
  end
end

describe Mock, ".verify_call" do
  before :each do
    @mock = Object.new
    @proxy = Mock.install_method @mock, :method_call
  end
  
  after :each do
    Mock.cleanup
  end
  
  it "does not raise an exception when the mock method receives the expected arguments" do
    @proxy.with(1, 'two', :three)
    Mock.verify_call @mock, :method_call, 1, 'two', :three
  end
  
  it "raises ExpectationNotMetError when the mock method does not receive the expected arguments" do
    @proxy.with(4, 2)
    lambda {
      Mock.verify_call @mock, :method_call, 42
    }.should raise_error(ExpectationNotMetError)
  end
  
  it "raises ExpectationNotMetError when the mock method is called with arguments but expects none" do
    lambda {
      Mock.verify_call @mock, :method_call, "hello"
    }.should raise_error(ExpectationNotMetError)
  end
  
  it "raises ExpectationNotMetError when the mock method is called with no arguments but expects some" do
    @proxy.with("hello", "beautiful", "world")
    lambda {
      Mock.verify_call @mock, :method_call
    }.should raise_error(ExpectationNotMetError)
  end
end

describe Mock, ".verify_count" do
  before :each do
    @mock = Object.new
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
  
  it "raises ExpectationNotMetError when the mock receives less than at least the expected number of calls" do
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
  
  it "raises ExpectationNotMetError when the mock receives more than at most the expected number of calls" do
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
  
  it "raises ExpectationNotMetError when the mock receives less than exactly the expected number of calls" do
    @proxy.exactly(2)
    @mock.method_call
    lambda { Mock.verify_count }.should raise_error(ExpectationNotMetError)
  end
  
  it "raises ExpectationNotMetError when the mock receives more than exactly the expected number of calls" do
    @proxy.exactly(2)
    @mock.method_call
    @mock.method_call
    @mock.method_call
    lambda { Mock.verify_count }.should raise_error(ExpectationNotMetError)
  end
end

describe Mock, ".cleanup" do
  before :each do
    @mock = Object.new
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
    Mock.expects.should == { [@mock, :method_call] => @proxy }

    Mock.cleanup
    Mock.expects.should == {}
  end
end
