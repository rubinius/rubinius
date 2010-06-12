require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.StringValue" do
  it "raises TypeError if the instance does not respond to to_str" do
    lambda { StringValue(KernelSpecs::A.new) }.should raise_error(TypeError)
  end

  it "raises TypeError for nil" do
    lambda { StringValue(nil) }.should raise_error(TypeError)
  end

  it "raises TypeError for a Fixnum" do
    lambda { StringValue(1) }.should raise_error(TypeError)
  end

  it "raises TypeError if the instance responds to to_str but doesn't return a String" do
    lambda { StringValue(KernelSpecs::B.new) }.should raise_error(TypeError)
  end

  it "calls to_str if the instance responds to it" do
    StringValue(KernelSpecs::C.new).should == "zam"
  end
end
