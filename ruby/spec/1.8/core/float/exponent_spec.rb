require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#**" do
  it "returns self raise to the other power" do
    (2.3 ** 3).should be_close(12.167,TOLERANCE)
    (5.2 ** -1).should be_close(0.192307692307692,TOLERANCE)
    (9.5 ** 0.5).should be_close(3.08220700148449, TOLERANCE) 
    (9.5 ** 0xffffffff).to_s.should == 'Infinity'
  end
end
