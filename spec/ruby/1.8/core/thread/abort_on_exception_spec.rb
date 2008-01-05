require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#abort_on_exception" do
  it "is false unless $DEBUG is true" do
    Thread.abort_on_exception.should == false unless $DEBUG
  end

  it "is true if $DEBUG is true" do
    Thread.abort_on_exception.should == true if $DEBUG
  end
  
  it "is changeable to true or false" do
    Thread.abort_on_exception = true
    Thread.abort_on_exception.should == true
    Thread.abort_on_exception = false
    Thread.abort_on_exception.should == false
  end
  
end