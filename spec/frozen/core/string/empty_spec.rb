require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#empty?" do
  it "returns true if the string has a length of zero" do
    "hello".empty?.should == false
    " ".empty?.should == false
    "\x00".empty?.should == false
    "".empty?.should == true
    StringSpecs::MyString.new("").empty?.should == true
  end
end
