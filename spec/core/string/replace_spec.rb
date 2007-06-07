require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#replace(other)" do
  it "should replace the content of self with other" do
    a = "some string"
    a.replace("another string")
    a.should == "another string"
  end
  
  it "should replace the taintedness of self with that of other" do
    a = "an untainted string"
    b = "a tainted string"
    b.taint
    a.replace(b)
    a.tainted?.should == true
  end
  
  it "should try to convert other to string using to_str" do
    module StringSpec
      class ReplaceObject
        def to_str
          "an object converted to a string"
        end
      end
    end
    
    "hello".replace(StringSpec::ReplaceObject.new).should == "an object converted to a string"
  end
  
  it "should raise a TypeError if other can't be converted to string" do
    should_raise(TypeError) do
      "hello".replace(123)
    end
    
    should_raise(TypeError) do
      "hello".replace(:test)
    end
    
    should_raise(TypeError) do
      "hello".replace(Object.new)
    end
  end
  
  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.replace("world")
    end
  end
end

describe "String#initialize_copy" do
  it "should be an alias of String#replace" do
    # TODO
  end
end