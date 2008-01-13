require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#module_function" do
  it "creates module functions for the given methods" do
    m = Module.new do
      def test()  end
      def test2() end
      def test3() end
      
      module_function :test, :test2
    end
    
    m.respond_to?(:test).should  == true
    m.respond_to?(:test2).should == true
    m.respond_to?(:test3).should == false
  end

  it "creates an independent copy of the function" do
    module Mixin
      def test
        "hello"
      end
      module_function :test
    end

    class BaseClass
      include Mixin
      def call_test
        test
      end
    end

    Mixin.test.should == "hello"
    c = BaseClass.new
    c.call_test.should == "hello"

    module Mixin
      def test
        "goodbye"
      end
    end

    Mixin.test.should == "hello"
    c.call_test.should == "goodbye"
  end

  it "makes the instance method versions private" do
    m = Module.new do
      def test() "hello" end
      module_function :test
    end
    
    (o = mock('x')).extend(m)
    o.respond_to?(:test).should == false
    o.send(:test).should == "hello"
  end
  
  it "makes subsequently defined methods module functions if no names are given" do
    m = Module.new do
      module_function
        def test() end
        def test2() end
      public
        def test3() end
    end

    m.respond_to?(:test).should  == true
    m.respond_to?(:test2).should == true
    m.respond_to?(:test3).should == false
  end
  
  it "tries to convert the given names to strings using to_str" do
    (o = mock('test')).should_receive(:to_str).any_number_of_times.and_return("test")
    (o2 = mock('test2')).should_receive(:to_str).any_number_of_times.and_return("test2")
    
    m = Module.new do
      def test() end
      def test2() end
      module_function o, o2
    end
    
    m.respond_to?(:test).should  == true
    m.respond_to?(:test2).should == true
  end

  it "raises a TypeError when the given names can't be converted to string using to_str" do
    o = mock('123')
    
    lambda { Module.new { module_function(o) } }.should raise_error(TypeError)

    o.should_receive(:to_str).and_return(123)
    lambda { Module.new { module_function(o) } }.should raise_error(TypeError)
  end
end
