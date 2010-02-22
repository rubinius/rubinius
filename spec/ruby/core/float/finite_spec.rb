require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#finite?" do
  it "returns true if a valid IEEE floating-point number" do
    (1.5**0xffffffff).finite?.should == false
    3.14159.finite?.should == true
    (-1.0/0.0).finite?.should == false
  end
end
