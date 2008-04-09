require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#precs" do

  before(:each) do
    @arr = [BigDecimal("2E40001"), BigDecimal("3E-20001"),\
            BigDecimal("Infinity"), BigDecimal("NaN")]
  end

  it "returns array of two values" do
    @arr.each do |x|
      x.precs.kind_of?(Array).should == true
      x.precs.size.should == 2
    end
  end

  it "returns Integers as array values" do
    @arr.each do |x|
      x.precs[0].kind_of?(Integer).should == true
      x.precs[1].kind_of?(Integer).should == true
    end
  end

  it "returns the current value of signifikant digits as first value" do
    BigDecimal("3.14159").precs[0].should >= 6
  end

end

