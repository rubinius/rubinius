require File.dirname(__FILE__) + '/../spec_helper'

context "A class definition" do
  specify "should create a new class" do
    example do
      class Foo; end

      Foo.new.class.to_s
    end.should == "Foo"
  end

  specify "should allow the declaration of cvars in the body" do
    example do
      class Foo
        @@bar = 'foo'
      end

      Foo.class_variables
    end.should == ["@@bar"]
  end

  specify "should allow the declaration of cvars in a method" do
    example do
      class Foo
        def initialize
          @@bar = 'foo'
        end
      end

      @before = Foo.class_variables
      Foo.new
      @after = Foo.class_variables 
      [@before, @after]
    end.should == [[], ["@@bar"]]
  end
end

context "def" do
  specify "should create a new method" do
    example do
      class Foo
        def foo
          'foo'
        end
      end

      Foo.new.foo
    end.should == 'foo'
  end
end

