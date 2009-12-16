require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#hash" do
  it "is provided" do
    bignum_value.respond_to?(:hash).should == true
  end

  it "is stable" do
    bignum_value.hash.should == bignum_value.hash
    bignum_value.hash.should_not == bignum_value(1).hash
  end
end
