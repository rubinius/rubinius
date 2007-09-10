require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#+" do
  it "returns self plus other" do
    (491 + 2).should == 493
    (90210 + 10).should == 90220
  end
  
  it "coerces fixnum and return self plus other" do
    (9 + 0xffffffff).should == 4294967304
    (1001 + 5.219).to_s.should == '1006.219'
  end
end
