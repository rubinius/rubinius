require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Module#prepend" do
  it "prepends module do class" do
    ModuleSpecs::ClassToPrepend.new.m.should == [:m, :c]
  end
end
