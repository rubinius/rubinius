require File.dirname(__FILE__) + '/../../../spec_helper'
require 'mathn'

describe "Integer#prime_division" do
  it "Performs a prime factorization of an integer" do
    100.prime_division.should == [[2, 2], [5, 2]]
    -26.prime_division.should == [[2, 1]]
  end

  it "raises a ZeroDivisionError when is called on zero" do
    lambda { 0.prime_division }.should raise_error(ZeroDivisionError)
  end
end
