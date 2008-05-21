require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'

describe "OpenStruct.new" do
  it "returns a new OpenStruct Object without any attributes" do
    OpenStruct.new.to_s.should == "#<OpenStruct>"
  end
end

describe "OpenStruct.new when given a hash" do
  before :each do
    @os = OpenStruct.new(:name => "John Smith", :age => 70, :pension => 300)
  end
  
  it "has the given keys as attributes" do
    @os.age.should == 70
    @os.pension.should == 300
    @os.name.should == "John Smith"
  end
  
  it "responds to methods named after the hash keys" do
    @os.respond_to?(:age).should == true
    @os.respond_to?(:name).should == true
    @os.respond_to?(:pension).should == true
    
    @os.respond_to?(:some_test).should == false
    @os.respond_to?(:some_other_test).should == false
  end
  
  it "creates attributes dynamically" do
    @os.attr_1 = "foo"
    @os.attr_2 = [1,2,3]
    @os.attr_3 = { :foo => "bar" }

    @os.attr_1.should == "foo"
    @os.attr_2.should == [1,2,3]
    @os.attr_3.should == { :foo => "bar" }
  end

  it "does not allow adding methods named the same as one of OpenStruct method" do
    @os = OpenStruct.new(:marshal_dump => "overwritten")
    @os.respond_to?(:marshal_dump=).should == false
  end
end
