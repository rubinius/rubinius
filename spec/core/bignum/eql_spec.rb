require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#eql?" do
  it "returns true if other is a Bignum with the same value" do
    a = BignumHelper.sbm(13)
    a.eql?(BignumHelper.sbm(13)).should == true
    a.eql?(2).should == false
    a.eql?(3.14).should == false
  end
end
