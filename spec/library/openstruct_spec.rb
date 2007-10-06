require File.dirname(__FILE__) + '/../spec_helper'
require 'ostruct'

describe "OpenStruct" do
  before(:each) do
    @os = OpenStruct.new("foo" => "baz", "baz" => "bar")
  end

  it "should create an OpenStruct with attributes" do
    @os.foo.should == "baz"
    @os.baz.should == "bar"
  end

  it "should create attributes dynamically" do
    @os.attr_1 = "foo"
    @os.attr_2 = [1,2,3]
    @os.attr_3 = { :foo => "bar" }

    @os.attr_1.should == "foo"
    @os.attr_2.should == [1,2,3]
    @os.attr_3.should == { :foo => "bar" }
  end
end
