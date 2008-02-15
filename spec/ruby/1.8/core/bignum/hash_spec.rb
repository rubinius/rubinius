require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#hash" do
  it "is provided" do
    bignum_value.respond_to?(:hash).should == true
  end

  it "is stable" do
    (2**100).hash.should == (2**100).hash
  end

end
