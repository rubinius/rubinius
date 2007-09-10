require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#to_s" do
  it "returns a string representation of self, possibly Nan, -Infinity, +Infinity" do
    0.551e7.to_s.should == "5510000.0"
    -3.14159.to_s.should == "-3.14159"
    0.0.to_s.should == "0.0"
  end  
end
