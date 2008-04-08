require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#to_f" do

  it "returns number of type float" do
    BigDecimal("3.14159").to_f.class.should == Float
  end

  it "Floating point rounding occurs" do
    bigdec = BigDecimal("3.141592653589793238462643383279502884197169399375")
    bigdec.to_f.should be_close(3.14159265358979, TOLERANCE)
  end

end

