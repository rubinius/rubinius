require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#divmod" do
  it "returns an Array containing quotient and modulus obtained from dividing self by the given argument" do
    13.divmod(4).should == [3, 1]
    4.divmod(13).should == [0, 4]

    13.divmod(4.0).should == [3, 1]
    4.divmod(13.0).should == [0, 4]

    1.divmod(2.0).should == [0, 1.0]
    200.divmod(0xffffffff).should == [0, 200]
  end
  
  it "raises a ZeroDivisionError when the given argument is 0" do
    lambda { 13.divmod(0)  }.should raise_error(ZeroDivisionError)
    lambda { 0.divmod(0)   }.should raise_error(ZeroDivisionError)
    lambda { -10.divmod(0) }.should raise_error(ZeroDivisionError)
  end

  version '1.8.4' do
    it "returns [NaN, NaN] if the given argument is 0 and is a Float" do
      1.divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  end
  
  version '1.8.5'..'1.8.6' do
    it "raises a FloatDomainError when the given argument is 0 and a Float" do
      lambda { 0.divmod(0.0)   }.should raise_error(FloatDomainError)
      lambda { 10.divmod(0.0)  }.should raise_error(FloatDomainError)
      lambda { -10.divmod(0.0) }.should raise_error(FloatDomainError)
    end
  end

  it "raises a TypeError when given a non-Integer" do
    lambda {
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13.divmod(obj)
    }.should raise_error(TypeError)
    lambda { 13.divmod("10")    }.should raise_error(TypeError)
    lambda { 13.divmod(:symbol) }.should raise_error(TypeError)
  end
end
