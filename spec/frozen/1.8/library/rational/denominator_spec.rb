require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#denominator" do
  it "returns the denominator" do
    Rational(3, 4).denominator.should equal(4)
    Rational(3, -4).denominator.should equal(4)
    
    Rational(1, bignum_value).denominator.should == bignum_value
  end
end
