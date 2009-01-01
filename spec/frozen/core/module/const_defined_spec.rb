require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../fixtures/constants'

describe "Module#const_defined?" do
  it "returns true if the given Symbol names a constant defined in the receiver" do
    ConstantSpecs.const_defined?(:CS_CONST2).should == true
    ConstantSpecs.const_defined?(:ModuleA).should == true
    ConstantSpecs.const_defined?(:ClassA).should == true
    ConstantSpecs::ContainerA.const_defined?(:ChildA).should == true
  end

  it "returns true if the given String names a constant defined in the receiver" do
    ConstantSpecs.const_defined?("CS_CONST2").should == true
    ConstantSpecs.const_defined?("ModuleA").should == true
    ConstantSpecs.const_defined?("ClassA").should == true
    ConstantSpecs::ContainerA.const_defined?("ChildA").should == true
  end

  it "returns false if the constant is not defined in the receiver" do
    ConstantSpecs::ContainerA::ChildA.const_defined?(:CS_CONST4).should == false
    ConstantSpecs::ParentA.const_defined?(:CS_CONST12).should == false
  end

  it "does not call #const_missing if the constant is not defined in the receiver" do
    ConstantSpecs::ClassA.should_not_receive(:const_missing)
    ConstantSpecs::ClassA.const_defined?(:CS_CONSTX).should == false
  end

  it "calls #to_str to convert the given name to a String" do
    name = mock("ClassA")
    name.should_receive(:to_str).and_return("ClassA")
    ConstantSpecs.const_defined?(name).should == true
  end

  it "raises a NameError if the name does not start with a capital letter" do
    lambda { ConstantSpecs.const_defined? "name" }.should raise_error(NameError)
  end

  it "raises a NameError if the name starts with a non-alphabetic character" do
    lambda { ConstantSpecs.const_defined? "__CONSTX__" }.should raise_error(NameError)
    lambda { ConstantSpecs.const_defined? "@Name" }.should raise_error(NameError)
    lambda { ConstantSpecs.const_defined? "!Name" }.should raise_error(NameError)
    lambda { ConstantSpecs.const_defined? "::Name" }.should raise_error(NameError)
  end

  it "raises a NameError if the name contains non-alphabetic characters except '_'" do
    ConstantSpecs.const_defined?("CS_CONSTX").should == false
    lambda { ConstantSpecs.const_defined? "Name=" }.should raise_error(NameError)
    lambda { ConstantSpecs.const_defined? "Name?" }.should raise_error(NameError)
  end

  it "raises a TypeError if conversion to a String by calling #to_str fails" do
    name = mock('123')
    lambda { ConstantSpecs.const_defined? name }.should raise_error(TypeError)

    name.should_receive(:to_str).and_return(123)
    lambda { ConstantSpecs.const_defined? name }.should raise_error(TypeError)
  end
end
