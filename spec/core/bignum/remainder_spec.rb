require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#remainder" do
  it "returns the remainder of dividing self by other" do
    a = BignumHelper.sbm(79)
    a.remainder(2).should == 1
    a.remainder(97.345).should_be_close(75.16000001254, TOLERANCE)
    a.remainder(BignumHelper.sbm).should == 79
  end
  
  it "raises ZeroDivisionError if other is zero and not a Float" do
    should_raise(ZeroDivisionError) { BignumHelper.sbm(66).remainder(0) }
  end
  
  it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
    a = BignumHelper.sbm(7)
    b = BignumHelper.sbm(32)
    a.remainder(0.0).to_s.should == 'NaN'
    b.remainder(-0.0).to_s.should == 'NaN'
  end
end
