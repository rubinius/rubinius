require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#eql? when given a Bignum" do
  it "returns true if the given argument has the same value" do
    a = BignumHelper.sbm(13)
    a.eql?(BignumHelper.sbm(13)).should == true
    (-a).eql?(-BignumHelper.sbm(13)).should == true
  end
end

describe "Bignum#eql? when given a non-Bignum" do
  it "returns false if the given argument is not a Bignum" do
    a = BignumHelper.sbm(13)
    a.eql?(2).should == false
    a.eql?(3.14).should == false
    a.eql?(:symbol).should == false
    a.eql?("String").should == false
    a.eql?(mock('str')).should == false
  end
end
