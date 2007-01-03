require File.dirname(__FILE__) + '/../spec_helper'

context "Module" do
  specify "instance_methods with false should exclude super class instance methods" do
    example(<<-CODE
    class Foo
      def foo
      end
    end
    CODE
    ) do
      p Foo.instance_methods(false)
    end.should == '["foo"]'
  end
end
