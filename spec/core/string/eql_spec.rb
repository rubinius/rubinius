require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#eql?" do
  it "returns true if two strings have the same length and content" do
    "hello".eql?("hello").should == true
    "hello".eql?("hell").should == false
    "1".eql?(1).should == false
    
    MyString.new("hello").eql?("hello").should == true
    "hello".eql?(MyString.new("hello")).should == true
    MyString.new("hello").eql?(MyString.new("hello")).should == true
  end
end
