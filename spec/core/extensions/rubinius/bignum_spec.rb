require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Bignum" do
    specify "should have max value 2 ** 29" do
      max = 2 ** 29
      max.class.should == Bignum
      (max - 1).class.should == Fixnum
    end
  end

  context "Bignum instance method" do
    specify "coerce should return [Bignum, Bignum] if other is a Fixnum" do
        a = 0xffffffff.coerce(1)
        a.should == [1, 4294967295]
        a.collect { |i| i.class }.should == [Bignum, Bignum]
    end

    specify "coerce should return [Float, Float] if other is not a Bignum or Fixnum" do
      a = 0xffffffff.coerce("2")
      a.inspect.should == "[2.0, 4294967295.0]"
      a.collect { |i| i.class }.should == [Float, Float]
      b = 0xffffffff.coerce(1.5)
      b.inspect.should == "[1.5, 4294967295.0]"
      b.collect { |i| i.class }.should == [Float, Float]
    end
  end
end

