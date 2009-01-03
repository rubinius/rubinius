require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#>>" do
  before(:each) do 
    @bignum = bignum_value(90812)
  end

  it "returns self shifted the given amount of bits to the right" do
    (@bignum >> 1).should == 4611686018427433310
    (@bignum >> 3).should == 1152921504606858327
  end

  it "performs a left-shift if given a negative value" do
    (@bignum >> -1).should == (@bignum << 1)
    (@bignum >> -3).should == (@bignum << 3)
  end
  
  it "tries to convert the given argument to an Integer using to_int" do
    (@bignum >> 1.3).should == 4611686018427433310
    
    (obj = mock('1')).should_receive(:to_int).and_return(1)
    (@bignum >> obj).should == 4611686018427433310
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum >> obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum >> obj }.should raise_error(TypeError)
  end

  it "returns 0 when the given argument is a Bignum and self is positive" do
    (@bignum >> bignum_value).should == 0
    
    obj = mock("Converted to Integer")
    obj.should_receive(:to_int).and_return(bignum_value)
    (@bignum >> obj).should == 0
  end
  
  it "returns -1 when the given argument is a Bignum and self is negative" do
    (-@bignum >> bignum_value).should == -1

    obj = mock("Converted to Integer")
    obj.should_receive(:to_int).and_return(bignum_value)
    (-@bignum >> obj).should == -1
  end
end