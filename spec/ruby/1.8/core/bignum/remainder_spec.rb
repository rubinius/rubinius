require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#remainder" do
  it "returns the remainder of dividing self by other" do
    a = BignumHelper.sbm(79)
    a.remainder(2).should == 1
    a.remainder(97.345).should be_close(46.5674996147722, TOLERANCE)
    a.remainder(BignumHelper.sbm).should == 79
  end
  
  it "raises a ZeroDivisionError if other is zero and not a Float" do
    lambda { BignumHelper.sbm(66).remainder(0) }.should raise_error(ZeroDivisionError)
  end
  
  it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
    a = BignumHelper.sbm(7)
    b = BignumHelper.sbm(32)
    a.remainder(0.0).to_s.should == 'NaN'
    b.remainder(-0.0).to_s.should == 'NaN'
  end
end
