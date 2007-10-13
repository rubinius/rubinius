require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#<=>" do
  before(:each) do
    @bignum = BignumHelper.sbm(96)
  end
  
  it "returns -1 when self is less than the given argument" do
    (-@bignum <=> @bignum).should == -1
    (-@bignum <=> -1).should == -1
    (-@bignum <=> -4.5).should == -1
  end
  
  it "returns 0 when self is equal to the given argument" do
    (@bignum <=> @bignum).should == 0
    (-@bignum <=> -@bignum).should == 0
  end
  
  it "returns 1 when self is greater than the given argument" do
    (@bignum <=> -@bignum).should == 1
    (@bignum <=> 1).should == 1
    (@bignum <=> 4.5).should == 1
  end

  it "returns nil when the given argument is no Integer" do
    (@bignum <=> Object.new).should == nil
    (@bignum <=> 'test').should == nil
  end
end
