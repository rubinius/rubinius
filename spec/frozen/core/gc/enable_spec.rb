require File.dirname(__FILE__) + '/../../spec_helper'

describe "GC.enable" do

  it "can be invoked without any exceptions" do
    lambda { GC.enable }.should_not raise_error
  end

  it "doesn't accept any arguments" do
    lambda { GC.enable(1) }.should raise_error(ArgumentError)
  end

  it "ignores the supplied block" do
    lambda { GC.enable {} }.should_not raise_error
  end

  it "returns true iff the garbage collection was already disabled" do
    GC.enable.should == false
    GC.disable
    GC.enable.should == true
    GC.enable.should == false
  end

end
