require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#instance_variable_set" do
  it "sets the value of the specified instance variable" do
    class Dog
      def initialize(p1, p2) 
        @a, @b = p1, p2 
      end 
    end 
    Dog.new('cat', 99).instance_variable_set(:@a, 'dog').should == "dog"
  end

  it "sets the value of the instance variable when no instance variables exist yet" do
    class NoVariables; end
    NoVariables.new.instance_variable_set(:@a, "new").should == "new"
  end

  it "raises NameError exception if the argument is not of form '@x'" do
    class NoDog; end
    should_raise(NameError) { NoDog.new.instance_variable_set(:c, "cat") }
  end

  it "should raise ArgumentError if the instance variable name is a Fixnum" do
    should_raise(ArgumentError) { "".instance_variable_set(1, 2) }
  end
  
  it "should raise TypeError if the instance variable name is an object that does not respond to to_str" do
    class KernelSpecs::A; end
    should_raise(TypeError) { "".instance_variable_set(KernelSpecs::A.new, 3) }
  end
  
  it "should raise NameError if the passed object, when coerced with to_str, does not start with @" do
    class KernelSpecs::B
      def to_str
        ":c"
      end
    end
    should_raise(NameError) { "".instance_variable_set(KernelSpecs::B.new, 4) }
  end
  
  it "should raise NameError if pass an object that cannot be a symbol" do
    should_raise(NameError) { "".instance_variable_set(:c, 1) }
  end
  
  it "should accept as instance variable name any instance of a class that responds to to_str" do
    class KernelSpecs::C
      def initialize
        @a = 1
      end
      def to_str
        "@a"
      end
    end
    KernelSpecs::C.new.instance_variable_set(KernelSpecs::C.new, 2).should == 2
  end
end
