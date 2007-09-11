require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#define_method" do
  it "defines the given method as an instance method with the given name in self" do
    c = Class.new do
      def test() "test" end
      define_method(:another_test, instance_method(:test))
    end
    
    o = c.new
    o.test.should == o.another_test
  end
  
  it "defines a new method with the given name and the given block as body in self" do
    c = Class.new do
      define_method(:test) { self }
      define_method(:test2, &lambda { self })
    end
    
    o = c.new
    o.test.should == o
    o.test2.should == o
  end
  
  it "raises a TypeError when the given method is no Method/Proc" do
    should_raise(TypeError, "wrong argument type String (expected Proc/Method)") do
      Class.new { define_method(:test, "self") }
    end
    
    should_raise(TypeError, "wrong argument type Fixnum (expected Proc/Method)") do
      Class.new { define_method(:test, 1234) }
    end
  end
end
