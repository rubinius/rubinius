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

  it "calls 'other == self' if the given argument is no Integer" do
    (obj = Object.new).should_receive(:==, :returning => false)    
    (@bignum == obj).should == false
    
    obj.should_receive(:==, :returning => true)
    (@bignum_float == obj).should == true
  end
end
