require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#~" do
  it "returns self bitwise inverted" do
    (~BignumHelper.sbm(48)).should == -1073741873
    (~(-BignumHelper.sbm(21))).should == 1073741844
    (~BignumHelper.sbm(1)).should == -1073741826
  end
end
