require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#coerce" do
  it "returns [Fixnum, Fixnum] if other is a Fixnum" do
    1.coerce(2).should == [2, 1]
    1.coerce(2).collect { |i| i.class }.should == [Fixnum, Fixnum]
  end
  
  it "returns [Float, Float] if other is not a Bignum or Fixnum" do
    a = 1.coerce("2")
    b = 1.coerce(1.5)
    a.should == [2.0, 1.0]
    a.collect { |i| i.class }.should == [Float, Float]
    b.should == [1.5, 1.0]
    b.collect { |i| i.class }.should == [Float, Float]
  end
  
  noncompliant :rubinius do
    it "returns [Bignum, Bignum] if other is a Bignum" do
      a = 1.coerce(0xffffffff)
      a.should == [4294967295, 1]
      a.collect { |i| i.class }.should == [Bignum, Bignum]
    end
  end
end
