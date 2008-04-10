require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'



describe "BigDecimal#floor" do

  before(:each) do
    @zero = BigDecimal("0")
    @mixed = BigDecimal("1.23456789")
    @mixed_big = BigDecimal("1.23456789E100")
    @pos_int = BigDecimal("2E5555")
    @neg_int = BigDecimal("-2E5555")
    @pos_frac = BigDecimal("2E-9999")
    @neg_frac = BigDecimal("-2E-9999")
  end

  it "returns the greatest integer smaller or equal to self" do
    @pos_int.floor.should == @pos_int
    @neg_int.floor.should == @neg_int
    @pos_frac.floor.should == @zero
    @neg_frac.floor.should == BigDecimal("-1")
  end

  it "returns n digits right of the decimal point if given n > 0" do
    @mixed.floor(1).should == BigDecimal("1.2")
    @mixed.floor(5).should == BigDecimal("1.23456")
  end

  it "sets n digits left of the decimal point to 0, if given n < 0" do
    BigDecimal("13345.234").ceil(-2).should == BigDecimal("13400.0")
    @mixed_big.floor(-99).should == BigDecimal("0.12E101")
    @mixed_big.floor(-100).should == BigDecimal("0.1E101")
    @mixed_big.floor(-95).should == BigDecimal("0.123456E101")
  end

end