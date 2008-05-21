require File.dirname(__FILE__) + '/../../spec_helper'

describe "Class.new with a block given" do
  it "uses the given block as the class' body" do
    klass = Class.new do
      def self.hello
        "hello"
      end
      
      def hello
        "hello again"
      end
    end
    
    klass.hello.should     == "hello"
    klass.new.hello.should == "hello again"
  end
end

describe "Class.new" do
  it "creates a new anonymous class" do
    klass = Class.new
    klass.is_a?(Class).should == true
    
    klass_instance = klass.new
    klass_instance.is_a?(klass).should == true
  end
  
  it "creates a class without a name" do
    Class.new.name.should == ""
  end
  
  it "creates a class that can be given a name by assigning it to a constant" do
    MyClass = Class.new
    MyClass.name.should == "MyClass"
    a = Class.new
    MyClass::NestedClass = a
    MyClass::NestedClass.name.should == "MyClass::NestedClass"
  end
  
  it "sets the new class' superclass to the given class" do
    top = Class.new
    Class.new(top).superclass.should == top
  end
  
  it "sets the new class' superclass to Object when no class given" do
    Class.new.superclass.should == Object
  end
  
  it "raises a TypeError when given a non-Class" do
    lambda { Class.new("")         }.should raise_error(TypeError)
    lambda { Class.new(1)          }.should raise_error(TypeError)
    lambda { Class.new(:symbol)    }.should raise_error(TypeError)
    lambda { Class.new(mock('o'))  }.should raise_error(TypeError)
    lambda { Class.new(Module.new) }.should raise_error(TypeError)
  end
end
