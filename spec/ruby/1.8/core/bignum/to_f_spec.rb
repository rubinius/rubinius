require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#to_f" do
  it "returns self converted to a Float" do
    BignumHelper.sbm(0x4000_0aa0_0bb0_0000).to_f.to_s.should == "1.38350697377893e+19"
    BignumHelper.sbm(0x8000_0000_0000_0ccc).to_f.to_s.should == "1.84467440737096e+19"
    (-BignumHelper.sbm(99)).to_f.to_s.should == "-9.22337203685478e+18"
  end
end
