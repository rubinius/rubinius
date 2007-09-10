require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#^" do
  it "returns self bitwise EXCLUSIVE OR other" do
    a = BignumHelper.sbm(18)
    (a ^ 2).should == 1073741840
    (a ^ a).should == 0
    (a ^ 14.5).should == 1073741852
  end
end
