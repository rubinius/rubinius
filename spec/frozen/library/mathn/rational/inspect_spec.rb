require File.dirname(__FILE__) + '/../../../spec_helper'
require 'mathn'

describe "Rational#inspect" do
  it "returns a string representation of self" do
    Rational.new!(3, 4).inspect.should == "3/4"
    Rational.new!(-5, 8).inspect.should == "-5/8"
    Rational.new!(-1, -2).inspect.should == "1/2"
    Rational.new!(0, 2).inspect.should == "0/2"
    Rational.new!(bignum_value, 1).inspect.should == "#{bignum_value}/1"
  end
end
