require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#abort_on_exception" do
  it "should be false unless $DEBUG is true" do
    Thread.abort_on_exception.should == false unless $DEBUG
  end

  it "should be true if $DEBUG is true" do
    Thread.abort_on_exception.should == true if $DEBUG
  end
  
  it "should be changeable to true or false" do
    Thread.abort_on_exception = true
    Thread.abort_on_exception.should == true
    Thread.abort_on_exception = false
    Thread.abort_on_exception.should == false
  end
  
end