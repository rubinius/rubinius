require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#divmod" do
  before(:each) do
    @bignum = BignumHelper.sbm(55)
  end
  
  it "returns an Array containing quotient and modulus obtained from dividing self by the given argument" do
    @bignum.divmod(4).should == [268435469, 3]
    @bignum.divmod(13).should == [82595529, 2]

    @bignum.divmod(4.0).should == [268435469, 3.0]
    @bignum.divmod(13.0).should == [82595529, 2.0]

    @bignum.divmod(2.0).should == [536870939, 1.0]
    @bignum.divmod(0xffffffff).should == [0,  1073741879]
  end
  
  it "raises a ZeroDivisionError when the given argument is 0" do
    lambda { @bignum.divmod(0) }.should raise_error(ZeroDivisionError)
    lambda { (-@bignum).divmod(0) }.should raise_error(ZeroDivisionError)
  end
  
  it "raises a FloatDomainError when the given argument is 0 and a Float" do
    lambda { @bignum.divmod(0.0) }.should raise_error(FloatDomainError, "NaN")
    lambda { (-@bignum).divmod(0.0) }.should raise_error(FloatDomainError, "NaN")
  end

  it "raises a TypeError when given a non-Integer" do
    lambda {
      (obj = Object.new).should_receive(:to_int).and_return(10)
      @bignum.divmod(obj)
    }.should raise_error(TypeError)
    lambda { @bignum.divmod("10") }.should raise_error(TypeError)
    lambda { @bignum.divmod(:symbol) }.should raise_error(TypeError)
  end
end
