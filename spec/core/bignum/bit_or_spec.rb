require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#|" do
  it "returns self bitwise OR other" do
    a = BignumHelper.sbm(11)
    (a | 2).should == 1073741835
    (a | 9.9).should == 1073741835
    (a | BignumHelper.sbm).should == 1073741835
  end
end
