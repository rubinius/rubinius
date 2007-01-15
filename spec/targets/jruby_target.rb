require 'rubygems'
require 'spec'
require File.dirname(__FILE__) + '/../jruby_target'

class JRubyTargetSpec
  include JRubyTarget
end

context "JRubyTarget" do
  setup do
    @target = JRubyTargetSpec.new
  end

  specify "should provide example method" do
    @target.should_respond_to :example
  end

  specify "example should take a default argument" do
    @target.method(:example).arity.should == -1
  end

  specify "example should receive a block" do
    lambda { @target.example }.should_raise ArgumentError
  end
  
  specify "should provide a code method" do
    @target.should_respond_to :code
  end
end
