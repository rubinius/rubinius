require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#round" do
  it "returns the nearest Integer" do
    5.5.round.should == 6
    0.4.round.should == 0
    -2.8.round.should == -3
    0.0.round.should == 0
  end
end
