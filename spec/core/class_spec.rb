require File.dirname(__FILE__) + '/../spec_helper'

context "a class definition" do
  specify "should create a new class" do
    example(<<-CODE
    class Foo; end
    CODE
    ) do
      p Foo.new.class
    end.should == 'Foo'
  end

  specify "should allow the declaration of cvars in the body" do
    example(<<-CODE
    class Foo
      @@bar = 'foo'
    end
    CODE
    ) do
      puts Foo.class_variables
    end.should == '@@bar'
  end

  specify "should allow the declaration of cvars in a method" do
    example(<<-CODE
    class Foo
      def initialize
        @@bar = 'foo'
      end
    end
    CODE
    ) do
      before = Foo.class_variables
      Foo.new
      after = Foo.class_variables 
      p [before, after]
    end.should == '[[], ["@@bar"]]'
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

