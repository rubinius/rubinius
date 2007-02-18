# Specifications:
#
# Keywords:
#   class
#   def
#
# Behaviour:
#   Constants
#   Class variables
#   Class instance variables
#   Instance variables
#
require File.dirname(__FILE__) + '/../spec_helper'

context "A class definition" do
  specify "should create a new class" do
    example do
      class Foo; end

      Foo.new.class.to_s
    end.should == "Foo"
  end

  specify "should allow the declaration of class variables in the body" do
    example do
      class Foo
        @@bar = 'foo'
      end

      Foo.class_variables
    end.should == ["@@bar"]
  end

  specify "should allow the declaration of class variables in a method" do
    example do
      class ClassSyntaxSpec
        def setup_cv
          @@bar = 'foo'
        end
      end

      @before = ClassSyntaxSpec.class_variables
      ClassSyntaxSpec.new.setup_cv
      @after = ClassSyntaxSpec.class_variables 

      [@before, @after]
    end.should == [[], ["@@bar"]]
  end
end

context "In a class definition" do
  specify "def should create a new method" do
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

