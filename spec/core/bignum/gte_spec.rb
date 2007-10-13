require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#>=" do
  before(:each) do
    @bignum = BignumHelper.sbm(14)
  end
  
  it "returns true if self is greater than or equal to other" do
    (@bignum >= @bignum).should == true
    (@bignum >= (@bignum + 2)).should == false
    (@bignum >= 5664.2).should == true
    (@bignum >= 4).should == true
  end

  it "raises an ArgumentError when given a non-Integer" do
    should_raise(ArgumentError) do
      @bignum >= "4"
    end
    
    should_raise(ArgumentError) do
      @bignum >= Object.new
    end
  end
end
