require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#add" do

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

  it "returns a + b with given precision" do
    # documentation states, that precision ist optional, but it ain't,
    @two.add(@one, 1).should == @three
    @one .add(@two, 1).should == @three
    @one.add(@one_minus, 1).should == @zero
    @frac_2.add(@frac_1, 10000).should == BigDecimal("1.9E-99999")
    @frac_1.add(@frac_1, 10000).should == BigDecimal("2E-99999")
  end

  it "returns NaN if NaN is involved" do
    @one.add(@nan, 10000).to_s.should == 'NaN'
    @nan.add(@one, 1).to_s.should == 'NaN'
  end

  it "returns Infinity or -Infinity if these are involved" do
    @zero.add(@infinity, 1).should == @infinity
    @frac_2.add(@infinity, 1).should == @infinity
    @two.add(@infinity_minus,1).should == @infinity_minus
  end

  it "returns NaN if Infinity + (- Infinity)" do
    @infinity.add(@infinity_minus, 10000).to_s.should == "NaN"
    # to_s needed because BigDecimal("NaN") never equals its self.
  end

end
