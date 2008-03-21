require File.dirname(__FILE__) + '/../../spec_helper'

describe "String.template" do
  it "returns a string of specified length initialized to a character" do
    String.template(10, 97).should == "aaaaaaaaaa"
  end

  it "returns a string of specified length initialized to a string" do
    String.template(10, "b").should == "bbbbbbbbbb"
    String.template(10, " ").should == "          "
    String.template(10, "abc").should == "abcabcabca"
  end
end
