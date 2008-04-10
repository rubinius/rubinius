require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#nonzero?" do

  it "returns self if self doesn't equal zero" do
    # documentation says, it returns true. (04/10/08)
    e2_plus = BigDecimal("2E40001")
    e3_minus = BigDecimal("3E-20001")
    infinity = BigDecimal("Infinity")
    nan = BigDecimal("NaN")
    infinity.nonzero?.should == infinity
    nan.nonzero?.to_s.should == 'NaN'
    # NaN never equals, not even it self.
    e3_minus.nonzero?.should == e3_minus
    e2_plus.nonzero?.should == e2_plus
  end

  it "returns nil otherwise" do
    # documentation states, it should return false. (04/10/08)
    really_small_zero = BigDecimal("0E-200000000")
    really_big_zero = BigDecimal("0E200000000000")
    really_small_zero.nonzero?.should == nil
    really_big_zero.nonzero?.should == nil
    BigDecimal("0.000000000000000000000000").nonzero?.should == nil
  end

end
