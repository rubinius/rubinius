require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is '1.9' do
  describe "Kernel#define_singleton_method" do
    describe "when given an UnboundMethod" do
      class DefineSingletonMethodSpecClass
        MY_CONST = 42
        define_singleton_method(:another_test_method, self.method(:constants))
      end

      it "correctly calls the new method" do
        klass = DefineSingletonMethodSpecClass
        klass.another_test_method.should == klass.constants
      end

      it "adds the new method to the methods list" do
        DefineSingletonMethodSpecClass.should have_method(:another_test_method)
      end
    end

    it "defines a new method with the given name and the given block as body in self" do
      class DefineSingletonMethodSpecClass
        define_singleton_method(:block_test1) { self }
        define_singleton_method(:block_test2, &lambda { self })
      end
    
      o = DefineSingletonMethodSpecClass
      o.block_test1.should == o
      o.block_test2.should == o
    end
  
    it "raises a TypeError when the given method is no Method/Proc" do
      lambda {
        Class.new { define_singleton_method(:test, "self") }
      }.should raise_error(TypeError)
    
      lambda {
        Class.new { define_singleton_method(:test, 1234) }
      }.should raise_error(TypeError)
    end
  
    it "defines a new singleton method for objects" do
      s = "hello"
      s.define_singleton_method(:test) { "world!" }
      s.test.should == "world!"
      lambda {
        "hello".test
      }.should raise_error(NoMethodError)
    end

    it "maintains the Proc's scope" do
      class DefineMethodByProcClass
        in_scope = true
        method_proc = proc { in_scope }
      
        define_singleton_method(:proc_test, &method_proc)
      end
    
      DefineMethodByProcClass.proc_test.should == true
    end
  end
end