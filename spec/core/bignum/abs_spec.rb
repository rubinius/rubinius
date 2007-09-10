require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#abs" do
  it "returns the absolute value" do
    BignumHelper.sbm(39).abs.should == 1073741863
    (-BignumHelper.sbm(18)).abs.should == 1073741842
  end
end
