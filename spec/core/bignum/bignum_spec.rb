require File.expand_path('../../../spec_helper', __FILE__)

describe "Bignum" do
  it "has minimum positive value 2 ** 62" do
    max = 2 ** 62
    max.class.should == Bignum
    sub = max - 1
    sub.should == 4611686018427387903
    sub.class.should == Fixnum
  end

  it "has maximum negative threshold value -(2 ** 62) - 1" do
    min = -2 ** 62
    min.class.should == Fixnum
    sub = min - 1
    sub.should == -4611686018427387905
    sub.class.should == Bignum
  end
end
