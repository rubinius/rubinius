require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#abs" do
  it "returns the absolute value" do
    bignum_value(39).abs.should == 9223372036854775847
    (-bignum_value(18)).abs.should == 9223372036854775826
  end
end
