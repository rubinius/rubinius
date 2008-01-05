require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#ceil" do
  it "returns the smallest Integer greater than or equal to self when passed a Fixnum" do
    0.ceil.should == 0
    100.ceil.should == 100
    -100.ceil.should == -100
  end

  it "returns the smallest Integer greater than or equal to self when passed a Float" do
    0.ceil.should == 0.0
    34.56.ceil.should == 35
    -34.56.ceil.should == -34
  end

  it "returns the smallest Integer greater than or equal to self when passed a Bignum" do
  # FIXME: use bignum_value helper
    2147483648.ceil.should == 2147483648
    -2147483648.ceil.should == -2147483648
    9223372036854775808.ceil.should == 9223372036854775808
    -9223372036854775808.ceil.should == -9223372036854775808  
  end
end
