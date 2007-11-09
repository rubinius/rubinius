require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#send" do
  it "invokes the named method" do
    class KernelSpecs::Foo
      def bar
        'done'
      end

      private
      def bar2
        'done2'
      end
    end
    KernelSpecs::Foo.new.send(:bar).should == 'done'
    KernelSpecs::Foo.new.send(:bar2).should == 'done2'
  end

  it "invokes a class method if called on a class" do
    class KernelSpecs::Foo
      def self.bar
        'done'
      end
    end
    KernelSpecs::Foo.send(:bar).should == 'done'
  end

  it "raises NoMethodError if the corresponding method can't be found" do
    class KernelSpecs::Foo
      def bar
        'done'
      end
    end
    should_raise(NoMethodError) { KernelSpecs::Foo.new.send(:baz) }
  end

  it "raises NoMethodError if the corresponding singleton method can't be found" do
    class KernelSpecs::Foo
      def self.bar
        'done'
      end
    end
    should_raise(NoMethodError) { KernelSpecs::Foo.send(:baz) }
  end

  it "raises ArgumentError if called with more arguments than available parameters" do
    class KernelSpecs::Foo
      def bar; end
    end

    should_raise(ArgumentError) { KernelSpecs::Foo.new.send(:bar, :arg) }
  end
  
  it "raises ArgumentError if called with fewer arguments than required parameters" do
    class KernelSpecs::Foo
      def foo(arg); end
    end

    should_raise(ArgumentError) { KernelSpecs::Foo.new.send(:foo) }
  end
  
  it "succeeds if passed an arbitrary number of arguments as a splat parameter" do
    class KernelSpecs::Foo
      def baz(*args); end
    end
    
    begin
      KernelSpecs::Foo.new.send(:baz)
    rescue
      fail
    end
    
    begin
      KernelSpecs::Foo.new.send(:baz, :quux)
    rescue
      fail
    end
    
    begin
      KernelSpecs::Foo.new.send(:baz, :quux, :foo)
    rescue
      fail
    end
  end
  
  it "succeeds when passing 1 or more arguments as a required and a splat parameter" do
    class KernelSpecs::Foo
      def foo(first, *rest); end
    end
    
    begin
      KernelSpecs::Foo.new.send(:baz, :quux)
    rescue
      fail
    end

    begin
      KernelSpecs::Foo.new.send(:baz, :quux, :foo)
    rescue
      fail
    end
  end
  
  it "succeeds when passing 0 arguments to a method with one parameter with a default" do
    class KernelSpecs::Foo
      def foo(first = true); end
    end
    
    begin
      KernelSpecs::Foo.new.send(:foo)
    rescue
      fail
    end

    begin
      KernelSpecs::Foo.new.send(:foo, :arg)
    rescue
      fail
    end
  end
end
