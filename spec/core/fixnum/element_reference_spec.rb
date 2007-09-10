require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#[]" do
  it "returns the nth bit in the binary representation of self" do
    2[3].should == 0
    15[1].should == 1
  end
  
  it "coerces the bit and return the nth bit in the binary representation of self" do
    2[3].should == 0
    15[1.3].should == 1
    3[0xffffffff].should == 0
  end
end
