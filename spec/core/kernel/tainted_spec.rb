require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#tainted?" do
  it "returns true if Object is tainted" do
    o = Object.new
    p = Object.new
    p.taint
    o.tainted?.should == false
    p.tainted?.should == true
  end
end