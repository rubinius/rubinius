require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#>=" do
  it "returns true if self is a superclass of, the same as or included by given module" do
    (ModuleSpecs::Parent      >= ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule >= ModuleSpecs::Child).should == true
    (ModuleSpecs::SuperModule >= ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule >= ModuleSpecs::SuperModule).should == true
    (ModuleSpecs::Child       >= ModuleSpecs::Child).should == true
    (ModuleSpecs::Parent      >= ModuleSpecs::Parent).should == true
    (ModuleSpecs::BasicModule >= ModuleSpecs::BasicModule).should == true
    (ModuleSpecs::SuperModule >= ModuleSpecs::SuperModule).should == true
  end
  
  it "returns nil if self is not related to the given module" do
    (ModuleSpecs::Parent      >= ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      >= ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule >= ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule >= ModuleSpecs::Parent).should == nil
  end
  
  it "returns false if self is a subclass of or includes the given module" do
    (ModuleSpecs::Child       >= ModuleSpecs::Parent).should == false
    (ModuleSpecs::Child       >= ModuleSpecs::BasicModule).should == false
    (ModuleSpecs::Child       >= ModuleSpecs::SuperModule).should == false
    (ModuleSpecs::SuperModule >= ModuleSpecs::BasicModule).should == false
  end

  it "raises a TypeError if the argument is no class/module" do
    should_raise(TypeError, "compared with non class/module") do
      ModuleSpecs::Parent     >= Object.new
    end
  end
end
