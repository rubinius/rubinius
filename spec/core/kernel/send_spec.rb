require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#send" do
  it "invokes the named method" do
    class KernelSpecs::Foo
      def bar
        'done'
      end
    end
    KernelSpecs::Foo.new.send(:bar).should == 'done'
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
end
