require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#<<" do
  before(:each) do
    @bignum = bignum_value(9)
  end
  
  it "returns self shifted the given amount of bits to the left" do
    (@bignum << 4).should == 147573952589676413072
    (@bignum << 9).should == 4722366482869645218304
  end

  it "performs a right-shift if given a negative value" do
    (@bignum << -2).should == (@bignum >> 2)
    (@bignum << -4).should == (@bignum >> 4)
  end

  it "tries to convert its argument to an Integer using to_int" do
    (@bignum << 4.5).should == 147573952589676413072
    
    (obj = mock('4')).should_receive(:to_int).and_return(4)
    (@bignum << obj).should == 147573952589676413072
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum << obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum << obj }.should raise_error(TypeError)
  end
  
  it "raises a RangeError when the given argument is out of range of Integer" do
    (obj = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
    lambda { @bignum << obj }.should raise_error(RangeError)

    obj = 8e19
    lambda { @bignum << obj }.should raise_error(RangeError)
  end
end
