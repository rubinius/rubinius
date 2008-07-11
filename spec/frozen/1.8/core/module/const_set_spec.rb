require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#const_set" do
  it "sets the constant with the given name to the given value" do
    Module.const_set :A, "A"
    Module.const_set "C", "C"
    
    Module.const_get("A").should == "A"
    Module.const_get("C").should == "C"
  end

  it "raises a NameError when the given name is no valid constant name" do
    lambda {
      Module.const_set "invalid", "some value"
    }.should raise_error(NameError)

    lambda {
      Module.const_set "Dup Dup", "some value"
    }.should raise_error(NameError)

    lambda {
      Module.const_set "123", "some value"
    }.should raise_error(NameError)
  end
  
  it "raises a NameError when there is no constant with the given name" do
    lambda {
      ModuleSpecs.const_get("NotExistant")
    }.should raise_error(NameError)
  end

  it "tries to convert the given name to a string using to_str" do
    (o = mock('A')).should_receive(:to_str).and_return("A")
    Module.const_set o, "test"
    Module.const_get(:A).should == "test"
  end
  
  it "raises a TypeError when the given name can't be converted to string using to_str" do
    o = mock('123')
    lambda { Module.const_set(o, "test") }.should raise_error(TypeError)

    o.should_receive(:to_str).and_return(123)
    lambda { Module.const_set(o, "Test") }.should raise_error(TypeError)
  end
end
