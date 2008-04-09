require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'


describe "BigDecimal#remainder" do

  before(:each) do
    @zero = BigDecimal("0")
    @mixed = BigDecimal("1.23456789")
    @pos_int = BigDecimal("2E5555")
    @neg_int = BigDecimal("-2E5555")
    @pos_frac = BigDecimal("2E-9999")
    @neg_frac = BigDecimal("-2E-9999")
    @nan = BigDecimal("NaN")
  end

  it "if both values are of same sign it equals modulo" do
    @mixed.remainder(@pos_frac).should == @mixed % @pos_frac
    @pos_int.remainder(@pos_frac).should == @pos_int % @pos_frac
    @neg_frac.remainder(@neg_int).should == @neg_frac % @neg_int
    @neg_int.remainder(@neg_frac).should == @neg_int % @neg_frac
  end

  it "Otherwise, it is the modulus minus the value divided by" do
    @mixed.remainder(@neg_frac).should == (@mixed % @neg_frac) * -1
    @pos_int.remainder(@neg_frac).should == (@pos_int % @neg_frac) * -1
    @neg_frac.remainder(@pos_int).should == @neg_frac % (@pos_int * -1)
    @neg_int.remainder(@pos_frac).should == (@neg_int % @pos_frac) * -1
  end

  it "returns NaN used with zero" do
    @mixed.remainder(@zero).to_s.should == 'NaN'
    @zero.remainder(@zero).to_s.should == 'NaN'
    # to_s because: NaN isn't even equal to its self.
  end

  it "returns zero if used on zero" do
    @zero.remainder(@mixed).should == @zero
  end

end