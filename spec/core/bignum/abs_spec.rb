require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#abs" do
  it "returns the absolute value" do
    BignumHelper.sbm(39).abs.should == 9223372036854775847
    (-BignumHelper.sbm(18)).abs.should == 9223372036854775826
  end
end
