require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#divmod" do

  before(:each) do
    @a = BigDecimal("42.00000000000000000001")
  end


  it "divides value, returns an array" do
    res = @a.divmod(5)
    res.kind_of?(Array).should == true
  end

  it "array contains quotient and modulus as BigDecimal" do
    res = @a.divmod(5)
    res.each do |x|
      x.kind_of?(BigDecimal).should == true
    end
    res[0].should == BigDecimal('0.8E1')
    res[1].should == BigDecimal('2.00000000000000000001')
  end

  it "Can be reversed with * and +" do
    # Example taken from BigDecimal documentation
    a = BigDecimal.new("42")
    b = BigDecimal.new("9")
    q,m = a.divmod(b)
    c = q * b + m
    a.should == c
  end


end

