require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#instance_variable_get" do
  it "returns the value of the instance variable" do
    class Fred 
      def initialize(p1, p2) 
        @a, @b = p1, p2 
      end 
    end 
    fred = Fred.new('cat', 99) 
    fred.instance_variable_get(:@a).should == "cat"
    fred.instance_variable_get("@b").should == 99

    a = []
    a.instance_variable_set(:@c, 1)
    a.instance_variable_get(:@c).should == 1
  end

  it "returns nil if the instance variable does not exist" do
    [].instance_variable_get(:@c).should == nil
  end

  it "raises NameError exception if the argument is not of form '@x'" do
    class NoFred; end
    should_raise(NameError) { NoFred.new.instance_variable_get(:c) }
    should_raise(NameError) { [].instance_variable_get(:c) }
  end

  it "should raise ArgumentError if the instance variable name is a Fixnum" do
    should_raise(ArgumentError) { "".instance_variable_get(1) }
  end
  
  it "should raise TypeError if the instance variable name is an object that does not respond to to_str" do
    class KernelSpecs::A; end
    should_raise(TypeError) { "".instance_variable_get(KernelSpecs::A.new) }
  end
  
  it "should raise NameError if the passed object, when coerced with to_str, does not start with @" do
    class KernelSpecs::B
      def to_str
        ":c"
      end
    end
    should_raise(NameError) { "".instance_variable_get(KernelSpecs::B.new) }
  end
  
  it "should raise NameError if pass an object that cannot be a symbol" do
    should_raise(NameError) { "".instance_variable_get(:c) }
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
    KernelSpecs::C.new.instance_variable_get(KernelSpecs::C.new).should == 1
  end
end
