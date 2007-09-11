require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#included_modules" do
  it "returns a list of modules included in self" do
    ModuleSpecs.included_modules.should              == []
    ModuleSpecs::Child.included_modules.should       == [ModuleSpecs::SuperModule, ModuleSpecs::BasicModule, Kernel]
    ModuleSpecs::Parent.included_modules.should      == [Kernel]
    ModuleSpecs::BasicModule.included_modules.should == []
    ModuleSpecs::SuperModule.included_modules.should == [ModuleSpecs::BasicModule]
  end
end
