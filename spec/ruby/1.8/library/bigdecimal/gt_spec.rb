require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#>" do

  it "returns true if a > b" do
    one = BigDecimal("1")
    zero = BigDecimal("0")
    two = BigDecimal("2")
    infinity = BigDecimal("Infinity")
    infinity_minus = BigDecimal("-Infinity")
    one_minus = BigDecimal("-1")
    frac_1 = BigDecimal("1E-99999")
    frac_2 = BigDecimal("0.9E-99999")
    (zero > one).should == false
    (two > zero).should == true
    (infinity > one_minus).should == true
    (zero > infinity).should == false
    (infinity_minus > infinity).should == false
    (frac_2 > frac_1).should == false
  end

end
