require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#suffix?" do
  it "returns true if string ends with the given string" do
    "blah".suffix?("ah").should == true
    "blah".suffix?("fo").should == false
    "go".suffix?("gogo").should == false
    "go".suffix?("go").should == true
  end
end
