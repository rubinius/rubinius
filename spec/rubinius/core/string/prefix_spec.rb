require File.dirname(__FILE__) + '/../../../spec_helper'

describe "String#prefix?" do
  it "returns true if string begins with argument" do
    "blah".prefix?("bl").should == true
    "blah".prefix?("fo").should == false
    "go".prefix?("gogo").should == false
  end  
end
