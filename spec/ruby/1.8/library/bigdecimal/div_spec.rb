require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#div" do

  before(:each) do
    @one = BigDecimal("1")
    @zero = BigDecimal("0")
    @two = BigDecimal("2")
    @three = BigDecimal("3")
    @nan = BigDecimal("NaN")
    @infinity = BigDecimal("Infinity")
    @infinity_minus = BigDecimal("-Infinity")
    @one_minus = BigDecimal("-1")
    @frac_1 = BigDecimal("1E-99999")
    @frac_2 = BigDecimal("0.9E-99999")
  end

  it "returns a / b with optional precision" do
    @two.div(@one).should == @two
    @one.div(@two).should == @zero
    # ^^ is this really intended for a class with arbitrary precision?
    @one.div(@two, 1).should == BigDecimal("0.5")
    @one.div(@one_minus).should == @one_minus
    @one_minus.div(@one_minus).should == @one
    @frac_2.div(@frac_1, 1).should == BigDecimal("0.9")
    @frac_1.div(@frac_1).should == @one
  end

  it "returns NaN if NaN is involved" do
    @one.div(@nan).nan?.should == true
    @nan.div(@one).nan?.should == true
  end

  it "returns NaN if divided by Infinity and no precision given" do
    @zero.div(@infinity).nan?.should == true
    @frac_2.div(@infinity).nan?.should == true
  end

  it "returns 0 if divided by Infinity with given precision" do
    @zero.div(@infinity, 0).should == 0
    @frac_2.div(@infinity, 1).should == 0
    @zero.div(@infinity, 100000).should == 0
    @frac_2.div(@infinity, 100000).should == 0
  end

  it "returns NaN if (+|-) Infinity divided by 1 and no precision given" do
    @infinity_minus.div(@one).nan?.should == true
    @infinity.div(@one).nan?.should == true
    @infinity_minus.div(@one_minus).nan?.should == true
  end

  it "returns (+|-)Infinity if (+|-)Infinity by 1 and precision given" do
    @infinity_minus.div(@one, 0).should == @infinity_minus
    @infinity.div(@one, 0).should == @infinity
    @infinity_minus.div(@one_minus, 0).should == @infinity
  end

  it "returns NaN if Infinity / ((+|-) Infinity)" do
    @infinity.div(@infinity_minus, 100000).nan?.should == true
    @infinity_minus.div(@infinity, 1).nan?.should == true
  end


end
