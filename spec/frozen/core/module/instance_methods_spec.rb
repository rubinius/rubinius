require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# Prior to MRI 1.9 #instance_methods returned an Array of Strings
ruby_version_is ""..."1.9" do
  describe "Module#instance_methods" do
    it "does not return undefined methods" do
      methods = ModuleSpecs::Parent.instance_methods(false)
      methods.should_not include("undefed_method")

      ModuleSpecs::Child.send(:undef_method, 'undefed_child')
      methods = ModuleSpecs::Child.instance_methods
      methods.should_not include("undefed_method")
      methods.should_not include("undefed_child")
    end

    it "returns an array containing the public and protected methods of self if include_super is false" do
      methods = ModuleSpecs::Parent.instance_methods(false)
      methods.should include("protected_parent", "public_parent")

      methods = ModuleSpecs::Child.instance_methods(false)
      methods.should include("protected_child", "public_child")
    end

    it "returns an array containing the public and protected methods of self and it's ancestors" do
      methods = ModuleSpecs::Basic.instance_methods
      methods.should include("protected_module", "public_module")

      methods = ModuleSpecs::Super.instance_methods
      methods.should include("protected_module", "protected_super_module",
        "public_module", "public_super_module")
    end
  end
end  


# As of MRI 1.9 #instance_methods returns an Array of Symbols
ruby_version_is "1.9" do
  describe "Module#instance_methods" do
    it "does not return undefined methods" do
      methods = ModuleSpecs::Parent.instance_methods(false)
      methods.should_not include(:undefed_method)

      ModuleSpecs::Child.send(:undef_method, :undefed_child)
      methods = ModuleSpecs::Child.instance_methods
      methods.should_not include(:undefed_method)
      methods.should_not include(:undefed_child)
    end

    it "returns an array containing the public and protected methods of self if include_super is false" do
      methods = ModuleSpecs::Parent.instance_methods(false)
      methods.should include(:protected_parent, :public_parent)

      methods = ModuleSpecs::Child.instance_methods(false)
      methods.should include(:protected_child, :public_child)
    end

    it "returns an array containing the public and protected methods of self and it's ancestors" do
      methods = ModuleSpecs::Basic.instance_methods
      methods.should include(:protected_module, :public_module)

      methods = ModuleSpecs::Super.instance_methods
      methods.should include(:protected_module, :protected_super_module,
        :public_module, :public_super_module)
    end
  end
end  
