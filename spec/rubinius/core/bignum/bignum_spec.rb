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
end
