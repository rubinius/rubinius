require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#private_class_method" do
  before(:each) do
    # This is not in classes.rb because after marking a class method private it 
    # will stay private.
    class << ModuleSpecs::Parent
      def private_method_1; end
      def private_method_2; end
      def private_method_3; end
    end
  end

  it "makes an existing class method private" do
    ModuleSpecs::Parent.private_method_1.should == nil
    ModuleSpecs::Parent.private_class_method :private_method_1
    should_raise(NoMethodError) { ModuleSpecs::Parent.private_method_1  }

    # Technically above we're testing the Singleton classes, class method(right?).
    # Try a "real" class method set private.
    should_raise(NoMethodError) { ModuleSpecs::Parent.private_method }
  end

  it "makes an existing class method private up the inheritance tree" do
    ModuleSpecs::Child.private_method_1.should == nil
    ModuleSpecs::Child.private_class_method :private_method_1

    should_raise(NoMethodError) { ModuleSpecs::Child.private_method_1 }
    should_raise(NoMethodError) { ModuleSpecs::Child.private_method }
  end

  it "accepts more than one method at a time" do
    ModuleSpecs::Parent.private_method_1.should == nil
    ModuleSpecs::Parent.private_method_2.should == nil
    ModuleSpecs::Parent.private_method_3.should == nil

    ModuleSpecs::Child.private_class_method :private_method_1, :private_method_2, :private_method_3
    
    should_raise(NoMethodError) { ModuleSpecs::Child.private_method_1 }
    should_raise(NoMethodError) { ModuleSpecs::Child.private_method_2 }
    should_raise(NoMethodError) { ModuleSpecs::Child.private_method_3 }
  end

  it "should raise a NameError if class method doesn't exist" do
    should_raise(NameError) { ModuleSpecs.private_class_method :no_method_here }
  end

  it "makes a class method private" do
    c = Class.new do
      def self.foo() "foo" end
      private_class_method :foo
    end
    should_raise(NoMethodError) { c.foo }
  end

  it "raises a NameError when the given name is not a method" do
    should_raise(NameError) do
      c = Class.new do
        private_class_method :foo
      end
    end
  end

  it "raises a NameError when the given name is an instance method" do
    should_raise(NameError) do
      c = Class.new do
        def foo() "foo" end
        private_class_method :foo
      end
    end
  end
end
