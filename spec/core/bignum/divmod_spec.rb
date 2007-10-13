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
    should_raise(ZeroDivisionError) do
      @bignum.divmod(0)
    end

    should_raise(ZeroDivisionError) do
      (-@bignum).divmod(0)
    end
  end
  
  it "raises a FloatDomainError when the given argument is 0 and a Float" do
    should_raise(FloatDomainError, "NaN") do
      @bignum.divmod(0.0)
    end
    
    should_raise(FloatDomainError, "NaN") do
      (-@bignum).divmod(0.0)
    end
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError) do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      @bignum.divmod(obj)
    end
    
    should_raise(TypeError) do
      @bignum.divmod("10")
    end

    should_raise(TypeError) do
      @bignum.divmod(:symbol)
    end
  end
end
