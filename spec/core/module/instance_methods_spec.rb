require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#instance_methods" do
  it "returns an array containing the public and protected methods of self if include_super is false" do
    methods = ModuleSpecs::Parent.instance_methods(false) 
    methods.sort.should == [ "protected_parent", "public_parent" ]
    
    methods = ModuleSpecs::Child.instance_methods(false) 
    methods.sort.should == [ "protected_child", "public_child" ]
  end
  
  it "returns an array containing the public and protected methods of self and it's ancestors" do
    methods = ModuleSpecs::BasicModule.instance_methods
    methods.sort.should == [ "protected_module", "public_module" ]
    
    methods = ModuleSpecs::SuperModule.instance_methods
    methods.sort.should == [ "protected_module", "protected_super_module", "public_module", "public_super_module" ]
  end
end
