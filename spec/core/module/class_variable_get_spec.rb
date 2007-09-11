require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#class_variable_get" do
  it "returns the value of the class variable with the given name" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    c.send(:class_variable_get, :@@class_var).should == "test"
    c.send(:class_variable_get, :@@class_var.to_i).should == "test"
    c.send(:class_variable_get, "@@class_var").should == "test"
  end
  
  it "raises a NameError when an uninitialized class variable is accessed" do
    c = Class.new
    [:@@no_class_var, :@@no_class_var.to_i, "@@no_class_var"].each do |cvar|
      should_raise(NameError, "uninitialized class variable @@no_class_var in #{c}") do
        c.send(:class_variable_get, cvar)
      end
    end
  end
  
  it "raises a NameError when the given name is not allowed" do
    c = Class.new
    
    should_raise(NameError, "`invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_get, :invalid_name)
    end
    
    should_raise(NameError, "`@invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_get, "@invalid_name")
    end
  end

  it "converts a non string/symbol/fixnum name to string using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    (o = Object.new).should_receive(:to_str, :returning => "@@class_var")
    c.send(:class_variable_get, o).should == "test"
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
       c.send(:class_variable_get, o)
    end
    
    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      c.send(:class_variable_get, o)
    end
  end
end
