require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#>" do
  it "returns true if self is greater than other" do
    a = BignumHelper.sbm(732)
    (a > (a + 500)).should == false
    (a > 14.6).should == true
    (a > (BignumHelper.sbm - 1)).should == true
  end
end
