require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#<=" do
  it "returns true if self is less than or equal to other" do
    (2 <= 13).should == true
    (-600 <= -500).should == true
    
    (5 <= 1).should == false
    (5 <= 5).should == true
    (-2 <= -2).should == true
    
    (900 <= 0xffffffff).should == true
    (5 <= 4.999).should == false
  end

  it "raises an ArgumentError when given a non-Integer" do
    should_raise(ArgumentError) do
      5 <= "4"
    end
    
    should_raise(ArgumentError) do
      5 <= Object.new
    end
  end
end
