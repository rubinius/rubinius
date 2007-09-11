require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Module#===" do
  it "returns true when the given Object is an instance of self or of self's descendants" do
    (ModuleSpecs::Child       === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::Parent      === ModuleSpecs::Parent.new).should == true
    
    (ModuleSpecs::Parent      === ModuleSpecs::Child.new).should == true
    (Object                   === ModuleSpecs::Child.new).should == true

    (ModuleSpecs::Child       === String.new).should == false
    (ModuleSpecs::Child       === Object.new).should == false
  end
  
  it "returns true when the given Object's class includes self or when the given Object is extended by self" do
    (ModuleSpecs::BasicModule === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::SuperModule === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::BasicModule === Object.new.extend(ModuleSpecs::SuperModule)).should == true
    (ModuleSpecs::SuperModule === Object.new.extend(ModuleSpecs::SuperModule)).should == true

    (ModuleSpecs::BasicModule === ModuleSpecs::Parent.new).should == false
    (ModuleSpecs::SuperModule === ModuleSpecs::Parent.new).should == false
    (ModuleSpecs::BasicModule === Object.new).should == false
    (ModuleSpecs::SuperModule === Object.new).should == false
  end
end
