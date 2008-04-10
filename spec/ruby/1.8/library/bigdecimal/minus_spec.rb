require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#-" do

  before(:each) do
    @one = BigDecimal("1")
    @zero = BigDecimal("0")
    @two = BigDecimal("2")
    @nan = BigDecimal("NaN")
    @infinity = BigDecimal("Infinity")
    @infinity_minus = BigDecimal("-Infinity")
    @one_minus = BigDecimal("-1")
    @frac_1 = BigDecimal("1E-99999")
    @frac_2 = BigDecimal("0.9E-99999")
  end

  it "returns a - b" do

    (@two - @one).should == @one
    (@one - @two).should == @one_minus
    (@one - @one_minus).should == @two
    (@frac_2 - @frac_1).should == BigDecimal("-0.1E-99999")
    (@two - @two).should == @zero
    (@frac_1 - @frac_1).should == @zero
  end

  it "returns NaN if NaN is involved" do
    (@one - @nan).to_s.should == 'NaN'
    (@nan - @one).to_s.should == 'NaN'
  end

  it "returns Infinity or -Infinity if these are involved" do
    (@infinity - @infinity_minus).should == @infinity
    (@zero - @infinity).should == @infinity_minus
    (@frac_2 - @infinity).should == @infinity_minus
    (@two - @infinity).should == @infinity_minus
  end

end
