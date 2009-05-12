require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#method" do
  it "returns a method object for a valid method" do
    class KernelSpecs::Foo; def bar; 'done'; end; end
    KernelSpecs::Foo.new.method(:bar).class.should == Method
  end

  it "returns a method object for a valid singleton method" do
    class KernelSpecs::Foo; def self.bar; 'done'; end; end
    KernelSpecs::Foo.method(:bar).class.should == Method
  end

  it "raises a NameError for an invalid method name" do
    class KernelSpecs::Foo; def bar; 'done'; end; end
    lambda {
      KernelSpecs::Foo.new.method(:invalid_and_silly_method_name)
    }.should raise_error(NameError)
  end

  it "raises a NameError for an invalid singleton method name" do
    class KernelSpecs::Foo; def self.bar; 'done'; end; end
    lambda { KernelSpecs::Foo.method(:baz) }.should raise_error(NameError)
  end

  # This may be a bug; see http://redmine.ruby-lang.org/issues/show/1151
  ruby_version_is "" ... "1.9" do
    it "changes the method called for super on a target aliased method" do
      c1 = Class.new do
        def a; 'a'; end
        def b; 'b'; end
      end
      c2 = Class.new(c1) do
        def a; super; end
        alias b a
      end

      c2.new.a.should == 'a'
      c2.new.b.should == 'a'
      c2.new.method(:b).call.should == 'b'
    end
  end
end
