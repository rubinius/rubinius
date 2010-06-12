require File.expand_path('../../../spec_helper', __FILE__)

describe "String#prefix?" do
  it "returns true if string begins with argument" do
    "blah".prefix?("bl").should == true
    "blah".prefix?("fo").should == false
    "go".prefix?("gogo").should == false
  end  
end
