require File.expand_path('../../../spec_helper', __FILE__)

describe "Bignum#&" do
  before(:each) do
    @bignum = bignum_value(5)
  end

  it "returns self bitwise AND other" do
    @bignum = bignum_value(5)
    (@bignum & 3).should == 1
    (@bignum & 52).should == 4
    (@bignum & bignum_value(9921)).should == 9223372036854775809

    ((2*bignum_value) & 1).should == 0
    ((2*bignum_value) & (2*bignum_value)).should == 18446744073709551616
  end

  it "returns self bitwise AND other when one operand is negative" do
    ((2*bignum_value) & -1).should == 18446744073709551616
    ((4*bignum_value) & -1).should == 36893488147419103232
    (@bignum & -0xffffffffffffff5).should == 9223372036854775809
    (@bignum & -@bignum).should == 1
    (@bignum & -0x8000000000000000).should == 9223372036854775808
  end

  it "returns self bitwise AND other when both operands are negative" do
    (-@bignum & -0x4000000000000005).should == -13835058055282163717
    (-@bignum & -@bignum).should == -9223372036854775813
    (-@bignum & -0x4000000000000000).should == -13835058055282163712
  end

  ruby_version_is ""..."1.9" do
    it "coerces Float arguments into Integers" do
      (@bignum & 3.4).should == 1
      (bignum_value & bignum_value(0xffff).to_f).should == 9223372036854775808
    end
  end

  ruby_version_is "1.9" do
    it "raises a TypeError when passed a Float" do
      lambda { (@bignum & 3.4) }.should raise_error(TypeError)
      lambda {
        (bignum_value & bignum_value(0xffff).to_f)
      }.should raise_error(TypeError)
    end
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (obj = mock('3')).should_receive(:to_int).and_return(3)
    (@bignum & obj).should == 1
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum & obj }.should raise_error(TypeError)

    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum & obj }.should raise_error(TypeError)
  end
end
