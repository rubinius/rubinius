require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "UnboundMethod#super_method" do
  it "returns the method that would be called by super in the method" do
    meth = UnboundMethodSpecs::C.instance_method(:overridden)
    meth = meth.super_method
    meth.should == UnboundMethodSpecs::B.instance_method(:overridden)
    meth = meth.super_method
    meth.should == UnboundMethodSpecs::A.instance_method(:overridden)
  end
end
