require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'

describe "Numeric#real" do
  it "returns self" do
    20.real.should == 20
    (-4.5).real.should == -4.5
    bignum_value.real.should == bignum_value
  end
end