require File.dirname(__FILE__) + '/../../spec_helper'

describe "GC.start" do
  it "can be invoked without any exceptions" do
    lambda { GC.start }.should_not raise_error
  end

  it "doesn't accept any arguments" do
    lambda { GC.start(1) }.should raise_error(ArgumentError)
  end

  it "ignores the supplied block" do
    lambda { GC.start {} }.should_not raise_error
  end

  it "always returns nil" do
    GC.start.should == nil
    GC.start.should == nil
  end
end
