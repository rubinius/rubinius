require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#quo" do
  before(:each) do
    @bignum = BignumHelper.sbm(3)
  end
  
  it "returns the result of self divided by the given Integer as a Float" do
    @bignum.quo(2.5).should_be_close(429496730.8, TOLERANCE)
    @bignum.quo(13).should_be_close(82595525.1538462, TOLERANCE)
    @bignum.quo(BignumHelper.sbm).should_be_close(1.00000000279397, TOLERANCE)
  end

  it "does not raise a ZeroDivisionError when the given Integer is 0" do
    @bignum.quo(0).to_s.should == "Infinity"
    (-@bignum).quo(0).to_s.should == "-Infinity"
  end

  it "does not raise a FloatDomainError when the given Integer is 0 and a Float" do
    @bignum.quo(0.0).to_s.should == "Infinity"
    (-@bignum).quo(0.0).to_s.should == "-Infinity"
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError) do
      (obj = Object.new).should_not_receive(:to_int)
      @bignum.quo(obj)
    end
    
    should_raise(TypeError) do
      @bignum.quo("10")
    end

    should_raise(TypeError) do
      @bignum.quo(:symbol)
    end
  end
end