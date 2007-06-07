require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#capitalize" do
  it "should return a copy of self with the first character converted to uppercase and the remainder to lowercase" do
    "hello".capitalize.should == "Hello"
    "HELLO".capitalize.should == "Hello"
    "123ABC".capitalize.should == "123abc"
  end
end

describe "String#capitalize!" do
  it "should capitalize self in place" do
    a = "hello"
    a.capitalize!.should == "Hello"
    a.should == "Hello"
  end
  
  it "should return nil when no changes are made" do
    a = "Hello"
    a.capitalize!.should == nil
    a.should == "Hello"
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "Hello"
      a.freeze
      a.capitalize!
    end
  end
end