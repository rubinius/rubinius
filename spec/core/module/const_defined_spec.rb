require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#const_defined?" do
  it "returns true if constant with the given Symbol is defined in self" do
    ModuleSpecs.const_defined?(:Child).should == true
    ModuleSpecs.const_defined?(:SomeThing).should == false
  end
  
  it "returns true if a constant with the given String is defined in self" do
    ModuleSpecs.const_defined?("Child").should == true
    ModuleSpecs.const_defined?("SomeThing").should == false
  end
  
  it "returns true if a constant with the given FixNum is defined in self" do
    # ModuleSpecs.const_defined?(:Child.to_i).should == true
    # ModuleSpecs.const_defined?(:SomeThing.to_i).should == false
  end
  
  it "tries to convert the given name to a string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "Child")
    ModuleSpecs.const_defined?(o).should == true
  end
  
  it "raises a NameError when the given constant name is not allowed" do
    # should_raise(NameError, "wrong constant name invalid_name") do
    #   ModuleSpecs.const_defined?("invalid_name")
    # end
    
    # should_raise(NameError, "wrong constant name @invalid_name") do
    #   ModuleSpecs.const_defined?("@invalid_name")
    # end
  end
  
  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    # o = Object.new
    # should_raise(TypeError, "#{o} is not a symbol") do
    #    ModuleSpecs.const_defined?(o)
    # end
    # 
    # o.should_receive(:to_str, :returning => 123)
    # should_raise(TypeError, "Object#to_str should return String") do
    #   ModuleSpecs.const_defined?(o)
    # end
  end
end
