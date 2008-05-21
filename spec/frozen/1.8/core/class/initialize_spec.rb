require File.dirname(__FILE__) + '/../../spec_helper'

describe "Class#new" do
  it "returns a new instance of self" do
    klass = Class.new
    klass.new.is_a?(klass).should == true
  end
  
  it "invokes #initialize on the new instance with the given args" do
    klass = Class.new do
      def initialize(*args)
        @initialized = true
        @args = args
      end
      
      def args
        @args
      end
      
      def initialized?
        @initialized || false
      end
    end
    
    klass.new.initialized?.should == true
    klass.new(1, 2, 3).args.should == [1, 2, 3]
  end
end