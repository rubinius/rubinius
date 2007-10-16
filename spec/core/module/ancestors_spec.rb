require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#ancestors" do
  it "returns a list of modules included in self (including self)" do
    ModuleSpecs.ancestors.should         == [ModuleSpecs]
    ModuleSpecs::Basic.ancestors.should  == [ModuleSpecs::Basic]
    ModuleSpecs::Super.ancestors.should  == [ModuleSpecs::Super, ModuleSpecs::Basic]
    ModuleSpecs::Parent.ancestors.should == [ModuleSpecs::Parent, Object, Kernel]
    ModuleSpecs::Child.ancestors.should  == [ModuleSpecs::Child, ModuleSpecs::Super, ModuleSpecs::Basic, ModuleSpecs::Parent, Object, Kernel]
  end
  
  it "returns only modules and classes" do
    class << ModuleSpecs::Child; self; end.ancestors.should == 
      [ModuleSpecs::Internal, Class, Module, Object, Kernel]
  end
end
