require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#-" do
  before(:each) do
    @bignum = bignum_value(314)
  end
  
  it "returns self minus the given Integer" do
    (@bignum - 9).should == 9223372036854776113
    (@bignum - 12.57).to_s.should == "9.22337203685478e+18"
    (@bignum - bignum_value(42)).should == 272
  end

  it "raises a TypeError when given a non-Integer" do
    lambda { @bignum - mock('10') }.should raise_error(TypeError)
    lambda { @bignum - "10" }.should raise_error(TypeError)
    lambda { @bignum - :symbol }.should raise_error(TypeError)
  end
end