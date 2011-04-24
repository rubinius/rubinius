require File.expand_path('../../../spec_helper', __FILE__)

describe "Fixnum#^" do
  it "returns self bitwise EXCLUSIVE OR other" do
    (3 ^ 5).should == 6
    (-2 ^ -255).should == 255
    (5 ^ bignum_value + 0xffff_ffff).should == 0x8000_0000_ffff_fffa
  end

  it "returns self bitwise EXCLUSIVE OR a Bignum" do
    (-1 ^ 2**64).should == -18446744073709551617
  end

  ruby_version_is "" ... "1.9" do
    ruby_bug "#", "1.8.6" do
      it "doesn't raise an error if passed a Float out of Fixnum range" do
        lambda { 1 ^ bignum_value(10000).to_f }.should_not raise_error()
        lambda { 1 ^ -bignum_value(10000).to_f }.should_not raise_error()
      end
    end

    it "converts a Float to an Integer" do
      (5 ^ 4.3).should == 1
      (-7 ^ 15.2).should == -10
    end
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (obj = mock('4')).should_receive(:to_int).and_return(4)
    (3 ^ obj).should == 7
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { 3 ^ obj }.should raise_error(TypeError)

    obj.should_receive(:to_int).and_return("asdf")
    lambda { 3 ^ obj }.should raise_error(TypeError)
  end

  ruby_bug "#", "1.8.6" do # Fixed at MRI 1.8.7
    it "coerces arguments correctly even if it is a Bignum" do
      (obj = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
      (3 ^ obj).should == 80000000000000000003
    end
  end
end
