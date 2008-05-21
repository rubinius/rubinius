require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#add" do

  before(:each) do
    @one = BigDecimal("1")
    @zero = BigDecimal("0")
    @two = BigDecimal("2")
    @three = BigDecimal("3")
    @ten = BigDecimal("10")
    @eleven = BigDecimal("11")
    @nan = BigDecimal("NaN")
    @infinity = BigDecimal("Infinity")
    @infinity_minus = BigDecimal("-Infinity")
    @one_minus = BigDecimal("-1")
    @frac_1 = BigDecimal("1E-99999")
    @frac_2 = BigDecimal("0.9E-99999")
  end

  it "returns a + b with given precision" do
    # documentation states, that precision ist optional, but it ain't,
    @two.add(@one, 1).should == @three
    @one .add(@two, 1).should == @three
    @one.add(@one_minus, 1).should == @zero
    @ten.add(@one, 2).should == @eleven
    @zero.add(@one, 1).should == @one
    @frac_2.add(@frac_1, 10000).should == BigDecimal("1.9E-99999")
    @frac_1.add(@frac_1, 10000).should == BigDecimal("2E-99999")
  end

  it "returns NaN if NaN is involved" do
    @one.add(@nan, 10000).nan?.should == true
    @nan.add(@one, 1).nan?.should == true
  end

  it "returns Infinity or -Infinity if these are involved" do
    @zero.add(@infinity, 1).should == @infinity
    @frac_2.add(@infinity, 1).should == @infinity
    @one_minus.add(@infinity, 1).should == @infinity
    @two.add(@infinity, 1).should == @infinity

    @zero.add(@infinity_minus, 1).should == @infinity_minus
    @frac_2.add(@infinity_minus, 1).should == @infinity_minus
    @one_minus.add(@infinity_minus, 1).should == @infinity_minus
    @two.add(@infinity_minus, 1).should == @infinity_minus

    @infinity.add(@zero, 1).should == @infinity
    @infinity.add(@frac_2, 1).should == @infinity
    @infinity.add(@one_minus, 1).should == @infinity
    @infinity.add(@two, 1).should == @infinity

    @infinity_minus.add(@zero, 1).should == @infinity_minus
    @infinity_minus.add(@frac_2, 1).should == @infinity_minus
    @infinity_minus.add(@one_minus, 1).should == @infinity_minus
    @infinity_minus.add(@two, 1).should == @infinity_minus

    @infinity.add(@infinity, 10000).should == @infinity
    @infinity_minus.add(@infinity_minus, 10000).should == @infinity_minus
  end

  it "returns NaN if Infinity + (- Infinity)" do
    @infinity.add(@infinity_minus, 10000).nan?.should == true
    @infinity_minus.add(@infinity, 10000).nan?.should == true
  end

end
