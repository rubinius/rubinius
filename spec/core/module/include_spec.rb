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
    should_raise(TypeError) { ModuleSpecs::Basic.send(:include, Class.new) }
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
    should_raise(TypeError) { ModuleSpecs::Child.include?("Test") }
    should_raise(TypeError) { ModuleSpecs::Child.include?(ModuleSpecs::Parent) }
  end
end
