require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#private_method_defined?" do
  it "returns true if the named private method is defined by module or its ancestors" do
    ModuleSpecs::CountsMixin.private_method_defined?("private_3").should == true

    ModuleSpecs::CountsParent.private_method_defined?("private_3").should == true
    ModuleSpecs::CountsParent.private_method_defined?("private_2").should == true

    ModuleSpecs::CountsChild.private_method_defined?("private_3").should == true
    ModuleSpecs::CountsChild.private_method_defined?("private_2").should == true
    ModuleSpecs::CountsChild.private_method_defined?("private_1").should == true
  end

  it "returns false if method is not a private method" do
    ModuleSpecs::CountsChild.private_method_defined?("public_3").should == false
    ModuleSpecs::CountsChild.private_method_defined?("public_2").should == false
    ModuleSpecs::CountsChild.private_method_defined?("public_1").should == false

    ModuleSpecs::CountsChild.private_method_defined?("protected_3").should == false
    ModuleSpecs::CountsChild.private_method_defined?("protected_2").should == false
    ModuleSpecs::CountsChild.private_method_defined?("protected_1").should == false
  end

  it "returns false if the named method is not defined by the module or its ancestors" do
    ModuleSpecs::CountsMixin.private_method_defined?(:private_10).should == false
  end

  it "accepts symbols for the method name" do
    ModuleSpecs::CountsMixin.private_method_defined?(:private_3).should == true
  end

  not_compliant_on :rubinius do
    it "raises an ArgumentError if passed a Fixnum" do
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(1)
      }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed a Symbol" do
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(nil)
      }.should raise_error(TypeError)
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(false)
      }.should raise_error(TypeError)
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(mock('x'))
      }.should raise_error(TypeError)

      sym = mock('symbol')
      def sym.to_sym() :private_3 end
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(sym)
      }.should raise_error(TypeError)
    end
  end

  it "accepts any argument that is a String type" do
    str = mock('string')
    def str.to_str() 'private_3' end
    ModuleSpecs::CountsMixin.private_method_defined?(str).should == true
  end

  deviates_on :rubinius do
    it "raises a TypeError if not passed a String type" do
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(1)
      }.should raise_error(TypeError)
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(nil)
      }.should raise_error(TypeError)
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(false)
      }.should raise_error(TypeError)
      lambda {
        ModuleSpecs::CountsMixin.private_method_defined?(mock('x'))
      }.should raise_error(TypeError)
    end
  end
end
