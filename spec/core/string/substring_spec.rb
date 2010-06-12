require File.expand_path('../../../spec_helper', __FILE__)

describe "String#substring" do
  it "returns the portion of string specified by index, length" do
    "blah".substring(0, 2).should == "bl"
    "blah".substring(0, 4).should == "blah"
    "blah".substring(2, 2).should == "ah"
    "blah".substring(1, 2).should == "la"
  end

  it "counts negative indexes from the end" do
    "halb".substring(-2, 2).should == "lb"
  end

  it "returns nil if index is out of the bounds of the string" do
    "halb".substring(-5, 2).should == nil
    "halb".substring(5, 2).should == nil
  end

  it "returns an empty string if index is equal to the size of the string" do
    "halb".substring(4, 2).should == ""
  end

  it "constrains length to the size of the string" do
    "halb".substring(0, 5).should == "halb"
    "halb".substring(3, 2).should == "b"
  end
end
