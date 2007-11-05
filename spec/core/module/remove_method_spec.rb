require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

module ModuleSpecs
  class NoInheritance
    def method_to_remove; 1; end
    failure :rubinius do
      remove_method :method_to_remove
    end
  end
  
  class Parent
    def method_to_remove; 1; end
  end
  
  class Child < Parent
    def method_to_remove; 2; end     
    failure :rubinius do
      remove_method :method_to_remove
    end
  end
  
  class First
    def method_to_remove; 1; end
  end
  
  class Second < First
    def method_to_remove; 2; end
  end
end

describe "Module#remove_method" do
  it "removes the method from a class" do
    x = ModuleSpecs::NoInheritance.new
    x.respond_to?(:method_to_remove).should == false
  end
  
  it "removes method from subclass, but not parent" do
    x = ModuleSpecs::Child.new
    x.respond_to?(:method_to_remove).should == true    
    x.method_to_remove.should == 1
  end
  
  it "raises NameError when attempting to remove method further up the inheritance tree" do
    should_raise(NameError) do
      class Third < ModuleSpecs::Second
        failure :rubinius do
          remove_method :method_to_remove
        end
      end
    end
  end
end
