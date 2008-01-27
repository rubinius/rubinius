require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#unbind" do
  before(:each) do
    @normal = MethodSpecs::Methods.new
    @normal_m = @normal.method :foo
    @normal_um = @normal_m.unbind
    @pop_um = MethodSpecs::MySub.new.method(:bar).unbind
  end

  it "returns an UnboundMethod" do
    @normal_um.class.should == UnboundMethod
  end

  it "gives UnboundMethod method name, Module defined in and Module extracted from" do
    @pop_um.inspect.should =~ /\bbar\b/
    @pop_um.inspect.should =~ /\bMethodSpecs::MyMod\b/
    @pop_um.inspect.should =~ /\bMethodSpecs::MySub\b/
  end

  specify "rebinding UnboundMethod to Method's obj produces exactly equivalent Methods" do
    @normal_um.bind(@normal).should == @normal_m
    @normal_m.should == @normal_um.bind(@normal)
  end
end
