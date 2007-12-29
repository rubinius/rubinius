require File.dirname(__FILE__) + '/../spec_helper'
require 'singleton'

module SingletonSpecs
  class MyClass
    attr_accessor :data
    include Singleton
  end
end

describe "Singleton" do
  it "should make the new method private" do
    lambda { SingletonSpecs::MyClass.new }.should raise_error(NoMethodError)
  end

  it "should provide an instance method" do
    SingletonSpecs::MyClass.instance.kind_of?(SingletonSpecs::MyClass).should == true
  end

  it "two instances of the class should return the same object" do
    a = SingletonSpecs::MyClass.instance
    b = SingletonSpecs::MyClass.instance
    a.data = 123

    a.should == b
    b.data.should == 123
  end
end
