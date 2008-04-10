require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#*" do
  before(:each) do
    @e3_minus = BigDecimal("3E-20001")
    @e3_plus = BigDecimal("3E20001")
    @e = BigDecimal("1.00000000000000000000123456789")
    @one = BigDecimal("1")
  end

  it "multiply self with other" do
    (@e3_minus * @e3_plus).should == BigDecimal("9")
    (@e3_minus * @e3_minus).should == @e3_minus ** 2
    (@e * @one).should == @e
  end
end
