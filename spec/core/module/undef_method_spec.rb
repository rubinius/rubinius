require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

module ModuleSpecs
  class NoInheritance
    def method_to_undef;1;end
    failure :rubinius do
      undef_method :method_to_undef
    end
  end
  
  class Parent
    def method_to_undef; 1; end
  end
  
  class Child < Parent
    failure :rubinius do
      undef_method :method_to_undef
    end
  end
  
  class Ancestor
    def method_to_undef; 1; end
  end
  
  class Descendant < Ancestor
    failure :rubinius do
      undef_method :method_to_undef
    end
  end
end

#undef method kills it all the way up the tree
describe "Module#undef_method" do
  it "removes a method definition from a class" do
    x = ModuleSpecs::NoInheritance.new
    x.respond_to?(:method_to_undef).should == false
  end
  
  it "removes a method defined in a super class" do
    x = ModuleSpecs::Child.new
    x.respond_to?(:method_to_undef).should == false
  end
  
  it "does not remove the method for instances of a parent class when removed in a child" do
    x = ModuleSpecs::Ancestor.new
    x.respond_to?(:method_to_undef).should == true
  end
end