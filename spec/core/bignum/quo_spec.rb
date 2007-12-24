require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#quo" do
  before(:each) do
    @bignum = BignumHelper.sbm(3)
  end
  
  it "returns the result of self divided by the given Integer as a Float" do
    @bignum.quo(2.5).should be_close(429496730.8, TOLERANCE)
    @bignum.quo(13).should be_close(82595525.1538462, TOLERANCE)
    @bignum.quo(BignumHelper.sbm).should be_close(1.00000000279397, TOLERANCE)
  end

  runner_not :rspec do
    it "does not raise a ZeroDivisionError when the given Integer is 0" do
      @bignum.quo(0).to_s.should == "Infinity"
      (-@bignum).quo(0).to_s.should == "-Infinity"
    end
  end

  it "does not raise a FloatDomainError when the given Integer is 0 and a Float" do
    @bignum.quo(0.0).to_s.should == "Infinity"
    (-@bignum).quo(0.0).to_s.should == "-Infinity"
  end

  runner_not :rspec do
    it "raises a TypeError when given a non-Integer" do
      lambda {
        (obj = mock('to_int')).should_not_receive(:to_int)
        @bignum.quo(obj)
      }.should raise_error(TypeError)
      lambda { @bignum.quo("10") }.should raise_error(TypeError)
      lambda { @bignum.quo(:symbol) }.should raise_error(TypeError)
    end
  end
end