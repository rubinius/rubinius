require File.expand_path('../../../spec_helper', __FILE__)

describe "Exception#message" do
  it "returns the exception message" do
    [Exception.new.message, Exception.new("Ouch!").message].should == ["Exception", "Ouch!"]
  end  
end
