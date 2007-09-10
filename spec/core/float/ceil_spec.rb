require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#ceil" do
  it "returns the smallest Integer greater than or equal to self" do
    -1.2.ceil.should == -1
    -1.0.ceil.should == -1
    0.0.ceil.should == 0
    1.3.ceil.should == 2
    3.0.ceil.should == 3
  end
end
