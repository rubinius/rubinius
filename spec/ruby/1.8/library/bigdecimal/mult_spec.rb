require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#mult" do
  before :each do
    @zero = BigDecimal "0"
    @zero_pos = BigDecimal "+0"
    @zero_neg = BigDecimal "-0"

    @one = BigDecimal "1"
    @mixed = BigDecimal "1.23456789"
    @pos_int = BigDecimal "2E5555"
    @neg_int = BigDecimal "-2E5555"
    @pos_frac = BigDecimal "2E-9999"
    @neg_frac = BigDecimal "-2E-9999"
    @nan = BigDecimal "NaN"
    @infinity = BigDecimal "Infinity"
    @infinity_minus = BigDecimal "-Infinity"
    @one_minus = BigDecimal "-1"
    @frac_1 = BigDecimal "1E-99999"
    @frac_2 = BigDecimal "0.9E-99999"

    @e3_minus = BigDecimal "3E-20001"
    @e = BigDecimal "1.00000000000000000000123456789"
    @one = BigDecimal "1"
    @tolerance = @e.sub @one, 1000
    @tolerance2 = BigDecimal "30001E-20005"

    @special_vals = [@infinity, @infinity_minus, @nan]
    @regular_vals = [ @one, @mixed, @pos_int, @neg_int,
                      @pos_frac, @neg_frac, @one_minus,
                      @frac_1, @frac_2
                    ]
    @zeroes = [@zero, @zero_pos, @zero_neg]
  end

  it "multiply self with other with (optional) precision" do
    @e.mult(@one, 1).should be_close(@one, @tolerance)
    @e3_minus.mult(@one, 1).should be_close(0, @tolerance2)
  end

  it "returns NaN if NaN is involved" do
    values = @regular_vals + @zeroes

    values.each do |val|
      @nan.mult(val, 10).nan?.should == true
      val.mult(@nan, 10).nan?.should == true
    end
  end

  it "returns zero if self or argument is zero" do
    values = @regular_vals + @zeroes

    values.each do |val|
      @zeroes.each do |zero|
        zero.mult(val, 10).should == 0
        val.mult(zero, 10).should == 0
      end
    end
  end

  it "returns infinite value if self or argument is infinite" do
    values = @regular_vals
    infs = [@infinity, @infinity_minus]

    values.each do |val|
      infs.each do |inf|
        inf.mult(val, 10).finite?.should == false
        val.mult(inf, 10).finite?.should == false
      end
    end

    @infinity.mult(@infinity, 10).infinite?.should == 1
    @infinity_minus.mult(@infinity_minus, 10).infinite?.should == 1
    @infinity.mult(@infinity_minus, 10).infinite?.should == -1
    @infinity_minus.mult(@infinity, 10).infinite?.should == -1
    @infinity.mult(@one, 10).infinite?.should == 1
    @infinity_minus.mult(@one, 10).infinite?.should == -1
  end

  it "returns NaN if the result is undefined" do
    @zero.mult(@infinity, 10).nan?.should == true
    @zero.mult(@infinity_minus, 10).nan?.should == true
    @infinity.mult(@zero, 10).nan?.should == true
    @infinity_minus.mult(@zero, 10).nan?.should == true
  end

end
