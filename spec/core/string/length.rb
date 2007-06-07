require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#length" do
  it "should return the length of self" do
    "".length.should == 0
    "one".length.should == 3
    "two".length.should == 3
    "three".length.should == 5
    "four".length.should == 4
  end
end

describe "String#size" do
  it "should be an alias of String#length" do
    # TODO:
  end
end