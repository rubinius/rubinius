require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#taint" do
  it "sets self to be tainted" do
    Object.new.taint.tainted?.should == true
  end
  
  it "has no effect on immediate values" do
    a = nil
    b = true
    c = false
    d = 1
    a.tainted?.should == false
    b.tainted?.should == false
    c.tainted?.should == false
    d.tainted?.should == false
  end
end
