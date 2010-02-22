require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#ceil" do
  it "returns the smallest Integer greater than or equal to self" do
    -1.2.ceil.should == -1
    -1.0.ceil.should == -1
    0.0.ceil.should == 0
    1.3.ceil.should == 2
    3.0.ceil.should == 3
    -9223372036854775808.1.ceil.should == -9223372036854775808
    9223372036854775808.1.ceil.should == 9223372036854775808
  end
end
