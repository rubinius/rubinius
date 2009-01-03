require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#&" do
  before(:each) do
    @bignum = bignum_value(5)
  end
  
  it "returns self bitwise AND other" do
    @bignum = bignum_value(5)
    (@bignum & 3).should == 1
    (@bignum & 52).should == 4
    (@bignum & bignum_value(9921)).should == 9223372036854775809

    ((2*bignum_value) & 1).should == 0
    ((2*bignum_value) & -1).should == 18446744073709551616
    ((4*bignum_value) & -1).should == 36893488147419103232
    ((2*bignum_value) & (2*bignum_value)).should == 18446744073709551616
    (bignum_value & bignum_value(0xffff).to_f).should == 9223372036854775808
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (@bignum & 3.4).should == 1
    
    (obj = mock('3')).should_receive(:to_int).and_return(3)
    (@bignum & obj).should == 1
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum & obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum & obj }.should raise_error(TypeError)
  end
end
