require File.expand_path('../../../spec_helper', __FILE__)

describe "String#data" do
  it "returns the size needed for storing the string (space for last \0 and multiple of 4)" do
    "hell".data.size.should == 8
  end
end
