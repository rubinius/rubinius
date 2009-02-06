require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/have_private_instance_method'

class HPIMMSpecs
  private

  def private_method
  end

  class Subclass < HPIMMSpecs
    private

    def private_sub_method
    end
  end
end

describe HavePrivateInstanceMethodMatcher do
  it "matches when mod has the private instance method" do
    matcher = HavePrivateInstanceMethodMatcher.new :private_method
    matcher.matches?(HPIMMSpecs).should be_true
    matcher.matches?(HPIMMSpecs::Subclass).should be_true
  end

  it "does not match when mod does not have the private instance method" do
    matcher = HavePrivateInstanceMethodMatcher.new :another_method
    matcher.matches?(HPIMMSpecs).should be_false
  end

  it "does not match if the method is in a superclass and include_super is false" do
    matcher = HavePrivateInstanceMethodMatcher.new :private_method, false
    matcher.matches?(HPIMMSpecs::Subclass).should be_false
  end

  it "provides a failure message for #should" do
    matcher = HavePrivateInstanceMethodMatcher.new :some_method
    matcher.matches?(HPIMMSpecs)
    matcher.failure_message.should == [
      "Expected HPIMMSpecs to have private method 'some_method'",
      "but it does not"
    ]
  end

  it "provides a failure messoge for #should_not" do
    matcher = HavePrivateInstanceMethodMatcher.new :some_method
    matcher.matches?(HPIMMSpecs)
    matcher.negative_failure_message.should == [
      "Expected HPIMMSpecs NOT to have private method 'some_method'",
      "but it does"
    ]
  end
end

describe HavePrivateInstanceMethodMatcher do
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
    HavePrivateInstanceMethodMatcher.new @method
  end

  it "does not convert the method name to a string if RUBY_VERSION >= 1.9" do
    Object.const_set :RUBY_VERSION, "1.9.0"
    @method.should_not_receive(:to_s)
    HavePrivateInstanceMethodMatcher.new @method
  end
end
