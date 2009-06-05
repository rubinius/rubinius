# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#capitalize" do
  it "returns a copy of self with the first character converted to uppercase and the remainder to lowercase" do
    "".capitalize.should == ""
    "h".capitalize.should == "H"
    "H".capitalize.should == "H"
    "hello".capitalize.should == "Hello"
    "HELLO".capitalize.should == "Hello"
    "123ABC".capitalize.should == "123abc"
  end

  it "taints resulting string when self is tainted" do
    "".taint.capitalize.tainted?.should == true
    "hello".taint.capitalize.tainted?.should == true
  end

  it "is locale insensitive (only upcases a-z and only downcases A-Z)" do
    "ÄÖÜ".capitalize.should == "ÄÖÜ"
    "ärger".capitalize.should == "ärger"
    "BÄR".capitalize.should == "BÄr"
  end

  it "returns subclass instances when called on a subclass" do
    StringSpecs::MyString.new("hello").capitalize.class.should == StringSpecs::MyString
    StringSpecs::MyString.new("Hello").capitalize.class.should == StringSpecs::MyString
  end
end

describe "String#capitalize!" do
  it "capitalizes self in place" do
    a = "hello"
    a.capitalize!.should equal(a)
    a.should == "Hello"
  end

  it "returns nil when no changes are made" do
    a = "Hello"
    a.capitalize!.should == nil
    a.should == "Hello"

    "".capitalize!.should == nil
    "H".capitalize!.should == nil
  end

  ruby_version_is ""..."1.9" do 
    it "raises a TypeError when self is frozen" do
      ["", "Hello", "hello"].each do |a|
        a.freeze
        lambda { a.capitalize! }.should raise_error(TypeError)
      end
    end
  end

  ruby_version_is "1.9" do 
    it "raises a RuntimeError when self is frozen" do
      ["", "Hello", "hello"].each do |a|
        a.freeze
        lambda { a.capitalize! }.should raise_error(RuntimeError)
      end
    end
  end
end
