require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#div with a Fixnum" do
  it "returns self divided by the given argument as an Integer" do
    2.div(2).should == 1
    1.div(2).should == 0
    5.div(2).should == 2
  end
end

describe "Fixnum#div" do
  it "rounds towards -inf" do
    8192.div(10).should == 819
    8192.div(-10).should == -820
    (-8192).div(10).should == -820
    (-8192).div(-10).should == 819
  end

  it "coerces self and the given argument to Floats and returns self divided by other as Fixnum" do
    1.div(0.2).should == 5
    1.div(0.16).should == 6
    1.div(0.169).should == 5
    -1.div(50.4).should == -1
    1.div(0xffffffff).should == 0
  end
  
  it "raises a FloatDomainError when the given argument is 0 and a Float" do
    should_raise(FloatDomainError, "NaN") do
      0.div(0.0)
    end
    
    should_raise(FloatDomainError, "Infinity") do
      10.div(0.0)
    end
    
    should_raise(FloatDomainError, "-Infinity") do
      -10.div(0.0)
    end
  end

  it "raises a ZeroDivisionError when the given argument is 0" do
    should_raise(ZeroDivisionError) do
      13.div(0)
    end
  end
  
  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError) do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13.div(obj)
    end
    
    should_raise(TypeError) do
      5.div("2")
    end
  end
end
