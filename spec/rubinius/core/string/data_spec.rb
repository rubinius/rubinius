require File.dirname(__FILE__) + '/../../../spec_helper'

describe "String#data" do
  it "returns the size needed for storing the string (space for last \0 and multiple of 4)" do
    "hell".data.size.should == 8
  end
end
