require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#>" do
  it "returns true if self is greater than other" do
    (1.5 > 1).should == true
    (2.5 > 3).should == false
    (45.91 > bignum_value).should == false
  end
end
