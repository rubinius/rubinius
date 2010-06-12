require File.expand_path('../../../spec_helper', __FILE__)

describe "Kernel#to_bool" do
  it "returns true if self evaluates to true" do
    true.to_bool.should be_true
    1.to_bool.should be_true
    1.5.to_bool.should be_true
    bignum_value.to_bool.should be_true
    "string".to_bool.should be_true
    Object.new.to_bool.should be_true
  end

  it "returns false if self evaluates to false" do
    nil.to_bool.should be_false
    false.to_bool.should be_false
  end
end
