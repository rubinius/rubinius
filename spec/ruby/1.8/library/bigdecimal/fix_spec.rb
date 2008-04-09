require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'


describe "BigDecimal#fix" do

  it "returns a BigDecimal" do
    BigDecimal("2E100000000").fix.kind_of?(BigDecimal).should == true
    BigDecimal("2E-999").kind_of?(BigDecimal).should == true
  end

  it "returns the integer part of the absolute value" do
    a = BigDecimal("2E1000")
    a.fix.should == a
    BigDecimal("0.123456789E5").fix.should == BigDecimal("0.12345E5")
    BigDecimal("-0.123456789E5").fix.should == BigDecimal("-0.12345E5")
  end

  it "returns 0 if the absolute value is < 1" do
    BigDecimal("0.99999").fix.should == 0
    BigDecimal("-1000000").fix.should_not == 0
    BigDecimal("0").fix.should == 0
  end

end