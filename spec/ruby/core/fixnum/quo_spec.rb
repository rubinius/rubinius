require File.expand_path('../../../spec_helper', __FILE__)

describe "Fixnum#quo" do
  it "returns the result of self divided by the given Integer as a Float" do
    2.quo(2.5).should == 0.8
    5.quo(2).should == 2.5
    45.quo(bignum_value).should be_close(1.04773789668636e-08, TOLERANCE)
  end

  ruby_version_is ""..."1.9" do
    conflicts_with :Rational do
      it "does not raise a ZeroDivisionError when the given Integer is 0" do
        0.quo(0).to_s.should == "NaN"
        10.quo(0).to_s.should == "Infinity"
        -10.quo(0).to_s.should == "-Infinity"
      end
    end
  end

  ruby_version_is "1.9" do
    it "raises a ZeroDivisionError when the given Integer is 0" do
      lambda { 0.quo(0) }.should raise_error(ZeroDivisionError)
      lambda { 10.quo(0) }.should raise_error(ZeroDivisionError)
      lambda { -10.quo(0) }.should raise_error(ZeroDivisionError)
    end
  end

  it "does not raise a FloatDomainError when the given Integer is 0 and a Float" do
    0.quo(0.0).to_s.should == "NaN"
    10.quo(0.0).to_s.should == "Infinity"
    -10.quo(0.0).to_s.should == "-Infinity"
  end

  conflicts_with :Rational do
    it "raises a TypeError when given a non-Integer" do
      lambda {
        (obj = mock('x')).should_not_receive(:to_int)
        13.quo(obj)
      }.should raise_error(TypeError)
      lambda { 13.quo("10")    }.should raise_error(TypeError)
      lambda { 13.quo(:symbol) }.should raise_error(TypeError)
    end
  end
end
