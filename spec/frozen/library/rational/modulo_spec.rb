require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#%" do
  it "returns the remainder when this value is divided by other" do
    (Rational(2, 3) % Rational(2, 3)).should == Rational(0, 1)
    (Rational(4, 3) % Rational(2, 3)).should == Rational(0, 1)
    (Rational(2, -3) % Rational(-2, 3)).should == Rational(0, 1)
    (Rational(0, -1) % -1).should == Rational(0, 1)

    (Rational(7, 4) % Rational(1, 2)).should == Rational(1, 4)
    (Rational(7, 4) % 1).should == Rational(3, 4)
    (Rational(7, 4) % Rational(1, 7)).should == Rational(1, 28)

    (Rational(3, 4) % -1).should == Rational(-1, 4)
    (Rational(1, -5) % -1).should == Rational(-1, 5)
  end

  it "returns a Float value when the argument is Float" do
    (Rational(7, 4) % 1.0).should be_kind_of(Float)
    (Rational(7, 4) % 1.0).should == 0.75
    (Rational(7, 4) % 0.26).should be_close(0.19, 0.0001)
  end

  it "raises ZeroDivisionError on zero denominator" do
    lambda { Rational(3, 5) % Rational(0, 1) }.should raise_error(ZeroDivisionError)
    lambda { Rational(0, 1) % Rational(0, 1) }.should raise_error(ZeroDivisionError)
    lambda { Rational(3, 5) % 0 }.should raise_error(ZeroDivisionError)
  end

  it "raises FloatDomainError when the argument is 0.0" do
    lambda { Rational(3, 5) % 0.0 }.should raise_error(FloatDomainError)
  end
end
