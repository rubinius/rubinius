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

  it "raises a NoMethodError if the corresponding method can't be found" do
    class KernelSpecs::Foo
      def bar
        'done'
      end
    end
    lambda { KernelSpecs::Foo.new.send(:baz) }.should raise_error(NameError)
  end

  it "raises a NoMethodError if the corresponding singleton method can't be found" do
    class KernelSpecs::Foo
      def self.bar
        'done'
      end
    end
    lambda { KernelSpecs::Foo.send(:baz) }.should raise_error(NameError)
  end

  it "raises an ArgumentError if called with more arguments than available parameters" do
    class KernelSpecs::Foo
      def bar; end
    end

    lambda { KernelSpecs::Foo.new.send(:bar, :arg) }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if called with fewer arguments than required parameters" do
    class KernelSpecs::Foo
      def foo(arg); end
    end

    lambda { KernelSpecs::Foo.new.send(:foo) }.should raise_error(ArgumentError)
  end

  it "succeeds if passed an arbitrary number of arguments as a splat parameter" do
    class KernelSpecs::Foo
      def baz(*args) args end
    end

    begin
      KernelSpecs::Foo.new.send(:baz).should == []
      KernelSpecs::Foo.new.send(:baz, :quux).should == [:quux]
      KernelSpecs::Foo.new.send(:baz, :quux, :foo).should == [:quux, :foo]
    rescue
      fail
    end
  end

  it "succeeds when passing 1 or more arguments as a required and a splat parameter" do
    class KernelSpecs::Foo
      def foo(first, *rest) [first, *rest] end
    end

    begin
      KernelSpecs::Foo.new.send(:baz, :quux).should == [:quux]
      KernelSpecs::Foo.new.send(:baz, :quux, :foo).should == [:quux, :foo]
    rescue
      fail
    end
  end

  it "succeeds when passing 0 arguments to a method with one parameter with a default" do
    class KernelSpecs::Foo
      def foo(first = true) first end
    end

    begin
      KernelSpecs::Foo.new.send(:foo).should == true
      KernelSpecs::Foo.new.send(:foo, :arg).should == :arg
    rescue
      fail
    end
  end
end
