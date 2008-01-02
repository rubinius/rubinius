require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#-" do
  it "returns self minus other" do
    (9_237_212.5280 - 5_280).should be_close(9231932.528, TOLERANCE)
    (2_560_496.1691 - 0xfffffffff).should be_close(-68716916238.8309, TOLERANCE)
    (5.5 - 5.5).should be_close(0.0,TOLERANCE)
  end
end
