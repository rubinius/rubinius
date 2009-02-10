require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/have_instance_method'

describe MethodMatcher do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  after :all do
    $VERBOSE = @verbose
  end

  before :each do
    @ruby_version = Object.const_get :RUBY_VERSION

    @method = mock("method name")
  end

  after :each do
    Object.const_set :RUBY_VERSION, @ruby_version
  end

  it "converts the method name to a string if RUBY_VERSION < 1.9" do
    Object.const_set :RUBY_VERSION, "1.8.6"
    @method.should_receive(:to_s).and_return("method_name")
    MethodMatcher.new @method
  end

  it "does not convert the method name to a string if RUBY_VERSION >= 1.9" do
    Object.const_set :RUBY_VERSION, "1.9.0"
    @method.should_not_receive(:to_s)
    MethodMatcher.new @method
  end
end
