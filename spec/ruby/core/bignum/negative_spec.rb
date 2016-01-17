require File.expand_path('../../../spec_helper', __FILE__)

describe "Bignum#negative?" do
  it "returns true if self is less than 0" do
    bignum_value.coerce(-1).first.negative?.should == true
  end

  it "return false if self is 0" do
    bignum_value.coerce(0).first.negative?.should == false
  end

  it "return false if self is greater than 0" do
    bignum_value.coerce(1).first.negative?.should == false
  end
end
