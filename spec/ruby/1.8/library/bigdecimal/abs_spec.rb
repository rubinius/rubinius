require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'



describe "BigDecimal#abs" do

  it "returns the absolute value" do
    zero = BigDecimal("0")
    mixed = BigDecimal("1.23456789")
    pos_int = BigDecimal("2E5555")
    neg_int = BigDecimal("-2E5555")
    pos_frac = BigDecimal("2E-9999")
    neg_frac = BigDecimal("-2E-9999")

    pos_int.abs.should == pos_int
    neg_int.abs.should == pos_int
    pos_frac.abs.should == pos_frac
    neg_frac.abs.should == pos_frac
  end

end