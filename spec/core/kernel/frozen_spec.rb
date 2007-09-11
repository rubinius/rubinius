require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#frozen?" do
  it "returns true if self is frozen" do
    o = Object.new
    p = Object.new
    p.freeze
    o.frozen?.should == false
    p.frozen?.should == true
  end
end
