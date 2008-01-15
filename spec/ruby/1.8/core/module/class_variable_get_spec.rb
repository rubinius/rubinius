require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#class_variable_get" do
  it "returns the value of the class variable with the given name" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    c.send(:class_variable_get, :@@class_var).should == "test"
    c.send(:class_variable_get, "@@class_var").should == "test"
  end
  
  not_compliant_on :rubinius do
    it "accepts Fixnums for class variables" do
      c = Class.new { class_variable_set :@@class_var, "test" }
      c.send(:class_variable_get, :@@class_var.to_i).should == "test"
    end
    
    it "raises a NameError when a Fixnum for an uninitialized class variable is given" do
      c = Class.new
      lambda {
        c.send :class_variable_get, :@@no_class_var.to_i
      }.should raise_error(NameError)
    end
  end
  
  it "raises a NameError when an uninitialized class variable is accessed" do
    c = Class.new
    [:@@no_class_var, "@@no_class_var"].each do |cvar|
      lambda  { c.send(:class_variable_get, cvar) }.should raise_error(NameError)
    end
  end
  
  it "raises a NameError when the given name is not allowed" do
    c = Class.new
    
    lambda { c.send(:class_variable_get, :invalid_name)   }.should raise_error(NameError)
    lambda { c.send(:class_variable_get, "@invalid_name") }.should raise_error(NameError)
  end

  it "converts a non string/symbol/fixnum name to string using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    (o = mock('@@class_var')).should_receive(:to_str).and_return("@@class_var")
    c.send(:class_variable_get, o).should == "test"
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    o = mock('123')
    lambda { c.send(:class_variable_get, o) }.should raise_error(TypeError)
    o.should_receive(:to_str).and_return(123)
    lambda { c.send(:class_variable_get, o) }.should raise_error(TypeError)
  end
end
