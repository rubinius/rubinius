require File.expand_path('../../../spec_helper', __FILE__)

describe "Bignum#positive?" do
  it "returns true if self is greater than 0" do
    bignum_value.coerce(1).first.positive?.should == true
  end

  it "returns false if self is 0" do
    bignum_value.coerce(0).first.positive?.should == false
  end

  it "returns false if self is less than 0" do
    bignum_value.coerce(-1).first.positive?.should == false
  end
end
