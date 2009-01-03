require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

# FIXME:
# Complex#denominator requires the rational library,
# as Integer#denominator is defined by it.
# I think this method is pretty buggy, as there is no
# denominator for Floats and rational might not always
# be loaded, both resulting in a method missing exception.
# Also, the documentation for Complex#denominator does
# not mention a dependency for rational.
require "rational"

describe "Complex#denominator" do
  it "returns the least common multiple denominator of the real and imaginary parts" do
    Complex(3, 4).denominator.should == 1
    Complex(3, bignum_value).denominator.should == 1

    Complex(3, Rational(3,4)).denominator.should == 4

    Complex(Rational(4,8), Rational(3,4)).denominator.should == 4
    Complex(Rational(3,8), Rational(3,4)).denominator.should == 8
  end
end