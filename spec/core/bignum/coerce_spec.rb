require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#coerce" do
  it "returns [Bignum, Bignum] if other is a Fixnum" do
      a = bignum_value.coerce(1)
      a.should == [1, 9223372036854775808]
      a.collect { |i| i.class }.should == [Bignum, Bignum]
  end

  it "returns [Float, Float] if other is not a Bignum or Fixnum" do
    a = bignum_value.coerce("2")
    a.inspect.should == "[2.0, 9.22337203685478e+18]"
    a.collect { |i| i.class }.should == [Float, Float]
    b = bignum_value.coerce(1.5)
    b.inspect.should == "[1.5, 9.22337203685478e+18]"
    b.collect { |i| i.class }.should == [Float, Float]
  end
end
