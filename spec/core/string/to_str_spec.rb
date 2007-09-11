require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#to_str" do
  it "returns self when self.class == String" do
    a = "a string"
    a.equal?(a.to_str).should == true
  end

  it "returns a new instance of String when called on a subclass" do
    a = MyString.new("a string")
    s = a.to_str
    s.should == "a string"
    s.class.should == String
  end
  
  it "taints the result when self is tainted" do
    "x".taint.to_str.tainted?.should == true
    MyString.new("x").taint.to_str.tainted?.should == true
  end
end
