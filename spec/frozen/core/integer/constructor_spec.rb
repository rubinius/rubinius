require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Integer()" do
  it "returns a Fixnum or Bignum object" do
    Integer(2).should be_an_instance_of(Fixnum)
    Integer(9**99).should be_an_instance_of(Bignum)
  end
  
  it "calls to_i on a non-numeric argument" do
    nine = mock('9')
    nine.should_receive(:to_i).and_return(9)
    Integer(nine).should == 9
    Integer('200').should == 200
  end
  
  it "truncates Floats" do
    Integer(3.14).should == 3
    Integer(90.8).should == 90
  end
    
  it "calls to_i on Rationals" do
    Integer(Rational(8,3)).should == 2
    Integer(3.quo(2)).should == 1
  end

  it "passes through Bignums as-is" do
    bignum = 99**99
    bignum.should be_an_instance_of(Bignum)
    Integer(bignum).should == bignum
    Integer(bignum).should be_an_instance_of(Bignum)
  end

  it "passes through Fixnums as-is" do
    fixnum = 99
    fixnum.should be_an_instance_of(Fixnum)
    Integer(fixnum).should == fixnum
    Integer(fixnum).should be_an_instance_of(Fixnum)
  end

  ruby_version_is "1.9" do
    it "raises a TypeError when passed nil" do
      lambda { Integer(nil) }.should raise_error(TypeError)
    end
  end
  
  ruby_version_is ""..."1.9" do
    it "returns 0 when passed nil" do
      Integer(nil).should == 0
    end
  end

  it "raises a FloatDomainError when passed NaN" do
    lambda { Integer(0.0/0.0) }.should raise_error(FloatDomainError)
  end  
end
