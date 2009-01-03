require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#eql?" do
  it "returns true if other is a Float equal to self" do
    1.0.should_not eql(1)
    0.0.should eql(0.0)
  end
end
