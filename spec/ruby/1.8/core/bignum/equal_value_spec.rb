require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#==" do
  before(:each) do
    @bignum = bignum_value
  end
  
  it "returns true if self has the same value as the given argument" do
    (@bignum == @bignum).should == true
    (@bignum == @bignum + 1).should == false
    (@bignum + 1 == @bignum).should == false
    
    (@bignum == 9).should == false
    (@bignum == 9.01).should == false
    
    (@bignum == bignum_value(10)).should == false
  end

  it "calls 'other == self' if the given argument is not an Integer" do
    obj = mock('not integer')
    obj.should_receive(:==).and_return(false)
    @bignum.should_not == obj
  end
end
