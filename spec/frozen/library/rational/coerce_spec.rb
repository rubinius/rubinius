require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#coerce when passed [Rational]" do
  it "returns the passed argument, self as Floats" do
    result = Rational(3, 4).coerce(1.0)
    result.should == [1.0, 0.75]
    result.first.is_a?(Float).should be_true
    result.last.is_a?(Float).should be_true
  end
end

describe "Rational#coerce when passed [Rational]" do
  it "returns the passed argument, self as Rationals" do
    result = Rational(3, 4).coerce(10)
    result.should == [Rational(10, 1), Rational(3, 4)]
    result.first.is_a?(Rational).should be_true
    result.last.is_a?(Rational).should be_true
  end
end

describe "Rational#coerce" do
  it "does try to convert the passed argument into a Float (using #to_f)" do
    obj = mock("something")
    obj.should_receive(:to_f).and_return(1.1)
    Rational(3, 4).coerce(obj)
  end
  
  it "returns the passed argument, self converted to Floats" do
    obj = mock("something")
    obj.should_receive(:to_f).and_return(1.1)
    
    result = Rational(3, 4).coerce(obj)
    result.should == [1.1, 0.75]
    result.first.is_a?(Float).should be_true
    result.last.is_a?(Float).should be_true
  end
end