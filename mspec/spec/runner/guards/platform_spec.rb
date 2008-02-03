require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/guards/platform'

describe MSpec, ".wordsize?" do
  before :each do
    @guard = PlatformGuard.new
  end
  
  it "returns true when arg is 32 and 1.size is 4" do
    @guard.wordsize?(32).should == (1.size == 4)
  end
  
  it "returns true when arg is 64 and 1.size is 8" do
    @guard.wordsize?(64).should == (1.size == 8)
  end
end


describe Object, "#platform_is" do
  before :each do
    @guard = PlatformGuard.new
    PlatformGuard.stub!(:new).and_return(@guard)
  end

  it "does not yield when #platform? returns false" do
    @guard.stub!(:platform?).and_return(false)
    platform_is(:ruby) { @record = :yield }
    @record.should_not == :yield
  end
  
  it "yields when #platform? returns true" do
    @guard.stub!(:platform?).and_return(true)
    platform_is(:solarce) { @record = :yield }
    @record.should == :yield
  end
end

describe Object, "#platform_is_not" do
  before :each do
    @guard = PlatformGuard.new
    PlatformGuard.stub!(:new).and_return(@guard)
  end
  
  it "does not yield when #platform? returns true" do
    @guard.stub!(:platform?).and_return(true)
    platform_is_not(:ruby) { @record = :yield }
    @record.should_not == :yield
  end
  
  it "yields when #platform? returns false" do
    @guard.stub!(:platform?).and_return(false)
    platform_is_not(:solarce) { @record = :yield }
    @record.should == :yield
  end
end

describe Object, "#platform_is :wordsize => SIZE_SPEC" do
  before :each do
    @guard = PlatformGuard.new :darwin, :wordsize => 32
    @guard.stub!(:platform?).and_return(true)
    PlatformGuard.stub!(:new).and_return(@guard)
  end
  
  it "yields when #wordsize? returns true" do
    @guard.stub!(:wordsize?).and_return(true)
    platform_is(:wordsize => 32) { @record = :yield }
    @record.should == :yield
  end
  
  it "doesn not yield when #wordsize? returns false" do
    @guard.stub!(:wordsize?).and_return(false)
    platform_is(:wordsize => 32) { @record = :yield }
    @record.should_not == :yield
  end
end

describe Object, "#platform_is_not :wordsize => SIZE_SPEC" do
  before :each do
    @guard = PlatformGuard.new :darwin, :wordsize => 32
    @guard.stub!(:platform?).and_return(true)
    PlatformGuard.stub!(:new).and_return(@guard)
  end
  
  it "yields when #wordsize? returns false" do
    @guard.stub!(:wordsize?).and_return(false)
    platform_is_not(:wordsize => 32) { @record = :yield }
    @record.should == :yield
  end
  
  it "doesn not yield when #wordsize? returns true" do
    @guard.stub!(:wordsize?).and_return(true)
    platform_is_not(:wordsize => 32) { @record = :yield }
    @record.should_not == :yield
  end
end
