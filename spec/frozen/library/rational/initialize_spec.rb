require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#initialize" do
  before(:each) do
    @rational = Rational.allocate
  end
  
  it "turns around the numerators and denominators signs around when the denominator is negative" do
    @rational.send(:initialize, 1, -2)
    @rational.numerator.should == -1
    @rational.denominator.should == 2
  end

  # I think this should work (at least Rational#initialize seems to be intended for this)
  # it "tries to convert the passed numerator and denominator to Integers using #to_i" do
  #   @rational.send(:initialize, "1", "2")
  #   @rational.numerator.should == "1"
  #   @rational.denominator.should == "2"
  # end
end
