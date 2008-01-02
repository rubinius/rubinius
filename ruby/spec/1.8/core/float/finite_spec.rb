require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#finite?" do
  it "returns true if a valid IEEE floating-point number" do
    (1.5**0xffffffff).finite?.should == false
    3.14159.finite?.should == true
    (-1.0/0.0).finite?.should == false
  end
end
