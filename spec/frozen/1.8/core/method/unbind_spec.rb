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
    name = @pop_um.inspect.sub(/0x\w+/, '0xXXXXXX')
    deviates_on(:rubinius) do
      name.should == "#<UnboundMethod: MethodSpecs::MySub#bar (defined in MethodSpecs::MyMod)>"
    end
    compliant_on(:ruby, :jruby) do
      name.should == "#<UnboundMethod: MethodSpecs::MySub(MethodSpecs::MyMod)#bar>"
    end
  end

  specify "rebinding UnboundMethod to Method's obj produces exactly equivalent Methods" do
    @normal_um.bind(@normal).should == @normal_m
    @normal_m.should == @normal_um.bind(@normal)
  end
end
