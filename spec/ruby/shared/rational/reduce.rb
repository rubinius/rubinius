require File.expand_path('../../../spec_helper', __FILE__)
require 'rational'

describe :rational_reduce_unfiy, :shared => true do
  # This is not consistent with the Complex library, Complex checks
  # Complex::Unify and not the top-level Unify.
  it "returns an Integer when the reduced denominator is 1" do
    begin
      Unify = true

      Rational.reduce(3, 1).should eql(3)
      Rational.reduce(5, 1).should eql(5)
      Rational.reduce(4, 2).should eql(2)
      Rational.reduce(-9, 3).should eql(-3)
    ensure
      Object.send :remove_const, :Unify
    end
  end
end

describe :rational_reduce, :shared => true do
  it "returns a new Rational with the passed numerator and denominator reduced to their lowest terms" do
    Rational(6, 8).should eql(Rational(3, 4))
    Rational(-5, 10).should eql(Rational(-1, 2))
  end

  it "raises a ZeroDivisionError when the passed denominator is 0" do
    lambda { Rational.reduce(4, 0) }.should raise_error(ZeroDivisionError)
    lambda { Rational.reduce(-1, 0) }.should raise_error(ZeroDivisionError)
  end
end
