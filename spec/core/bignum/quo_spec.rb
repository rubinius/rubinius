require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#quo" do
  it "returns the floating-point result of self divided by other" do
    a = BignumHelper.sbm(3)
    a.quo(2.5).should_be_close(429496730.8, TOLERANCE)
    a.quo(13).should_be_close(82595525.1538462, TOLERANCE)
    a.quo(BignumHelper.sbm).should_be_close(1.00000000279397, TOLERANCE)
  end

  it "does NOT raise an exception when other is zero" do
    # a.quo(0) should also not raise (i.e works in irb and from a file),
    # but fails here.
    a = BignumHelper.sbm(91)
    b = BignumHelper.sbm(28)
    a.quo(0.0).to_s.should == "Infinity"
    b.quo(-0.0).to_s.should == "-Infinity"
  end
end
