require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#infinite?" do

  it "returns 1 if self is Infinitu" do
    # documentation says returns true.
    BigDecimal("Infinity").infinite?.should == 1
    # nan = BigDecimal("NaN")
    # nan.infinite?.should == true
    # This fails.
    # Is NaN really finite?
  end
  
  it "returns -1 if self is -Infinity" do
    # This is how MRI behaves.
    BigDecimal("-Infinity").infinite?.should == -1
  end

  it "returns not true otherwise" do
    # If nil or false is not really specified in documentation.
    e2_plus = BigDecimal("2E40001")
    e3_minus = BigDecimal("3E-20001")
    really_small_zero = BigDecimal("0E-200000000")
    really_big_zero = BigDecimal("0E200000000000")
    e3_minus.infinite?.should == nil
    e2_plus.infinite?.should == nil
    really_small_zero.infinite?.should == nil
    really_big_zero.infinite?.should == nil
    BigDecimal("0.000000000000000000000000").infinite?.should == nil
  end

end