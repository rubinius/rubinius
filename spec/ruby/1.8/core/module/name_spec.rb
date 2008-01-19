require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#name" do
  it "returns the name of self" do
    Module.new.name.should == ""
    Class.new.name.should == ""
    
    ModuleSpecs.name.should == "ModuleSpecs"
    ModuleSpecs::Child.name.should == "ModuleSpecs::Child"
    ModuleSpecs::Parent.name.should == "ModuleSpecs::Parent"
    ModuleSpecs::Basic.name.should == "ModuleSpecs::Basic"
    ModuleSpecs::Super.name.should == "ModuleSpecs::Super"
    
    begin
      (ModuleSpecs::X = Module.new).name.should == "ModuleSpecs::X"
    ensure
      ModuleSpecs.send :remove_const, :X
    end
  end

  it "is memoized to the last set constant" do
    m = Module.new
    m.name.should == ""
    begin
      ModuleSpecs.const_set(:X, m)
      ModuleSpecs::X.name.should == "ModuleSpecs::X"
      ModuleSpecs.const_set(:Y, m)
      ModuleSpecs::X.name.should == "ModuleSpecs::X"
      ModuleSpecs::Y.name.should == "ModuleSpecs::X"
    ensure
      ModuleSpecs.send :remove_const, :X
      ModuleSpecs.send :remove_const, :Y rescue nil
    end
    m.name.should == "ModuleSpecs::X"

    m = Module.new
    m.name.should == ""
    begin
      ModuleSpecs.const_set(:X, m)
      ModuleSpecs.const_set(:Y, m)
      ModuleSpecs::X.name.should == "ModuleSpecs::Y"
      ModuleSpecs::Y.name.should == "ModuleSpecs::Y"
    ensure
      ModuleSpecs.send :remove_const, :X
      ModuleSpecs.send :remove_const, :Y rescue nil
    end
    m.name.should == "ModuleSpecs::Y"
  end
end
