require File.expand_path('../../../spec_helper', __FILE__)

describe "Bignum#coerce" do
  it "returns [Bignum, Bignum] if other is a Fixnum" do
      a = bignum_value.coerce(1)
      a.should == [1, 9223372036854775808]
      a.collect { |i| i.class }.should == [Bignum, Bignum]
  end

  it "returns [Float, Float] if other is not a Bignum or Fixnum" do
    a, b = c = bignum_value.coerce("2")
    a.should be_close(2.0, TOLERANCE)
    b.should be_close(9223372036854775808.000, TOLERANCE)
    c.collect { |i| i.class }.should == [Float, Float]
    
    a, b = c = bignum_value.coerce(1.5)
    a.should be_close(1.5, TOLERANCE)
    b.should be_close(9223372036854775808.000, TOLERANCE)
    c.collect { |i| i.class }.should == [Float, Float]
  end
end
