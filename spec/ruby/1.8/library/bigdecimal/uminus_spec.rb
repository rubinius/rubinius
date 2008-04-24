require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#-@" do

  it "negates self" do
    BigDecimal("2.221").send(:-@).should == BigDecimal("-2.221")
    BigDecimal("2E10000").send(:-@).should == BigDecimal("-2E10000")
    some_number = BigDecimal("2455999221.5512")
    some_number_neg = BigDecimal("-2455999221.5512")
    some_number.send(:-@).should == some_number_neg
    (-BigDecimal("-5.5")).should == BigDecimal("5.5")
    another_number = BigDecimal("-8.551551551551551551")
    another_number_pos = BigDecimal("8.551551551551551551")
    another_number.send(:-@).should == another_number_pos
  end

end