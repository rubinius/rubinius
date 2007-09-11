require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#initialize_copy" do
  it "is a private method" do
    "".private_methods.map { |m| m.to_s }.include?("initialize_copy").should == true
  end
  
  it "is an alias for replace()" do
    a = "some string"
    a.instance_eval do
      initialize_copy("another string").equal?(a).should == true
    end
    
    a.should == "another string"
  end
  
  it "replaces the taint status of self with that of other" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.instance_eval { initialize_copy(b) }
    a.tainted?.should == true
  end
    
  it "raises a TypeError if self is frozen" do
    a = "hello".freeze

    a.instance_eval { initialize_copy(a) } # ok, no change
    should_raise(TypeError) { a.instance_eval { initialize_copy("") } }
    should_raise(TypeError) { a.instance_eval { initialize_copy("world") } }
  end
end
