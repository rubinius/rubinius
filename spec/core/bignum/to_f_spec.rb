require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#to_f" do
  it "returns self converted to a Float" do
    BignumHelper.sbm(2).to_f.should == 1073741826.0
    BignumHelper.sbm(14).to_f.should == 1073741838.0
    (-BignumHelper.sbm(99)).to_f.should == -1073741923.0
  end
end
