require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#<" do

  it "returns true if a < b" do
    one = BigDecimal("1")
    zero = BigDecimal("0")
    two = BigDecimal("2")
    infinity = BigDecimal("Infinity")
    infinity_minus = BigDecimal("-Infinity")
    one_minus = BigDecimal("-1")
    frac_1 = BigDecimal("1E-99999")
    frac_2 = BigDecimal("0.9E-99999")
    (zero < one).should == true
    (two < zero).should == false
    (infinity < one_minus).should == false
    (zero < infinity).should == true
    (infinity_minus < infinity).should == true
    (frac_2 < frac_1).should == true
  end

end
