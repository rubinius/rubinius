require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#divmod" do
  it "returns an array containing quotient and modulus obtained from dividing self by the given argument" do
    13.divmod(4).should == [3, 1]
    4.divmod(13).should == [0, 4]

    13.divmod(4.0).should == [3, 1]
    4.divmod(13.0).should == [0, 4]

    1.divmod(2.0).should == [0, 1.0]
    200.divmod(0xffffffff).should == [0, 200]
  end
  
  it "raises a ZeroDivisionError when the given argument is 0" do
    should_raise(ZeroDivisionError) do
      13.divmod(0)
    end

    should_raise(ZeroDivisionError) do
      0.divmod(0)
    end
    
    should_raise(ZeroDivisionError) do
      -10.divmod(0)
    end
  end

  version '1.8.4' do
    it "returns [NaN, NaN] if the given argument is 0 and is a Float" do
      1.divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  end
  
  version '1.8.5'..'1.8.6' do
    it "raises a FloatDomainError when the given argument is 0 and a Float" do
      should_raise(FloatDomainError, "NaN") do
        0.divmod(0.0)
      end
      
      should_raise(FloatDomainError, "NaN") do
        10.divmod(0.0)
      end
      
      should_raise(FloatDomainError, "NaN") do
        -10.divmod(0.0)
      end
    end
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError) do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13.divmod(obj)
    end
    
    should_raise(TypeError) do
      13.divmod("10")
    end

    should_raise(TypeError) do
      13.divmod(:symbol)
    end
  end
end
