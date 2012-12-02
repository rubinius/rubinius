require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Module#ancestors" do
  it "returns a list of modules included in self (including self)" do
    ModuleSpecs.ancestors.should         include(ModuleSpecs)
    ModuleSpecs::Basic.ancestors.should  include(ModuleSpecs::Basic)
    ModuleSpecs::Super.ancestors.should  include(ModuleSpecs::Super, ModuleSpecs::Basic)
    ModuleSpecs::Parent.ancestors.should include(ModuleSpecs::Parent, Object, Kernel)
    ModuleSpecs::Child.ancestors.should  include(ModuleSpecs::Child, ModuleSpecs::Super, ModuleSpecs::Basic, ModuleSpecs::Parent, Object, Kernel)
  end

  it "returns only modules and classes" do
    class << ModuleSpecs::Child; self; end.ancestors.should include(ModuleSpecs::Internal, Class, Module, Object, Kernel)
  end

  it "has 1 entry per module or class" do
    ModuleSpecs::Parent.ancestors.should == ModuleSpecs::Parent.ancestors.uniq
  end

  ruby_version_is "2.0" do
    ModuleSpecs::PrependModules::C1.ancestors.should include(
      ModuleSpecs::PrependModules::M2, ModuleSpecs::PrependModules::M3,
      ModuleSpecs::PrependModules::C1, ModuleSpecs::PrependModules::M4,
      ModuleSpecs::PrependModules::M1, ModuleSpecs::PrependModules::C0,
      ModuleSpecs::PrependModules::M0, ModuleSpecs::PrependModules::C,
      Object, Kernel, BasicObject
    )
  end
end
