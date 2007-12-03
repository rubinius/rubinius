require File.dirname(__FILE__) + '/../spec_helper'
require 'singleton'

class MyClass
  attr_accessor :data
  include Singleton
end

describe "Singleton" do
  it "should make the new method private" do
    lambda { MyClass.new }.should raise_error(NoMethodError)
  end

  it "should provide an instance method" do
    MyClass.instance.kind_of?(MyClass).should == true
  end

  it "two instances of the class should return the same object" do
    a = MyClass.instance
    b = MyClass.instance
    a.data = 123

    a.should == b
    b.data.should == 123
  end
end
