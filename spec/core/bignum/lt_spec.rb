require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#<" do
  before(:each) do
    @bignum = BignumHelper.sbm(32)
  end
  
  it "returns true if self is less than the given argument" do
    (@bignum < @bignum + 1).should == true
    (-@bignum < -(@bignum - 1)).should == true
    
    (@bignum < 1).should == false
    (@bignum < 5).should == false
    
    (@bignum < @bignum + 0.5).should == true
    (@bignum < 4.999).should == false
  end
  
  it "raises an ArgumentError when given a non-Integer" do
    should_raise(ArgumentError) do
      @bignum < "4"
    end
    
    should_raise(ArgumentError) do
      @bignum < Object.new
    end
  end
end
