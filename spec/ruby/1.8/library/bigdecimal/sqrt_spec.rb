require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#sqrt" do

  it "returns sqrt of self with at least the given precision" do
    string = "1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415"
    BigDecimal("2").sqrt(100).to_s("F")[0, 100] == string
  end

end
