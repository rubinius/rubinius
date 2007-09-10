require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#<" do
  it "returns true if self is less than other" do
    a = BignumHelper.sbm(32)
    (a < 2).should == false
    (a < 456_912.410).should == false
    (a < BignumHelper.sbm(42)).should == true
  end
end
