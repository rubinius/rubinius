require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#upcase" do
  it "should return a copy of self with all lowercase letters upcase'd" do
    "Hello".upcase.should == "HELLO"
    "HELLO".upcase.should == "HELLO"
  end
  
  it "should be locale insensitive (only replacing A-Z)" do
    "äöü".upcase.should == "äöü"
  end
end

describe "String#upcase!" do
  it "should modify self in place" do
    a = "HeLlO"
    a.upcase!.should == "HELLO"
    a.should == "HELLO"
  end
  
  it "should return nil if no modifications were made" do
    a = "HELLO"
    a.upcase!.should == nil
    a.should == "HELLO"
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "HeLlO"
      a.freeze
      a.upcase!
    end
  end
end