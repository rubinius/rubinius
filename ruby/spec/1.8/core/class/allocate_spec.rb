require File.dirname(__FILE__) + '/../../spec_helper'

describe "Class#allocate" do
  it "returns an instance of self" do
    klass = Class.new
    klass.allocate.is_a?(klass).should == true
  end
  
  it "does not call initialize on the new instance" do
    klass = Class.new do
      def initialize(*args)
        @initialized = true
      end
      
      def initialized?
        @initialized || false
      end
    end
    
    klass.allocate.initialized?.should == false
  end
end