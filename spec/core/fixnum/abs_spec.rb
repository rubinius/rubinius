require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#abs" do
  it "returns the absolute value" do
    0.abs.should == 0
    -2.abs.should == 2
    5.abs.should == 5
  end
end
