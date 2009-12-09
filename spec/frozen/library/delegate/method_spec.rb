require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + "/fixtures/classes"
require 'delegate'

describe "SimpleDelegator#method" do
  before :each do
    @simple = DelegateSpecs::Simple.new
    @delegate = SimpleDelegator.new(@simple)
  end

  it "returns a method object for a valid method" do
    m = @delegate.method(:pub)
    m.should be_an_instance_of(Method)
    m.call.should == :foo
  end

  it "raises a NameError for an invalid method name" do
    lambda {
      @delegate.method(:invalid_and_silly_method_name)
    }.should raise_error(NameError)
  end

  ruby_version_is "1.9" do
    it "returns a method that respond_to_missing?" do
      m = @delegate.method(:pub_too)
      m.should be_an_instance_of(Method)
      m.call.should == :pub_too
    end
  end

  it "raises a NameError if method is no longer valid because object has changed" do
    m = @delegate.method(:pub)
    @delegate.__setobj__([1,2,3])
    lambda {
      m.call
    }.should raise_error(NameError)
  end
end
