require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#dup" do
  it "does not raise a NoMethodError when #send is undefined in the object" do
    c = Class.new do
      undef_method :send
    end
    lambda { c.new.dup }.should_not raise_error(NoMethodError)
  end
end
