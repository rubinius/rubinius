require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#==" do
  before(:each) do
    @bignum = BignumHelper.sbm(67)
    @bignum_float = BignumHelper.sbm(67.7)
  end
  
  it "should return true if self has the same value as the given argument" do
    (@bignum == @bignum).should == true
    (@bignum_float == @bignum_float).should == true
    
    (@bignum == @bignum_float).should == false
    (@bignum_float == @bignum).should == false
    
    (@bignum_float == (@bignum_float + 0.01)).should == false
    (@bignum == 9).should == false
    (@bignum == 9.01).should == false
    
    (@bignum == 0xffffffff).should == false
  end

  it "calls 'other == self' if the given argument is not an Integer" do
    obj = mock('not integer')
    obj.should_receive(:==).any_number_of_times.and_return(false)
    @bignum.should_not == obj
    
    obj = mock('wannabe float')
    obj.should_receive(:==).and_return(true)
    @bignum_float.should == obj
  end
end
