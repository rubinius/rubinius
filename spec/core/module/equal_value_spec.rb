require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@module_equal = shared "Module#==" do |cmd|
  describe "Module##{cmd}(other)" do
    it "returns true if self and the given module are the same" do
      ModuleSpecs.send(cmd, ModuleSpecs).should == true
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Child).should == true
      ModuleSpecs::Parent.send(cmd, ModuleSpecs::Parent).should == true
      ModuleSpecs::Basic.send(cmd, ModuleSpecs::Basic).should == true
      ModuleSpecs::Super.send(cmd, ModuleSpecs::Super).should == true
      
      ModuleSpecs::Child.send(cmd, ModuleSpecs).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Parent).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Basic).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Super).should == false
    end
  end
end

describe "Module#==" do
  it_behaves_like(@module_equal, :==)
end
