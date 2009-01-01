require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#>=" do
  it "returns true if self is greater than or equal to other" do
    (5.2 >= 5.2).should == true
    (9.71 >= 1).should == true
    (5.55382 >= 0xfabdafbafcab).should == false
  end
end
