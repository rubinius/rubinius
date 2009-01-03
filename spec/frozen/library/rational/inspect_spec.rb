require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#inspect" do
  conflicts_with :Prime do
    it "returns a reconstructable string representation of self" do
      Rational(3, 4).inspect.should == "Rational(3, 4)"
      Rational(-5, 8).inspect.should == "Rational(-5, 8)"
      Rational(-1, -2).inspect.should == "Rational(1, 2)"
      Rational(bignum_value, 1).inspect.should == "Rational(#{bignum_value}, 1)"
    end
  end
end
