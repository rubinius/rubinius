require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#== when passed [Rational]" do
  it "returns true if self has the same numerator and denominator as the passed argument" do
    (Rational(3, 4) == Rational(3, 4)).should be_true
    (Rational(-3, -4) == Rational(3, 4)).should be_true
    (Rational(-4, 5) == Rational(4, -5)).should be_true
    
    (Rational(bignum_value, 3) == Rational(bignum_value, 3)).should be_true
    (Rational(-bignum_value, 3) == Rational(bignum_value, -3)).should be_true
  end
  
  it "returns false if self has different numerator/denominator but the same numerical value as the passed argument" do
    (Rational(3, 4) == Rational.new!(6, 8)).should be_false
    (Rational(-3, 4) == Rational.new!(-6, 8)).should be_false
  end
end

describe "Rational#== when passed [Integer]" do
  it "returns true if self has the passed argument as numerator and a denominator of 1" do
    # Rational(x, y) reduces x and y automatically
    (Rational(4, 2) == 2).should be_true
    (Rational(-4, 2) == -2).should be_true
    (Rational(4, -2) == -2).should be_true

    (Rational.new!(4, 2) == 2).should be_false
    (Rational.new!(9, 3) == 3).should be_false
  end
end

describe "Rational#== when passed [Float]" do
  it "converts self to a Float and compares it with the passed argument" do
    (Rational(3, 4) == 0.75).should be_true
    (Rational(4, 2) == 2.0).should be_true
    (Rational(-4, 2) == -2.0).should be_true
    (Rational(4, -2) == -2.0).should be_true

    # This is inconsistent to behaviour when passed Integers or Rationals
    (Rational.new!(4, 2) == 2.0).should be_true
    (Rational.new!(9, 3) == 3.0).should be_true
  end
end

describe "Rational#==" do
  it "returns the result of calling #== with self on the passed argument" do
    obj = mock("Object")
    obj.should_receive(:==).and_return(:result)
    
    (Rational(3, 4) == obj).should == :result
  end
end