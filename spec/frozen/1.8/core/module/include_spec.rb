require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#include" do
  it "calls #append_features(self) in reversed order on each module" do
    $appended_modules = []
    
    m = Module.new do
      def self.append_features(mod)
        $appended_modules << [ self, mod ]
      end
    end
    
    m2 = Module.new do
      def self.append_features(mod)
        $appended_modules << [ self, mod ]
      end
    end

    m3 = Module.new do
      def self.append_features(mod)
        $appended_modules << [ self, mod ]
      end
    end
    
    c = Class.new { include(m, m2, m3) }
    
    $appended_modules.should == [ [ m3, c], [ m2, c ], [ m, c ] ]
  end
  
  it "raises a TypeError when the argument is not a Module" do
    lambda { ModuleSpecs::Basic.send(:include, Class.new) }.should raise_error(TypeError)
  end

  it "does not raise a TypeError when the argument is an instance of a subclass of Module" do
    lambda { ModuleSpecs::SubclassSpec.send(:include, ModuleSpecs::Subclass.new) }.should_not raise_error(TypeError)
  end
  
  it "imports constants to modules and classes" do
    ModuleSpecs::A.constants.should include("CONSTANT_A")
    ModuleSpecs::B.constants.should include("CONSTANT_A","CONSTANT_B")
    ModuleSpecs::C.constants.should include("CONSTANT_A","CONSTANT_B")
  end

  it "does not override existing constants in modules and classes" do
    ModuleSpecs::A::OVERRIDE.should == :a
    ModuleSpecs::B::OVERRIDE.should == :b
    ModuleSpecs::C::OVERRIDE.should == :c    
  end

  it "imports instance methods to modules and classes" do
    ModuleSpecs::A.instance_methods.should include("ma")
    ModuleSpecs::B.instance_methods.should include("ma","mb")
    ModuleSpecs::C.instance_methods.should include("ma","mb")
  end

  it "imports constants to the toplevel" do
    eval "include ModuleSpecs::Z", TOPLEVEL_BINDING
    MODULE_SPEC_TOPLEVEL_CONSTANT.should == 1
  end

  it "does not import methods to modules and classes" do
    ModuleSpecs::A.methods.include?("cma").should == true
    ModuleSpecs::B.methods.include?("cma").should == false
    ModuleSpecs::B.methods.include?("cmb").should == true
    ModuleSpecs::C.methods.include?("cma").should == false    
    ModuleSpecs::C.methods.include?("cmb").should == false
  end

  it "attaches the module as the caller's immediate ancestor" do
    module IncludeSpecsTop
      def value; 5; end
    end

    module IncludeSpecsMiddle
      include IncludeSpecsTop
      def value; 6; end
    end

    class IncludeSpecsClass
      include IncludeSpecsMiddle
    end

    IncludeSpecsClass.new.value.should == 6
  end
end

describe "Module#include?" do
  it "returns true if the given module is included by self or one of it's ancestors" do
    ModuleSpecs::Super.include?(ModuleSpecs::Basic).should == true
    ModuleSpecs::Child.include?(ModuleSpecs::Basic).should == true
    ModuleSpecs::Child.include?(ModuleSpecs::Super).should == true
    ModuleSpecs::Child.include?(Kernel).should == true
    
    ModuleSpecs::Parent.include?(ModuleSpecs::Basic).should == false
    ModuleSpecs::Basic.include?(ModuleSpecs::Super).should == false
  end
  
  it "raises a TypeError when no module was given" do
    lambda { ModuleSpecs::Child.include?("Test") }.should raise_error(TypeError)
    lambda { ModuleSpecs::Child.include?(ModuleSpecs::Parent) }.should raise_error(TypeError)
  end
end
