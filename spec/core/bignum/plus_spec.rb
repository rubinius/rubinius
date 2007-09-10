require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#+" do
  it "returns self plus other" do
    a = BignumHelper.sbm(76)
    (a + 4).should == 1073741904
    (a + 4.2).should == 1073741904.2
    (a + BignumHelper.sbm(3)).should == 2147483727
  end
end
