require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#infinite?" do
  it "returns nil, -1, +1 when self is finite, -Infinity, +Infinity" do
    1.0.infinite?.should == nil
    infinity_value.infinite?.should == 1
    (-infinity_value).infinite?.should == -1
  end
end
