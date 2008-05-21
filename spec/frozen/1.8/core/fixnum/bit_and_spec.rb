require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#&" do
  it "returns self bitwise AND other" do
    (256 & 16).should == 0
    (2010 & 5).should == 0
    (65535 & 1).should == 1
    (0xffff & bignum_value + 0xffff_ffff).should == 65535
  end
  
  it "should be able to AND a bignum with a fixnum" do
    (-1 & 2**64).should == 18446744073709551616
  end
  
  it "raises a RangeError if passed a Float out of Fixnum range" do
    lambda { 1 & bignum_value(10000).to_f }.should raise_error(RangeError)
    lambda { 1 & -bignum_value(10000).to_f }.should raise_error(RangeError)
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

  ruby_bug "#", "1.8.6" do # Fixed at MRI 1.8.7
    it "coerces arguments correctly even if it is a Bignum" do
      (obj = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
      (3 & obj).should == 0
    end
  end
end
