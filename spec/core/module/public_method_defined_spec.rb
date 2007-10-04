require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#public_method_defined?" do
  it "returns true if the named public method is defined by module or its ancestors" do
    ModuleSpecs::CountsMixin.public_method_defined?("public_3").should == true

    ModuleSpecs::CountsParent.public_method_defined?("public_3").should == true
    ModuleSpecs::CountsParent.public_method_defined?("public_2").should == true
    
    ModuleSpecs::CountsChild.public_method_defined?("public_3").should == true
    ModuleSpecs::CountsChild.public_method_defined?("public_2").should == true
    ModuleSpecs::CountsChild.public_method_defined?("public_1").should == true
  end

  it "should return false if method is not a public method" do
    ModuleSpecs::CountsChild.public_method_defined?("private_3").should == false
    ModuleSpecs::CountsChild.public_method_defined?("private_2").should == false
    ModuleSpecs::CountsChild.public_method_defined?("private_1").should == false

    ModuleSpecs::CountsChild.public_method_defined?("protected_3").should == false
    ModuleSpecs::CountsChild.public_method_defined?("protected_2").should == false
    ModuleSpecs::CountsChild.public_method_defined?("protected_1").should == false
  end

  it "returns false if the named public method is not defined by the module or its ancestors" do
    ModuleSpecs::CountsMixin.public_method_defined?(:public_10).should == false
  end

  it "should except symbols as a method name" do
    ModuleSpecs::CountsMixin.public_method_defined?(:public_3).should == true
  end

  it "raise an error on improper argument" do
    should_raise(ArgumentError) { ModuleSpecs::CountsMixin.public_method_defined?(1) }
    should_raise(TypeError) { ModuleSpecs::CountsMixin.public_method_defined?(nil) }
    should_raise(TypeError) { ModuleSpecs::CountsMixin.public_method_defined?(false) }
  end
end
