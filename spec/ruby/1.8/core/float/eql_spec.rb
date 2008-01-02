require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#eql?" do
  it "returns true if other is a Float equal to self" do
    1.0.eql?(1).should == false
    0.0.eql?(0.0).should == true
  end
end
