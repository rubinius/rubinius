require File.dirname(__FILE__) + '/../../spec_helper'

describe "UnboundMethod#aritiy" do
  it "returns the number of arguments self does take" do
    klass = Class.new do
      def one()
      end
      def two(a)
      end
      def three(a, b)
      end
      def four(a, b, &c)
      end
    end
    
    klass.instance_method(:one).arity.should == 0
    klass.instance_method(:two).arity.should == 1
    klass.instance_method(:three).arity.should == 2
    klass.instance_method(:four).arity.should == 2
  end

  it "returns the negative number of mandatory arguments - 1 if self does take optional arguments" do
    klass = Class.new do
      def one(*a)
      end
      def two(a, *b)
      end
      def three(a, b, *c)
      end
      def four(a, b, *c, &d)
      end
    end
    
    klass.instance_method(:one).arity.should == -1
    klass.instance_method(:two).arity.should == -2
    klass.instance_method(:three).arity.should == -3
    klass.instance_method(:four).arity.should == -3
  end
end