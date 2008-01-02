require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#round" do
  it "returns the nearest Integer" do
    5.5.round.should == 6
    0.4.round.should == 0
    -2.8.round.should == -3
    0.0.round.should == 0
  end
end
