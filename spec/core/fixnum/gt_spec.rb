require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#>" do
  it "returns true if self is greater than other" do
    (-500 > -600).should == true
    (13 > 2).should == true
    (1 > 5).should == false
  end
end
