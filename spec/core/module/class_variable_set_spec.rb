require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#class_variable_set" do
  it "sets the class variable with the given name to the given value" do
    c = Class.new
    
    c.send(:class_variable_set, :@@test, "test")
    c.send(:class_variable_set, :@@test2.to_i, "test2")
    c.send(:class_variable_set, "@@test3", "test3")
    
    c.send(:class_variable_get, :@@test).should == "test"
    c.send(:class_variable_get, :@@test2).should == "test2"
    c.send(:class_variable_get, :@@test3).should == "test3"
  end
  
  it "raises a TypeError when self is frozen" do
    should_raise(TypeError, "can't modify frozen class") do
      Class.new.freeze.send(:class_variable_set, :@@test, "test")
    end

    should_raise(TypeError, "can't modify frozen module") do
      Module.new.freeze.send(:class_variable_set, :@@test, "test")
    end
  end
  
  it "raises a NameError when the given name is not allowed" do
    c = Class.new
    
    should_raise(NameError, "`invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_set, :invalid_name, "test")
    end
    
    should_raise(NameError, "`@invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_set, "@invalid_name", "test")
    end
  end

  it "converts a non string/symbol/fixnum name to string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "@@class_var")
    c = Class.new
    c.send(:class_variable_set, o, "test")
    c.send(:class_variable_get, :@@class_var).should == "test"
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
       c.send(:class_variable_set, o, "test")
    end
    
    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      c.send(:class_variable_set, o, "test")
    end
  end
end
