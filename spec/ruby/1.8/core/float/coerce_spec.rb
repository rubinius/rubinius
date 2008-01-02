require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#coerce" do
  it "returns [other, self] both as Floats" do
    1.2.coerce(1).inspect.should == '[1.0, 1.2]'
    -0.0.coerce(0xffffffff).inspect.should == '[4294967295.0, -0.0]'
    5.28.coerce(1.0).inspect.should == '[1.0, 5.28]'
    1.0.coerce(1).inspect.should == '[1.0, 1.0]'
    1.0.coerce(0xffffffff).inspect.should == '[4294967295.0, 1.0]'
    1.0.coerce("2.5").inspect.should == '[2.5, 1.0]'
    1.0.coerce(3.14).inspect.should == '[3.14, 1.0]'
  end
end
