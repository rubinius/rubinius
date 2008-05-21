require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'

describe "Numeric#polar" do
  it "returns self's absolute value and self's argument" do
    5.polar.should == [5, 0]
    (-5).polar.should == [5, Math::PI]
    
    4.5.polar.should == [4.5, 0]
    (-4.5).polar.should == [4.5, Math::PI]

    bignum_value.polar.should == [bignum_value, 0]
    (-bignum_value).polar.should == [bignum_value, Math::PI]
  end
end