require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#public_class_method" do
  before(:each) do
    class << ModuleSpecs::Parent
      private
      def public_method_1; end
      def public_method_2; end
      def public_method_3; end
    end
  end

  it "make an existing class method public" do
    should_raise(NoMethodError) { ModuleSpecs::Parent.public_method_1 }
    ModuleSpecs::Parent.module_eval "public_class_method :public_method_1"
    ModuleSpecs::Parent.public_method_1.should == nil

    # Technically above we're testing the Singleton classes, class method(right?).
    # Try a "real" class method set public.
    ModuleSpecs::Parent.public_method.should == nil
  end

  it "make an existing class method public up the inheritance tree" do
    should_raise(NoMethodError) { ModuleSpecs::Child.public_method_1 }
    ModuleSpecs::Child.module_eval "public_class_method :public_method_1"

    ModuleSpecs::Child.public_method_1.should == nil
    ModuleSpecs::Child.public_method.should == nil
  end

  it "excepts more than one method at a time" do
    should_raise(NoMethodError) { ModuleSpecs::Parent.public_method_1 }
    should_raise(NoMethodError) { ModuleSpecs::Parent.public_method_2 }
    should_raise(NoMethodError) { ModuleSpecs::Parent.public_method_3 }

    ModuleSpecs::Child.module_eval "public_class_method :public_method_1, :public_method_2, :public_method_3"
    
    ModuleSpecs::Child.public_method_1.should == nil
    ModuleSpecs::Child.public_method_2.should == nil
    ModuleSpecs::Child.public_method_3.should == nil
  end

  it "should raise a NameError if class method doesn't exist" do
    should_raise(NameError) { ModuleSpecs.module_eval "public_class_method :no_method_here" }
  end
end
