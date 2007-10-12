require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#instance_methods" do
  it "returns an array containing the public and protected methods of self if include_super is false" do
    methods = ModuleSpecs::Parent.instance_methods(false) 
    methods.should_include("protected_parent", "public_parent")
    
    methods = ModuleSpecs::Child.instance_methods(false) 
    methods.should_include("protected_child", "public_child")
  end
  
  it "returns an array containing the public and protected methods of self and it's ancestors" do
    methods = ModuleSpecs::Basic.instance_methods
    methods.should_include("protected_module", "public_module")
    
    methods = ModuleSpecs::Super.instance_methods
    methods.should_include("protected_module", "protected_super_module", 
      "public_module", "public_super_module")
  end
end
