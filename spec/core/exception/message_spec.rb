require File.dirname(__FILE__) + '/../../spec_helper'

describe "Exception#message" do
  it "returns the exception message" do
    [Exception.new.message, Exception.new("Ouch!").message].should == ["Exception", "Ouch!"]
  end  
end
