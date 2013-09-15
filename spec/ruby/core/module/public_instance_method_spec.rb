require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Module#public_instance_method" do
    it "is a public method" do
      Module.should have_public_instance_method(:public_instance_method, false)
    end

    it "requires an argument" do
      Module.new.method(:public_instance_method).arity.should == 1
    end

    describe "when given a public method name" do
      it "returns an UnboundMethod corresponding to the defined Module" do
        ret = ModuleSpecs::Super.public_instance_method(:public_module)
        ret.should be_an_instance_of(UnboundMethod)
        ret.owner.should equal(ModuleSpecs::Basic)

        ret = ModuleSpecs::Super.public_instance_method(:public_super_module)
        ret.should be_an_instance_of(UnboundMethod)
        ret.owner.should equal(ModuleSpecs::Super)
      end

      it "accepts if the name is a Symbol or String" do
        ret = ModuleSpecs::Basic.public_instance_method(:public_module)
        ModuleSpecs::Basic.public_instance_method("public_module").should == ret
      end
    end

    it "raises a TypeError when given a name is not Symbol or String" do
      lambda { Module.new.public_instance_method(nil) }.should raise_error(TypeError)
    end

    it "raises a NameError when given a protected method name" do
      lambda { ModuleSpecs::Basic.public_instance_method(:protected_module) }.should(
        raise_error(NameError) do |error|
          error.should be_an_instance_of(NameError)
          error.name.should == :protected_module
        end
      )
    end

    it "raises a NameError when given a private method name" do
      lambda { ModuleSpecs::Basic.public_instance_method(:private_module) }.should(
        raise_error(NameError) do |error|
          error.should be_an_instance_of(NameError)
          error.name.should == :private_module
        end
      )
    end

    it "raises a NameError when given an undefined method name" do
      lambda { ModuleSpecs::Parent.public_instance_method(:undefed_method) }.should(
        raise_error(NameError) do |error|
          error.should be_an_instance_of(NameError)
          error.name.should == :undefed_method
        end
      )
    end

    it "raises a NameError when given a not existing method name" do
      lambda { Module.new.public_instance_method(:this_is_not_exist) }.should(
        raise_error(NameError) do |error|
          error.should be_an_instance_of(NameError)
          error.name.should == :this_is_not_exist
        end
      )
    end
  end
end