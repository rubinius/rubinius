require File.dirname(__FILE__) + '/../spec_helper'
require 'ostruct'

describe "OpenStruct.new" do
  it "should return a new OpenStruct Object without any attributes" do
    OpenStruct.new.to_s.should == "#<OpenStruct>"
  end
end

describe "OpenStruct.new when given a hash" do
  it "should return a new OpenStruct Object with the given keys as attributes" do
    os = OpenStruct.new(:name => "John Smith", :age => 70, :pension => 300)
    str = os.to_s
    str.index('#<OpenStruct').should == 0
    str.should include('age=70')
    str.should include('name="John Smith"')
    str.should include('pension=300')
    str.index('>').should == str.size - 1
    
    os.age.should == 70
    os.pension.should == 300
    os.name.should == "John Smith"
    
    os.respond_to?(:age).should == true
    os.respond_to?(:name).should == true
    os.respond_to?(:pension).should == true
    
    os.respond_to?(:some_test).should == false
    os.respond_to?(:some_other_test).should == false
  end
  
  it "should not allow adding methods named the same as one of OpenStruct method" do
    os = OpenStruct.new(:marshal_dump => "overwritten")
    os.respond_to?(:marshal_dump=).should == false
  end
end