require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum" do
  platform_is :wordsize => 32 do
    it "has positive threshold value 2 ** 29" do
      max = 2 ** 29
      max.class.should == Bignum
      sub = max - 1
      sub.should == 536870911
      sub.class.should == Fixnum
    end
  end

  platform_is :wordsize => 64 do
    it "has positive threshold value 2 ** 61" do
      max = 2 ** 61
      max.class.should == Bignum
      sub = max - 1
      sub.should == 2305843009213693951
      sub.class.should == Fixnum
    end
  end
  
  platform_is :wordsize => 32 do
    it "has negative threshold value -(2 ** 29)" do
      min = -2 ** 29
      min.class.should == Bignum
      sub = min + 1
      sub.should == -536870911
      sub.class.should == Fixnum
    end
  end

  platform_is :wordsize => 64 do
    it "has negative threshold value -(2 ** 61)" do
      min = -2 ** 61
      min.class.should == Bignum
      sub = min + 1
      sub.should == -2305843009213693951
      sub.class.should == Fixnum
    end
  end
end
