require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#>>" do
  it "returns self shifted right other bits" do
    (7 >> 1).should == 3
    (4095 >> 3).should == 511
    (9245278 >> 1).should == 4622639
  end
  
  it "coerces other to fixnum and return self shifted right other bits" do
    (7 >> 1.5).should == 3
    (0xfff >> 3).should == 511
    (9_245_278 >> 1).should == 4622639
  end
end
