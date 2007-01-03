require File.dirname(__FILE__) + '/../spec_helper'

context "class" do
  specify "should create a new class" do
    example(<<-CODE
    class Foo; end
    CODE
    ) do
      p Foo.new.class
    end.should == 'Foo'
  end


end

context "def" do
  specify "should create a new method" do
    example(<<-CODE
    class Foo
      def foo
        'foo'
      end
    end
    CODE
    ) do
      puts Foo.new.foo
    end.should == 'foo'
  end
end
