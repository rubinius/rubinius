require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Module#instance_methods" do
  it "excludes super class instance methods when passed false" do
    class A
      def foo
      end
    end
    A.instance_methods(false).should == ["foo"]
  end

  it "returns all instance methods of a module when not passed an argument" do
    module B
      def foo
      end
    end
    B.instance_methods.should include("foo")
  end
end
