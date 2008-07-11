require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex when passed [Complex, Complex]" do
  it "returns a new Complex number based on the two given numbers" do
    Complex(Complex(3, 4), Complex(5, 6)).should == Complex.new(3 - 6, 4 + 5)
    Complex(Complex(1.5, 2), Complex(-5, 6.3)).should == Complex.new(1.5 - 6.3, 2 - 5)
  end
end

describe "Complex when passed [Complex]" do
  it "returns the passed Complex number" do
    Complex(Complex(1, 2)).should == Complex(1, 2)
    Complex(Complex(-3.4, bignum_value)).should == Complex(-3.4, bignum_value)
  end
end

describe "Complex when passed [Integer, Integer]" do
  it "returns a new Complex number" do
    Complex(1, 2).should == Complex.new(1, 2)
    Complex(-3, -5).should == Complex.new(-3, -5)
    Complex(3.5, -4.5).should == Complex.new(3.5, -4.5)
    Complex(bignum_value, 30).should == Complex.new(bignum_value, 30)
  end
end

describe "Complex when passed [Integer]" do
  it "returns a new Complex number with 0 as the imaginary component" do
    # Guard against the Mathn library
    conflicts_with :Prime do
      Complex(1).should eql(Complex.new(1, 0))
      Complex(-3).should eql(Complex.new(-3, 0))
      Complex(-4.5).should eql(Complex.new(-4.5, 0))
      Complex(bignum_value).should eql(Complex.new(bignum_value, 0))
    end
  end
  
  it "returns the passed Integer when Complex::Unify is defined" do
    # Guard against the Mathn library
    conflicts_with :Prime do
      begin
        Complex::Unify = true
      
        Complex(1).should eql(1)
        Complex(-3).should eql(-3)
        Complex(-4.5).should eql(-4.5)
        Complex(bignum_value).should eql(bignum_value)
      ensure
        Complex.send :remove_const, :Unify
      end
    end
  end
end
