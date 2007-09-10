require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#<=>" do
  it "returns -1, 0, 1 when self is less than, equal, or greater than other" do
    a = BignumHelper.sbm(96)
    (a <=> BignumHelper.sbm(196)).should == -1
    (a <=> a).should == 0
    (a <=> 4.5).should == 1
  end
end
