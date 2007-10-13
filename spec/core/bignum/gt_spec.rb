require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#>" do
  before(:each) do
    @bignum = BignumHelper.sbm(732)
  end
  
  it "returns true if self is greater than the given argument" do
    (@bignum > (@bignum - 1)).should == true
    (@bignum > 14.6).should == true
    (@bignum > 10).should == true

    (@bignum > (@bignum + 500)).should == false
  end
  
  it "raises an ArgumentError when given a non-Integer" do
    should_raise(ArgumentError) do
      @bignum > "4"
    end
    
    should_raise(ArgumentError) do
      @bignum > Object.new
    end
  end
end
