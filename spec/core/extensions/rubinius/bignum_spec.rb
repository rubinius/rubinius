require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Bignum" do
    it "has positive threshold value 2 ** 29" do
      max = 2 ** 29
      max.class.should == Bignum
      sub = max - 1
      sub.should == 536870911
      sub.class.should == Fixnum
    end
    
    it "has negative threshold value -(2 ** 29)" do
      min = -2 ** 29
      min.class.should == Bignum
      sub = min + 1
      sub.should == -536870911
      sub.class.should == Fixnum
    end
  end

  context "Bignum#coerce" do
    it "returns [Bignum, Bignum] if other is a Fixnum" do
        a = 0xffffffff.coerce(1)
        a.should == [1, 4294967295]
        a.collect { |i| i.class }.should == [Bignum, Bignum]
    end

    it "returns [Float, Float] if other is not a Bignum or Fixnum" do
      a = 0xffffffff.coerce("2")
      a.inspect.should == "[2.0, 4294967295.0]"
      a.collect { |i| i.class }.should == [Float, Float]
      b = 0xffffffff.coerce(1.5)
      b.inspect.should == "[1.5, 4294967295.0]"
      b.collect { |i| i.class }.should == [Float, Float]
    end
  end
end

