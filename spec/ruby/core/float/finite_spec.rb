require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#finite?" do
  it "returns true if a valid IEEE floating-point number" do
    (1.5**0xffffffff).finite?.should == false
    3.14159.finite?.should == true
    (-infinity_value).finite?.should == false
  end
end
