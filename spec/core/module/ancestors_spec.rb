require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#ancestors" do
  it "returns a list of modules included in self (including self)" do
    ModuleSpecs.ancestors.should == [ModuleSpecs]
    ModuleSpecs::BasicModule.ancestors.should == [ModuleSpecs::BasicModule]
    ModuleSpecs::SuperModule.ancestors.should == [ModuleSpecs::SuperModule, ModuleSpecs::BasicModule]
    ModuleSpecs::Parent.ancestors.should == [ModuleSpecs::Parent, Object, Kernel]
    ModuleSpecs::Child.ancestors.should == [ModuleSpecs::Child, ModuleSpecs::SuperModule, ModuleSpecs::BasicModule, ModuleSpecs::Parent, Object, Kernel]
  end
end
