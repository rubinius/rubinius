require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#-" do
  it "returns self minus other" do
    a = BignumHelper.sbm(314)
    (a - 9).should == 1073742129
    (a - 12.57).should == 1073742125.43
    (a - BignumHelper.sbm(42)).should == 272
  end
end
