require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#>" do
  it "returns true if self is a superclass of or included by the given module" do
    (ModuleSpecs::Parent      > ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule > ModuleSpecs::Child).should == true
    (ModuleSpecs::SuperModule > ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule > ModuleSpecs::SuperModule).should == true
  end
  
  it "returns false if self is the same as the given module" do
    (ModuleSpecs::Child       > ModuleSpecs::Child).should == false
    (ModuleSpecs::Parent      > ModuleSpecs::Parent).should == false
    (ModuleSpecs::BasicModule > ModuleSpecs::BasicModule).should == false
    (ModuleSpecs::SuperModule > ModuleSpecs::SuperModule).should == false
  end
  
  it "returns nil if self is not related to the given module" do
    (ModuleSpecs::Parent      > ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      > ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule > ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule > ModuleSpecs::Parent).should == nil
  end
  
  it "raises a TypeError the argument is no class/module" do
    should_raise(TypeError, "compared with non class/module") do
      ModuleSpecs::Parent     > Object.new
    end
  end
end
