require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#<<" do
  it "returns self shifted left other bits" do
    a = BignumHelper.sbm(9)
    (a << 4).should == 17179869328
    (a << 1.5).should == 2147483666
    (a << 9).should == 549755818496
  end
end
