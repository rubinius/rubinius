require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#>=" do
  it "returns true if self is greater than or equal to other" do
    a = BignumHelper.sbm(14)
    (a >= a).should == true
    (a >= (a + 2)).should == false
    (a >= 5664.2).should == true
    (a >= 4).should == true
  end
end
