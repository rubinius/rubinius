require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'



describe "BigDecimal#<=>" do

  before(:each) do
    @zero = BigDecimal("0")
    @mixed = BigDecimal("1.23456789")
    @mixed_big = BigDecimal("1.23456789E100")
    @pos_int = BigDecimal("2E5555")
    @neg_int = BigDecimal("-2E5555")
    @pos_frac = BigDecimal("2E-9999")
    @neg_frac = BigDecimal("-2E-9999")
  end


  it "returns 0 if a == b" do
    (@pos_int <=> @pos_int).should == 0
    (@neg_int <=> @neg_int).should == 0
    (@pos_frac <=> @pos_frac).should == 0
    (@neg_frac <=> @neg_frac).should == 0
    (@zero <=> @zero).should == 0
  end

  it "returns 1 if a > b" do
    (@pos_int <=> @neg_int).should == 1
    (@pos_frac <=> @neg_frac).should == 1
    (@pos_frac <=> @zero).should == 1
  end

  it "returns -1 if a < b" do
    (@zero <=> @pos_frac).should == -1
    (@neg_int <=> @pos_frac).should == -1
    (@pos_frac <=> @pos_int).should == -1
  end

end