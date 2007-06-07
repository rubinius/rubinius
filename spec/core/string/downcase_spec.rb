require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#downcase" do
  it "should return a copy of self with all uppercase letters downcase'd" do
    "hELLO".downcase.should == "hello"
    "hello".downcase.should == "hello"
  end
  
  it "should be locale insensitive (only replacing A-Z)" do
    "ÄÖÜ".downcase.should == "ÄÖÜ"
  end
end

describe "String#downcase!" do
  it "should modify self in place" do
    a = "HeLlO"
    a.downcase!.should == "hello"
    a.should == "hello"
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.downcase!.should == nil
    a.should == "hello"
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "HeLlO"
      a.freeze
      a.downcase!
    end
  end
end