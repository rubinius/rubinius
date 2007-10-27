require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Bignum" do
    specify "should have max value 2 ** 29" do
      max = 2 ** 29
      max.class.should == Bignum
      sub = max - 1
      sub.should == 536870911
      sub.class.should == Fixnum
    end
    
    specify "should have min value -(2 ** 29)" do
      min = -2 ** 29
      min.class.should == Bignum
      sub = min + 1
      sub.should == -536870911
      sub.class.should == Fixnum
    end
  end

  describe "Bignum#coerce" do
    it "returns two bignums even if the given argument is a Fixnum" do
      a = 0xffffffff.coerce(1)
      a.should == [1, 4294967295]
      a.collect { |i| i.class }.should == [Bignum, Bignum]
    end
  end
end

