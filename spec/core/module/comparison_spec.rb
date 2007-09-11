require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#<=>" do
  it "returns -1 if self is a subclass of or includes the given module" do
    (ModuleSpecs::Child       <=> ModuleSpecs::Parent).should == -1
    (ModuleSpecs::Child       <=> ModuleSpecs::BasicModule).should == -1
    (ModuleSpecs::Child       <=> ModuleSpecs::SuperModule).should == -1
    (ModuleSpecs::SuperModule <=> ModuleSpecs::BasicModule).should == -1
  end
  
  it "returns 0 if self is the same as the given module" do
    (ModuleSpecs::Child       <=> ModuleSpecs::Child).should == 0
    (ModuleSpecs::Parent      <=> ModuleSpecs::Parent).should == 0
    (ModuleSpecs::BasicModule <=> ModuleSpecs::BasicModule).should == 0
    (ModuleSpecs::SuperModule <=> ModuleSpecs::SuperModule).should == 0
  end
  
  it "returns +1 if self is a superclas of or included by the given module" do
    (ModuleSpecs::Parent      <=> ModuleSpecs::Child).should == +1
    (ModuleSpecs::BasicModule <=> ModuleSpecs::Child).should == +1
    (ModuleSpecs::SuperModule <=> ModuleSpecs::Child).should == +1
    (ModuleSpecs::BasicModule <=> ModuleSpecs::SuperModule).should == +1
  end
  
  it "returns nil if self and the given module are not related" do
    (ModuleSpecs::Parent      <=> ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      <=> ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule <=> ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule <=> ModuleSpecs::Parent).should == nil
  end
  
  it "returns nil if the argument is no class/module" do
    (ModuleSpecs::Parent      <=> Object.new).should == nil
  end
end
