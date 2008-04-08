require 'bigdecimal'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "BigDecimal#finite?" do

  it "is false if Infinity or NaN" do
    BigDecimal("Infinity").finite?.should == false
    BigDecimal("NaN").finite?.should == false
  end

  it "is true otherwise" do
    BigDecimal("3E-20001").finite?.should == true
    BigDecimal("2E40001").finite?.should == true
  end
end

