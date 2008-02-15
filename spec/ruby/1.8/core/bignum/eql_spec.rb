require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#eql? when given a Bignum" do
  it "returns true if the given argument has the same value" do
    a = bignum_value(13)
    a.eql?(bignum_value(13)).should == true
    (-a).eql?(-bignum_value(13)).should == true
  end
end

describe "Bignum#eql? when given a non-Bignum" do
  it "returns false if the given argument is not a Bignum" do
    a = bignum_value(13)
    a.eql?(2).should == false
    a.eql?(3.14).should == false
    a.eql?(:symbol).should == false
    a.eql?("String").should == false
    a.eql?(mock('str')).should == false
  end
end
