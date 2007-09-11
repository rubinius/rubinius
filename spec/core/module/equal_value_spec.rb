require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@module_equal = shared "Module#==" do |cmd|
  describe "Module##{cmd}(other)" do
    it "returns true if self and the given module are the same" do
      ModuleSpecs.send(cmd, ModuleSpecs).should == true
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Child).should == true
      ModuleSpecs::Parent.send(cmd, ModuleSpecs::Parent).should == true
      ModuleSpecs::BasicModule.send(cmd, ModuleSpecs::BasicModule).should == true
      ModuleSpecs::SuperModule.send(cmd, ModuleSpecs::SuperModule).should == true
      
      ModuleSpecs::Child.send(cmd, ModuleSpecs).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Parent).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::BasicModule).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::SuperModule).should == false
    end
  end
end

describe "Module#==" do
  it_behaves_like(@module_equal, :==)
end
