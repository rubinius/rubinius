require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#&" do
  it "returns self bitwise AND other" do
    (256 & 16).should == 0
    (2010 & 5).should == 0
    (65535 & 1).should == 1
    (0xffff & bignum_value + 0xffff_ffff).should == 65535
  end
  
  it "raises a RangeError if passed a Float out of Fixnum range" do
    lambda { 1 & bignum_value.to_f }.should raise_error(RangeError)
    lambda { 1 & -bignum_value.to_f }.should raise_error(RangeError)
  end
  
  it "tries to convert it's argument to an Integer using to_int" do
    (3 & 2.4).should == 2
    
    (obj = mock('2')).should_receive(:to_int).and_return(2)
    (3 & obj).should == 2
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { 3 & obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { 3 & obj }.should raise_error(TypeError)
  end
end
