require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/conflict'

describe Object, "#conflicts_with" do
  before :each do
    ScratchPad.clear
  end

  it "does not yield if Object.constants includes any of the arguments" do
    Object.stub!(:constants).and_return(["SomeClass", "OtherClass"])
    conflicts_with(:SomeClass, :AClass, :BClass) { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "yields if Object.constants does not include any of the arguments" do
    Object.stub!(:constants).and_return(["SomeClass", "OtherClass"])
    conflicts_with(:AClass, :BClass) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end
end
