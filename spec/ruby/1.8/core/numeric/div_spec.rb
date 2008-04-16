require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#div" do  
  it "returns the integer quotient of two Integers" do 
    13.div(4).should == 3
    4.div(13).should == 0
  end
  
  it "returns the integer quotient of an Integer and a Float" do 
    13.div(4.0).should == 3
    4.div(13.0).should == 0
  end
  
  it "returns the integer quotient of a Float and an Integer" do 
    (13.0).div(4).should == 3
    (4.0).div(13).should == 0
  end
  
  it "returns the integer quotient of two Floats" do 
    (13.0).div(4.0).should == 3
    (4.0).div(13.0).should == 0
  end
  
  it "returns the integer quotient of a Bignum and an Integer" do
    bignum_value(0).div(100).should == 92233720368547758
  end
  
  it "raises an ArgumentError if not passed one argument" do
    lambda { 13.div       }.should raise_error(ArgumentError)
    lambda { 13.div(1, 2) }.should raise_error(ArgumentError)
  end
  
  it "raises a ZeroDivisionError if passed 0" do
    lambda { 13.div(0) }.should raise_error(ZeroDivisionError)
  end
  
  it "raises a TypeError if not passed a Numeric type" do
    lambda { 13.div(nil)    }.should raise_error(TypeError)
    lambda { 13.div('test') }.should raise_error(TypeError)
    lambda { 13.div(true)   }.should raise_error(TypeError)   
  end
end
