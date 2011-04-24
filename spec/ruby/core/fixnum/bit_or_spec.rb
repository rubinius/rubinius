require File.expand_path('../../../spec_helper', __FILE__)

describe "Fixnum#|" do
  it "returns self bitwise OR other" do
    (1 | 0).should == 1
    (5 | 4).should == 5
    (5 | 6).should == 7
    (248 | 4096).should == 4344
    (0xffff | bignum_value + 0xf0f0).should == 0x8000_0000_0000_ffff
  end

  it "returns self bitwise OR a Bignum" do
    (-1 | 2**64).should == -1
  end

  ruby_version_is "" ... "1.9" do
    ruby_bug "#", "1.8.6" do
      it "doesn't raise an error if passed a Float out of Fixnum range" do
        lambda { 1 | bignum_value(10000).to_f }.should_not raise_error()
        lambda { 1 | -bignum_value(10000).to_f }.should_not raise_error()
      end
    end

    it "converts a Float to an Integer" do
      (5 | 4.3).should == 5
    end
  end

  it "tries to convert the int like argument to an Integer using to_int" do
    (obj = mock('4')).should_receive(:to_int).and_return(4)
    (3 | obj).should == 7
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { 3 | obj }.should raise_error(TypeError)

    obj.should_receive(:to_int).and_return("asdf")
    lambda { 3 | obj }.should raise_error(TypeError)
  end

  ruby_bug "#", "1.8.6" do # Fixed at MRI 1.8.7
    it "coerces arguments correctly even if it is a Bignum" do
      (obj = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
      (3 | obj).should == 80000000000000000003
    end
  end
end
