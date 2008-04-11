require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#ceil" do
  before(:each) do
    @zero = BigDecimal("0")
    @mixed = BigDecimal("1.23456789")
    @mixed_big = BigDecimal("1.23456789E100")
    @pos_int = BigDecimal("2E5555")
    @neg_int = BigDecimal("-2E5555")
    @pos_frac = BigDecimal("2E-9999")
    @neg_frac = BigDecimal("-2E-9999")

    @infinity = BigDecimal("Infinity")
    @infinity_neg = BigDecimal("-Infinity")
    @nan = BigDecimal("NaN")
    @zero_pos = BigDecimal("+0")
    @zero_neg = BigDecimal("-0")
  end

  it "returns the smallest integer greater or equal to self" do
    @pos_int.ceil.should == @pos_int
    @neg_int.ceil.should == @neg_int
    @pos_frac.ceil.should == BigDecimal("1")
    @neg_frac.ceil.should == @zero
    @infinity.ceil.should == @infinity
    @infinity_neg.ceil.should == @infinity_neg
    @nan.ceil.nan?.should == true
    @zero.ceil.should == 0
    @zero_pos.ceil.should == @zero_pos
    @zero_neg.ceil.should == @zero_neg

    BigDecimal('2.3').ceil.should == 3
    BigDecimal('2.5').ceil.should == 3
    BigDecimal('2.9999').ceil.should == 3
    BigDecimal('-2.3').ceil.should == -2
    BigDecimal('-2.5').ceil.should == -2
    BigDecimal('-2.9999').ceil.should == -2
  end

  it "returns n digits right of the decimal point if given n > 0" do
    @mixed.ceil(1).should == BigDecimal("1.3")
    @mixed.ceil(5).should == BigDecimal("1.23457")
  end

  it "sets n digits left of the decimal point to 0, if given n < 0" do
    BigDecimal("13345.234").ceil(-2).should == BigDecimal("13400.0")
    @mixed_big.ceil(-99).should == BigDecimal("0.13E101")
    @mixed_big.ceil(-100).should == BigDecimal("0.2E101")
    @mixed_big.ceil(-95).should == BigDecimal("0.123457E101")
  end

end