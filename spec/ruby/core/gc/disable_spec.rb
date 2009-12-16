require File.dirname(__FILE__) + '/../../spec_helper'

describe "GC.disable" do
  after :each do
    GC.enable
  end

  it "can be invoked without any exceptions" do
    lambda { GC.disable }.should_not raise_error
  end

  it "doesn't accept any arguments" do
    lambda { GC.disable(1) }.should raise_error(ArgumentError)
  end

  it "ignores the supplied block" do
    lambda { GC.disable {} }.should_not raise_error
  end

  it "returns true iff the garbage collection was previously disabled" do
    GC.disable.should == false
    GC.disable.should == true
    GC.disable.should == true
    GC.enable
    GC.disable.should == false
    GC.disable.should == true
  end

end
