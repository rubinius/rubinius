require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#floor" do
  it "returns the largest Integer less than or equal to self" do
    -1.2.floor.should == -2
    -1.0.floor.should == -1
    0.0.floor.should == 0
    1.0.floor.should == 1
    5.9.floor.should == 5
    -9223372036854775808.1.floor.should == -9223372036854775808
    9223372036854775808.1.floor.should == 9223372036854775808
  end
end
