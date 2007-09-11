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
end

describe "Module#include?" do
  it "returns true if the given module is included by self or one of it's ancestors" do
    ModuleSpecs::SuperModule.include?(ModuleSpecs::BasicModule).should == true
    ModuleSpecs::Child.include?(ModuleSpecs::BasicModule).should == true
    ModuleSpecs::Child.include?(ModuleSpecs::SuperModule).should == true
    ModuleSpecs::Child.include?(Kernel).should == true
    
    ModuleSpecs::Parent.include?(ModuleSpecs::BasicModule).should == false
    ModuleSpecs::BasicModule.include?(ModuleSpecs::SuperModule).should == false
  end
  
  it "raises a TypeError when no module was given" do
    should_raise(TypeError, "wrong argument type String (expected Module)") do
      ModuleSpecs::Child.include?("Test")
    end

    should_raise(TypeError, "wrong argument type Class (expected Module)") do
      ModuleSpecs::Child.include?(ModuleSpecs::Parent)
    end
  end
end
