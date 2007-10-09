require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
    (ModuleSpecs::Basic === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::Super === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::Basic === Object.new.extend(ModuleSpecs::Super)).should == true
    (ModuleSpecs::Super === Object.new.extend(ModuleSpecs::Super)).should == true

    (ModuleSpecs::Basic === ModuleSpecs::Parent.new).should == false
    (ModuleSpecs::Super === ModuleSpecs::Parent.new).should == false
    (ModuleSpecs::Basic === Object.new).should == false
    (ModuleSpecs::Super === Object.new).should == false
  end
end
