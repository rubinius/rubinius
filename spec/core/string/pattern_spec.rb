require File.expand_path('../../../spec_helper', __FILE__)

describe "String.pattern" do
  it "returns a string of specified length initialized to a character" do
    String.pattern(10, 97).should == "aaaaaaaaaa"
  end

  it "returns a string of specified length initialized to a string" do
    String.pattern(10, "b").should == "bbbbbbbbbb"
    String.pattern(10, " ").should == "          "
    String.pattern(10, "abc").should == "abcabcabca"
  end
end
