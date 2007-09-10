require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#<=" do
  it "returns true if self is less than or equal to other" do
    a = BignumHelper.sbm(39)
    (a <= a).should == true
    (a <= 4).should == false
    (a <= 45.8).should == false
  end
end
