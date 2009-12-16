require File.dirname(__FILE__) + '/../../spec_helper'

describe "GC#garbage_collect" do

  before :each do
    @obj = Object.new
    @obj.extend(GC)
  end

  it "can be invoked without any exceptions" do
    lambda { @obj.garbage_collect }.should_not raise_error
  end

  it "doesn't accept any arguments" do
    lambda { @obj.garbage_collect(1) }.should raise_error(ArgumentError)
  end

  it "ignores the supplied block" do
    lambda { @obj.garbage_collect {} }.should_not raise_error
  end

  it "always returns nil" do
    @obj.garbage_collect.should == nil
    @obj.garbage_collect.should == nil
  end

end
