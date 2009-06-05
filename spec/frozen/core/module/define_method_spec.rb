require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

class DefineMethodSpecClass
end

describe "Module#define_method when given an UnboundMethod" do
  it "correctly passes given arguments to the new method" do
    klass = Class.new do
      def test_method(arg1, arg2)
        [arg1, arg2]
      end
      define_method(:another_test_method, instance_method(:test_method))
    end
    
    klass.new.another_test_method(1, 2).should == [1, 2]
  end

  it "adds the new method to the methods list" do
    klass = Class.new do
      def test_method(arg1, arg2)
        [arg1, arg2]
      end
      define_method(:another_test_method, instance_method(:test_method))
    end
    klass.new.should have_method(:another_test_method)
  end
end

describe "Module#define_method" do
  it "defines the given method as an instance method with the given name in self" do
    class DefineMethodSpecClass
      def test1
        "test" 
      end
      define_method(:another_test, instance_method(:test1))
    end
    
    o = DefineMethodSpecClass.new
    o.test1.should == o.another_test
  end
  
  it "defines a new method with the given name and the given block as body in self" do
    class DefineMethodSpecClass
      define_method(:block_test1) { self }
      define_method(:block_test2, &lambda { self })
    end
    
    o = DefineMethodSpecClass.new
    o.block_test1.should == o
    o.block_test2.should == o
  end
  
  it "raises a TypeError when the given method is no Method/Proc" do
    lambda {
      Class.new { define_method(:test, "self") }
    }.should raise_error(TypeError)
    
    lambda {
      Class.new { define_method(:test, 1234) }
    }.should raise_error(TypeError)
  end
  
  it "should maintain the Proc's scope" do
    class DefineMethodByProcClass
      in_scope = true
      method_proc = proc { in_scope }
      
      define_method(:proc_test, &method_proc)
    end
    
    o = DefineMethodByProcClass.new
    o.proc_test.should == true
  end
end
