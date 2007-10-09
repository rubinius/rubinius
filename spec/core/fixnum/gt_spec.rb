require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#>" do
  it "returns true if self is greater than the given argument" do
    (13 > 2).should == true
    (-500 > -600).should == true
    
    (1 > 5).should == false
    (5 > 5).should == false
    
    (900 > 0xffffffff).should == false
    (5 > 4.999).should == true
  end
  
  it "raises an ArgumentError when given a non-Integer" do
    should_raise(ArgumentError) do
      5 > "4"
    end
    
    should_raise(ArgumentError) do
      5 > Object.new
    end
  end
end
