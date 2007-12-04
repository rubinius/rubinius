require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#initialize" do
  it "is a private method" do
    "".private_methods.map { |m| m.to_s }.include?("initialize").should == true    
  end
  
  it "can be used to reinitialize self to str" do
    "some string".instance_eval do
      initialize("another string").equal?(self).should == true
      self.should == "another string"

      initialize("").should == ""
    end
  end
  
  it "doesn't change self when str isn't supplied" do
    "some string".instance_eval do
      initialize().should == "some string"
    end
  end
  
  it "replaces the taint status of self with that of str" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.instance_eval { initialize(b) }
    a.tainted?.should == true
  end
  
  compliant :mri, :jruby do
    it "raises a TypeError if self is frozen" do
      a = "hello".freeze

      a.instance_eval { initialize(a) } # ok, no change
      lambda { a.instance_eval { initialize("world") } }.should raise_error(TypeError)
    end
  end
end
