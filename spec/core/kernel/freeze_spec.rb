require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#freeze" do
  it "prevents self from being further modified" do
    module Mod; end
    o = Object.new
    o.freeze
    should_raise(TypeError) { o.extend Mod }
  end
  
  it "has no effect on immediate values" do
    a = nil
    b = true
    c = false
    d = 1
    a.freeze
    b.freeze
    c.freeze
    d.freeze
    a.frozen?.should == false
    b.frozen?.should == false
    c.frozen?.should == false
    d.frozen?.should == false
  end
end
