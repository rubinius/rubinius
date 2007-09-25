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
  
  it "sets the new class' superclass to the given class" do
    top = Class.new
    Class.new(top).superclass.should == top
  end
  
  it "sets the new class' superclass to Object when no class given" do
    Class.new.superclass.should == Object
  end
  
  it "raises a TypeError when given a non-Class" do
    should_raise(TypeError, "superclass must be a Class (String given)") do
      Class.new("")
    end

    should_raise(TypeError, "superclass must be a Class (Fixnum given)") do
      Class.new(1)
    end

    should_raise(TypeError, "superclass must be a Class (Symbol given)") do
      Class.new(:symbol)
    end

    should_raise(TypeError, "superclass must be a Class (Object given)") do
      Class.new(Object.new)
    end

    should_raise(TypeError, "superclass must be a Class (Module given)") do
      Class.new(Module.new)
    end
  end
end